#include "audio/ChannelBase.hpp"

ChannelBase::ChannelBase(ChannelType type)
	: m_Type(type),

	// Emplace all the parameters
	volume(Emplace<VolumeSlider>(volumeParam)),
	pan(Emplace<PanSlider>(panParam)),
	name(Emplace<SMXRTextComponent>("", 14, false)),
	volumeVal(Emplace<SMXRTextComponent>("0.0dB")),
	mute(Emplace<Button<MuteButton, ButtonType::Toggle>>("MUTE")),
	mono(Emplace<Button<MonoButton, ButtonType::Toggle>>("MONO")),
	route(Emplace<Button<RouteButton, ButtonType::Toggle>>((type & Type::Input) ? "in" : ""))
{
	// Standard width of channel is 70.
	Width(70);

	// Parameter settings
	volumeVal.AlignLines(Align::LEFT);
	name.AlignLines(Align::CENTER);
	name.Editable(true);
	route.Disable();
	pan.Name("Pan");
	pan.DisplayName(false);

	// Unfocus when pressing enter
	name.Listener() += [this](Event::KeyTyped& e)
	{
		if (e.key == Key::ENTER && Focused())
		{
			Event e{ Event::Type::Unfocused };
			name.AddEvent(e);
			name.Focused(false);
			m_FocusedComponent = nullptr;
		};
	};

	// Layout is divs, for easy complex layout.
	Layout<Layout::Divs>();
	Div() = { 7, Div::Alignment::Vertical, 0, false, Div::AUTO };
	Div()[6] = { name, Div::Alignment::Center, 28, true };
	Div()[5].DivSize(14);
	Div()[4] = { 2, Div::Alignment::Horizontal, 0, false, Div::AUTO };
	Div()[4][0] = { volume, Div::Alignment::Left, Div::AUTO, true };
	Div()[4][1].DivSize(24);
	Div()[3] = { 2, Div::Alignment::Horizontal, 0, false, 24 };
	Div()[3][0].DivSize(6);
	Div()[3][1] = { volumeVal, Div::Alignment::Center, Div::AUTO, true };
	Div()[2] = { 4, Div::Alignment::Horizontal, 3, false, 32 };
	Div()[2][0].DivSize(2);
	Div()[2][1] = { mono, Div::Alignment::Center, Div::AUTO, true };
	Div()[2][2] = { mute, Div::Alignment::Center, Div::AUTO, true };
	Div()[2][3].DivSize(2);
	Div()[1] = { 3, Div::Alignment::Horizontal, 2, false, 22 };
	Div()[1][0].DivSize(2);
	Div()[1][1] = { pan, Div::Alignment::Center, Div::AUTO, true };
	Div()[1][2].DivSize(2);
	Div()[0] = { route, Div::Alignment::Center, 25, true };

	/**
		* Listener for mousepressed which sets the static selected to 'this' if it's
		* not hovering over any buttons.
		*/
	m_Listener += [this](Event::MousePressed& e)
	{
		// Only select if not hovering over any button/parameter
		bool hovering = pan.Hovering() || mute.Hovering() ||
			mono.Hovering() || (route.Hovering() && !route.Disabled());

		if (!hovering && e.button == Event::MouseButton::LEFT)
			selected = this;
	};
}

void ChannelBase::Connect(ChannelBase* c)
{
	if (!std::contains(m_Connections, c))
	{
		m_Lock.lock();
		m_Connections.push_back(c);
		m_Lock.unlock();
	}
}

void ChannelBase::Disconnect(ChannelBase* c)
{
	auto& it = std::find(m_Connections.begin(), m_Connections.end(), c);
	if (it != m_Connections.end())
	{
		m_Lock.lock();
		m_Connections.erase(it);
		m_Lock.unlock();
	}
}

void ChannelBase::Input(float s, int c)
{
	m_Levels[c % m_Lines] += s;
}

void ChannelBase::Process()
{
	float _avg = 0;
	bool _mono = mono.Active();

	// Input
	if (Type() & ChannelBase::Type::Input)
	{
		// Go through all the lines 
		for (int i = 0; i < m_Lines; i++)
		{
			float _sample = m_Levels[i];

			// If muted set sample to 0
			if (!mute.Active())
			{
				_sample = m_EffectChain.NextSample(_sample, i);
				_sample *= volume.Value();
				_sample *= m_Pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly add to all connections
			if (!_mono)
			{
				for (auto& j : Connections())
					j->Input(_sample, i);

				// Set peak, for the volume slider meter.
				if (_sample > m_Peaks[i])
					m_Peaks[i] = _sample;
				if (-_sample > m_Peaks[i])
					m_Peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= Lines();

		// If mono, send the avg sample to all lines of all connected channels
		if (_mono)
		{
			// The connections will get averaged levels, but panning will still be applied
			// panning will be applied as if there were the same amount of channels (n), but 
			// any channel above n will receive the pan of x mod n channel. So example:
			// this has 2 channels, connection 4, channel 3 of connection will get pan of channel 1.
			for (auto& j : Connections())
				for (int i = 0; i < j->Lines(); i++)
				{
					float _level = _avg * m_Pans[i % Lines()];
					j->Input(_level, i);
				}

			// Set peaks for volume slider meter to mono, panning will be applied after monoing.
			for (int i = 0; i < Lines(); i++)
			{
				float _level = _avg * m_Pans[i];

				// Set peak, for the volume slider meter.
				if (_level > m_Peaks[i])
					m_Peaks[i] = _level;
				if (-_level > m_Peaks[i])
					m_Peaks[i] = -_level;
			}
		}
	}

	// Otherwise it's output
	else
	{
		float _avg = 0;
		bool _mono = mono.Active();

		// Go through all the lines
		for (int i = 0; i < m_Lines; i++)
		{
			float _sample = m_Levels[i];

			// If muted set sample to 0
			if (!mute.Active())
			{
				_sample = m_EffectChain.NextSample(_sample, i);
				_sample *= volume.Value();
				if (!_mono)
					_sample *= m_Pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly send to endpoints
			if (!_mono)
			{
				m_Levels[i] = constrain(_sample, -1, 1);

				// Set peak, for the volume slider meter.
				if (_sample > m_Peaks[i])
					m_Peaks[i] = _sample;
				if (-_sample > m_Peaks[i])
					m_Peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= Lines();

		// If mono, send the avg sample to all endpoints
		if (_mono)
		{
			for (int i = 0; i < Lines(); i++)
			{
				float _level = _avg * m_Pans[i];
				m_Levels[i] = constrain(_level, -1, 1);

				// Set peak, for the volume slider meter.
				if (_level > m_Peaks[i])
					m_Peaks[i] = _level;
				if (-_level > m_Peaks[i])
					m_Peaks[i] = -_level;
			}
		}
	}

	// Every 512 samples, set the value for the volume slider meter.
	m_Counter++;
	if (m_Counter > 512)
	{
		// Set the levels of the volume slider
		for (int i = 0; i < Lines(); i++)
		{
			float r = volume.Slider().Reduces()[i];
			volume.Slider().SetReduce(i, r * 0.8 + 0.2 * m_Peaks[i]);
			m_Peaks[i] = 0;
		}

		m_Counter = 0;
	}
}

bool ChannelBase::Connected(ChannelBase* c) const
{
	// If it's an input it stores the connections
	if (Type() & Type::Input)
		return std::find(m_Connections.begin(), m_Connections.end(), c) != m_Connections.end();

	// Otherwise it's an output, so check if param is input and is connected to this
	else if (c && (c->Type() & Type::Input))
		return std::find(c->Connections().begin(), c->Connections().end(), this) != c->Connections().end();
}

void ChannelBase::Lines(int c)
{
	// Don't update if same.
	if (c == m_Lines)
		return;

	// Make sure the volume parameter has the same amount of channels.
	volumeParam.Channels(c);

	// Emplace to levels.
	m_Levels.reserve(c);
	while (m_Levels.size() < c)
		m_Levels.push_back(0);

	// Emplace to peaks.
	m_Peaks.reserve(c);
	while (m_Peaks.size() < c)
		m_Peaks.push_back(0);

	// Emplace to pans.
	m_Pans.reserve(c);
	while (m_Pans.size() < c)
		m_Pans.push_back(0);

	// Also make sure the effect chain has the amount of lines.
	m_EffectChain.Lines(c);

	// Set lines
	m_Lines = c;
}

void ChannelBase::Update(const Vec4<int>& v)
{
	// Update the pan values for each line.
	UpdatePans();

	// If new channel was selected, make sure the state of the route button matches routing.
	if (m_Prev != selected)
	{
		m_Prev = selected;

		// Disable the route button if m_Selected is nullptr or
		// if the type of the channel is the same as this channel, but only for input/output
		if (!selected || (selected->Type() & Type() & (Type::Input | Type::Output)))
			route.Disable();
		else
			route.Enable(), route.Active(Connected(selected));
	}

	// Update connections given the state of the route button.
	if (selected && selected != this)
		if (!(Type() & Type::Input))
			if (route.Active())
				selected->Connect(this);
			else
				selected->Disconnect(this);
		else if (Type() & Type::Input)
			if (route.Active())
				Connect(selected);
			else
				Disconnect(selected);

	// Set volume value in text component.
	volumeVal.Content(volume.ValueText());

	// Background color of the panel using theme.
	Color c, c2;
	if (selected == this)
		c = ThemeT::Get().channel_active_background,
		c2 = ThemeT::Get().channel_active_name_text;
	else if (Hovering())
		c = ThemeT::Get().channel_hovering_background,
		c2 = ThemeT::Get().channel_hovering_name_text;
	else
		c = ThemeT::Get().channel_idle_background,
		c2 = ThemeT::Get().channel_idle_name_text;

	volumeVal.TextColor(c2);
	name.TextColor(c2);
	Background(c);

	Panel::Update(v);
}

void ChannelBase::UpdatePans()
{
	double _p = pan.Value() / 50.0;
	double _a = 1.0 - std::abs((Lines() - 1) / 2.0 * _p);
	for (int i = 0; i < Lines(); i++)
		m_Pans[i] = constrain(_p * (i - (Lines() - 1) / 2.0) + _a, 0.0, 1.0);
}
