#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

ListPanel::ListPanel(AsioDevice& sarasio)
	: asio(sarasio), 
	m_Channels(Panel<::Panel>().Emplace<::SMXRScrollPanel>(Layout::Hint::Center)),
	m_Effect(Panel().Emplace<::EffectScrollPanel>(Layout::Hint::East)),
	m_Inputs(m_Channels.Panel<::Panel>().Emplace<::Panel>()),
	m_Divider(&m_Channels.Panel().Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0)),
	m_Outputs(m_Channels.Panel().Emplace<::Panel>())
{
	m_Effect.Width(300);
	m_Effect.MinWidth(300);
	m_Effect.MaxWidth(500);
	m_Effect.Hide();
	m_Channels.Panel<::Panel>();
	m_Channels.Panel().Layout<Layout::SidewaysStack>(0);
	m_Channels.Panel().AutoResize(true, false);
	m_Channels.MinWidth(100);
	m_Channels.EnableScrollbars(true, false);
	Background(Theme<C::MainPanel>::Get());

	Panel().Background(Theme<C::MainPanel>::Get());
	Panel().Layout<Layout::Border>(0, 8, true, true, true, true);
	Panel().AutoResize(false, false);

	m_Inputs.AutoResize(true, false);
	m_Inputs.Layout<Layout::SidewaysStack>(8);
	
	m_Outputs.AutoResize(true, false);
	m_Outputs.Layout<Layout::SidewaysStack>(8);

	Background(Color{ 38, 38, 38, 355 });
	EnableScrollbars(false, false);

	m_Channels.Scrolled().x;

	m_Listener += [this](Event::MousePressed& e)
	{
		if (!m_Channels.WithinBounds({ e.x, e.y }))
			return;

		Vec2<int> translated = { 
			e.x - X() - m_Channels.X() - m_Channels.Panel().X() - Panel().X(),
			e.y - Y() - m_Channels.Y() - m_Channels.Panel().Y() - Panel().Y()
		};

		if (e.button == Event::MouseButton::LEFT)
		{
			bool s = false;
			for (auto& _panel : m_InputChannels)
			{
				InputChannelPanel& _current = *_panel;
				if (_current.WithinBounds(translated - m_Inputs.Position()))
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_OutputChannels)
						{
							OutputChannelPanel& _c = *_p;
							_c.Select(&_current.ChannelGroup());
							_c.Selected(false);
						}
						for (auto& _p : m_InputChannels)
						{
							InputChannelPanel& _c = *_p;
							_c.Select(&_current.ChannelGroup());
							_c.Selected(false);
						}
						_current.Selected(true);
						if (m_Effect.Visible())
							ShowEffectsPanel(_current.ChannelGroup().EffectsGroup());
					}
					s = true;
				}
			}
			for (auto& _panel : m_OutputChannels)
			{
				ChannelPanel<::OutputChannelGroup>& _current = *_panel;
				if (_current.WithinBounds(translated - m_Outputs.Position()))
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_InputChannels)
						{
							ChannelPanel<::InputChannelGroup>& _c = *_p;
							_c.Select(&_current.ChannelGroup());
							_c.Selected(false);
						}
						for (auto& _p : m_OutputChannels)
						{
							OutputChannelPanel& _c = *_p;
							_c.Select(&_current.ChannelGroup());
							_c.Selected(false);
						}
						_current.Selected(true);
						if (m_Effect.Visible())
							ShowEffectsPanel(_current.ChannelGroup().EffectsGroup());
					}
					s = true;
				}
			}
			if (!s && !m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
			{
				for (auto& _p : m_InputChannels)
				{
					_p->Selected(false);
					_p->Unselect();
				}
				for (auto& _p : m_OutputChannels)
				{
					_p->Selected(false);
					_p->Unselect();
				}
			}
		}
	};

	m_Listener += [this](Event::Unfocused& e)
	{
		if (!m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
		{
			for (auto& _p : m_InputChannels)
			{
				_p->Selected(false);
				_p->Unselect();
			}
			for (auto& _p : m_OutputChannels)
			{
				_p->Selected(false);
				_p->Unselect();
			}
		}
	};
}

void ListPanel::SortChannels()
{
	std::sort(m_Inputs.Components().begin(), m_Inputs.Components().end(),
		[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
		{
			InputChannelPanel* _ina = dynamic_cast<InputChannelPanel*>(a.get());
			InputChannelPanel* _inb = dynamic_cast<InputChannelPanel*>(b.get());

			if (_ina != nullptr && _inb != nullptr)
				return _ina->ChannelGroup().ID() < _inb->ChannelGroup().ID();

			return -1;
		});

	std::sort(m_Outputs.Components().begin(), m_Outputs.Components().end(),
		[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
		{
			OutputChannelPanel* _ina = dynamic_cast<OutputChannelPanel*>(a.get());
			OutputChannelPanel* _inb = dynamic_cast<OutputChannelPanel*>(b.get());

			if (_ina != nullptr && _inb != nullptr)
				return _ina->ChannelGroup().ID() < _inb->ChannelGroup().ID();

			return -1;
		});
}

void ListPanel::ResetGrouping()
{
	m_Inputs.Clear();
	m_Outputs.Clear();
	m_InputChannels.clear();
	m_OutputChannels.clear();

	int i = 0;
	for (i = 0; i < asio.Device().info.maxInputChannels - 1; i += 2)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel<InputChannelPanel>();
		_c.AddChannel(&asio.Inputs()[i]);
		_c.AddChannel(&asio.Inputs()[i + 1]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.SliderValue(0);
		_c.volume.SliderValue(1);
	}

	// if there were an uneven amount of channels, add one last mono channel
	if (i == asio.Device().info.maxInputChannels - 1)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel<InputChannelPanel>();
		_c.AddChannel(&asio.Inputs()[i]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.SliderValue(0);
		_c.volume.SliderValue(1);
	}

	for (i = 0; i < asio.Device().info.maxOutputChannels - 1; i += 2)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel<OutputChannelPanel>();
		_c.AddChannel(&asio.Outputs()[i]);
		_c.AddChannel(&asio.Outputs()[i + 1]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.SliderValue(0);
		_c.volume.SliderValue(1);
	}

	// if there were an uneven amount of channels, add one last mono channel
	if (i == asio.Device().info.maxOutputChannels - 1)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel<OutputChannelPanel>();
		_c.AddChannel(&asio.Outputs()[i]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.SliderValue(0);
		_c.volume.SliderValue(1);
	}
}

void ListPanel::Update(const Vec4<int>& s)
{
	for (auto& c = m_InputChannels.begin(); c != m_InputChannels.end(); ++c)
	{
		if ((*c)->Delete())
		{
			m_Inputs.Erase(std::remove_if(m_Inputs.Components().begin(), m_Inputs.Components().end(),
				[&](const std::unique_ptr<Component>& const a)
				{
					return dynamic_cast<Component*>(*c) == a.get();
				}
			));
			m_InputChannels.erase(c);
			break;
		}
	}

	for (auto& c = m_OutputChannels.begin(); c != m_OutputChannels.end(); ++c)
		if ((*c)->Delete())
		{
			m_Outputs.Erase(std::remove_if(m_Outputs.Components().begin(), m_Outputs.Components().end(),
				[&](const std::unique_ptr<Component>& const a)
				{
					return dynamic_cast<Component*>(*c) == a.get();
				}
			));
			m_OutputChannels.erase(c);
			break;
		}

	if (m_Divider)
		m_Divider->Color(Theme<C::Divider>::Get());

	ScrollPanel::Update(s);
}