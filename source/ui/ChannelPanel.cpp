#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

void ChannelPanel::Select(ChannelPanel* s)
{
	pan.DisplayName(false);
	if (s->IsInput() == m_IsInput)
	{
		m_HasSelect = false;
		m_Routed = false;
		routed.Disable();

		if (&s->ChannelGroup() != &m_ChannelGroup && !m_IsSpecial && !s->IsSpecial())
		{
			m_Connect->Name(std::string("Combine with ") + s->ChannelGroup().Name());
			m_Connect->Visible(true);
		}
		else
			m_Connect->Visible(false);

		m_SelectedSame = &s->ChannelGroup();
	}
	else
	{
		m_SelectedChannels = &s->ChannelGroup();
		m_HasSelect = true;

		if (m_IsInput)
			m_Routed = m_ChannelGroup.Connected(&s->ChannelGroup());

		else
			m_Routed = s->ChannelGroup().Connected(&m_ChannelGroup);

		routed.Enable();
		m_Connect->Visible(false);
	}
}

void ChannelPanel::AddChannel(Channel* s)
{
	m_ChannelGroup.AddChannel(s);
	m_MenuTitle->Name(m_ChannelGroup.Name());
	m_IsInput = s->IsInput();
}

void ChannelPanel::Update(const Vec4<int>& viewport)
{
	if (m_Counter)
		m_Counter--;

	if (m_HasSelect)
		if (!m_IsInput)
			if (m_Routed)
				m_SelectedChannels->Connect(&m_ChannelGroup);
			else
				m_SelectedChannels->Disconnect(&m_ChannelGroup);
		else if (m_IsInput)
			if (m_Routed)
				m_ChannelGroup.Connect(m_SelectedChannels);
			else
				m_ChannelGroup.Disconnect(m_SelectedChannels);

	Color c;
	if (Selected())
		c = theme->Get(C::ChannelSelected);
	else
		c = theme->Get(C::Channel);
	c.a = m_Transparency ? 245.0f : 255.0f;
	Background(c);

	//m_Div1->Color(theme->Get(C::Divider));
	m_Div2->Color(theme->Get(C::Divider));
	m_Div3->Color(theme->Get(C::Divider));
	m_Div4->Color(theme->Get(C::Divider));
	m_Div3->Visible(m_Connect->Visible() || m_Split->Visible());
	m_Split->Visible(m_ChannelGroup.Channels().size() > 1 && !m_IsSpecial);

	volume.Size(Vec2<int>{50, Height() - 135});

	m_MenuMono->Active(m_ChannelGroup.Mono());
	m_MenuMuted->Active(m_ChannelGroup.Muted());
	mono.Active(m_ChannelGroup.Mono());
	muted.Active(m_ChannelGroup.Muted());

	m_ChannelGroup.Volume(volume.Value());
	m_ChannelGroup.Pan(pan.Value());

	Panel::Update(viewport);
}

void ChannelPanel::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	Background(d);

	int _channels = m_ChannelGroup.ChannelAmount();

	int _width = std::floor((_channels-1) / 4 + 1) * 32.0;

	int _y = 100;
	int _rh = Height() - 35 - _y;
	int _0db = ((1.0 / 1.412536) * (_rh)) + _y;
	int _3db = ((std::powf(std::powf(10, 3 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
	int _6db = ((std::powf(std::powf(10, 6 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
	int _w = (_width / _channels) - 2;
	int _x = 10;

	// Draw all audio meters
	for (int i = 0; i < _channels; i++)
	{
		_x = 10 + i * (_w + 2);
		float _level = std::powf(m_ChannelGroup.Channels()[i]->Peak(), 0.25);

		int _h = (std::min(_level, 1.412536f) / 1.412536) * (_rh);
		d.Command<Graphics::Fill>(theme->Get(C::VMeter));
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
		d.Command<Graphics::Fill>(theme->Get(C::Channel));
		d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
		d.Command<Graphics::Fill>(theme->Get(C::VMeterFill));
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		if (_level > 1.0)
		{
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFillC1));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, _h - _0db + _y});
		}
		if (_level > std::powf(std::powf(10, 3 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFillC2));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _3db, _w, _h - _3db + _y});
		}
		if (_level > std::powf(std::powf(10, 6 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFillC3));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _6db, _w, _h - _6db + _y});
		}
	}

	Width(_x + 30 + _w);
	volume.Width(_x + _w + 10);
	routed.Width(Width());
	pan.Position(Vec2<int>{Width() / 2 - 31, 27});
	muted.Position(Vec2<int>{Width() / 2 - 30, 50});
	mono.Position(Vec2<int>{Width() / 2 + 3, 50});

	// Volume level
	d.Command<Font>(Gidole14, 14.0f);
	d.Command<Fill>(theme->Get(C::TextSmall));
	d.Command<TextAlign>(Align::CENTER, Align::TOP);
	d.Command<Text>(&volume.ValueText(), Vec2<int>{volume.X() + 6 + (volume.Width() - 6 * 2) / 2, volume.Y()});

	// Channel name
	d.Command<Font>(Gidole14, 14.0f);
	d.Command<Fill>(theme->Get(C::TextSmall));
	d.Command<TextAlign>(Align::CENTER, Align::TOP);
	d.Command<Text>(&m_ChannelGroup.Name(), Vec2<int>{ Width() / 2, Height() - 4 });


	// db numbers besides volume meter
	int _d = 3;
	bool _b = true;
	d.Command<Graphics::Font>(Gidole14, 14.0f);
	d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
	for (int i = 12; i > -120; i -= _d)
	{
		if (i < -11)
			_d = 6;
		if (i < -47)
			_d = 12;
		if (i < -71)
			_d = 24;

		if (_b)
			d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
		else
			d.Command<Graphics::Fill>(theme->Get(C::VMeterIndD));

		int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
		if (_b)
		{
			if (m_Numbers.find(i) == m_Numbers.end())
			{
				m_Numbers.emplace(i, std::to_string(std::abs(i)));
			}
			d.Command<Graphics::Fill>(theme->Get(C::TextOff));
			d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
		}
		_b ^= true;
	}
	d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
	d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});
	d.Command<Graphics::Fill>(theme->Get(C::TextOff));
	d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y});

	Container::Render(d);
	d.Command<PopMatrix>();
}
