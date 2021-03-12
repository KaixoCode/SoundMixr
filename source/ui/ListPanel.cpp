#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

ListPanel::ListPanel(AsioDevice& sarasio)
	: asio(sarasio),
	m_ChannelsPanel(Panel<::Panel>().Emplace<::SMXRScrollPanel>(Layout::Hint::Center)),
	m_Effect(Panel().Emplace<::EffectScrollPanel>(Layout::Hint::East)),
	m_Inputs(m_ChannelsPanel.Panel<::Panel>().Emplace<::Panel>()),
	m_Divider(&m_ChannelsPanel.Panel().Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0)),
	m_Outputs(m_ChannelsPanel.Panel().Emplace<::Panel>()),
	m_Divider2(&m_ChannelsPanel.Panel().Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0)),
	m_Specials(m_ChannelsPanel.Panel().Emplace<::Panel>())
{
	m_Effect.Width(332);
	m_Effect.MinWidth(332);
	m_Effect.Hide();
	m_ChannelsPanel.Panel<::Panel>();
	m_ChannelsPanel.Panel().Layout<Layout::SidewaysStack>(0);
	m_ChannelsPanel.Panel().AutoResize(true, false);
	m_ChannelsPanel.MinWidth(200);
	m_ChannelsPanel.EnableScrollbars(true, false);
	Background(theme->Get(C::MainPanel));

	Panel().Background(theme->Get(C::MainPanel));
	Panel().Layout<Layout::Border>(0, 8, false, false, false, false);
	Panel().AutoResize(false, false);

	m_Inputs.AutoResize(true, false);
	m_Inputs.Layout<Layout::SidewaysStack>(8);
	
	m_Outputs.AutoResize(true, false);
	m_Outputs.Layout<Layout::SidewaysStack>(8);

	m_Specials.AutoResize(true, false);
	m_Specials.Layout<Layout::SidewaysStack>(8);
	m_Specials.Hide();
	m_Divider2->Hide();

	Background(Color{ 38, 38, 38, 355 });
	EnableScrollbars(false, false);

	m_ChannelsPanel.Scrolled().x;

	m_Listener += [this](Event::MousePressed& e)
	{
		if (!m_ChannelsPanel.WithinBounds({ e.x, e.y }))
			return;

		Vec2<int> translated = { 
			e.x - X() - m_ChannelsPanel.X() - m_ChannelsPanel.Panel().X() - Panel().X(),
			e.y - Y() - m_ChannelsPanel.Y() - m_ChannelsPanel.Panel().Y() - Panel().Y()
		};

		if (e.button == Event::MouseButton::LEFT)
		{
			bool s = false;
			for (auto& _panel : m_Channels)
			{
				ChannelPanel& _current = *_panel;

				auto& t = translated;

				if (_current.Hovering())
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_Channels)
						{
							ChannelPanel& _c = *_p;
							_c.Select(&_current);
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
				for (auto& _p : m_Channels)
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
			for (auto& _p : m_Channels)
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
			ChannelPanel* _ina = dynamic_cast<ChannelPanel*>(a.get());
			ChannelPanel* _inb = dynamic_cast<ChannelPanel*>(b.get());

			if (_ina != nullptr && _inb != nullptr)
				return _ina->ChannelGroup().ID() < _inb->ChannelGroup().ID();

			return -1;
		});

	std::sort(m_Outputs.Components().begin(), m_Outputs.Components().end(),
		[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
		{
			ChannelPanel* _ina = dynamic_cast<ChannelPanel*>(a.get());
			ChannelPanel* _inb = dynamic_cast<ChannelPanel*>(b.get());

			if (_ina != nullptr && _inb != nullptr)
				return _ina->ChannelGroup().ID() < _inb->ChannelGroup().ID();

			return -1;
		});
}

void ListPanel::ResetGrouping()
{
	for (auto& i : asio.Inputs())
		i.Group(nullptr, -1);

	for (auto& i : asio.Outputs())
		i.Group(nullptr, -1);

	for (auto& i : asio.SoundboardChannels())
		i.Group(nullptr, -1);
	Clear();

	int i = 0;
	for (i = 0; i < asio.Device().info.maxInputChannels - 1; i += 2)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel(true);
		_c.AddChannel(&asio.Inputs()[i]);
		_c.AddChannel(&asio.Inputs()[i + 1]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.Value(0);
		_c.volume.Value(1);
	}

	// if there were an uneven amount of channels, add one last mono channel
	if (i == asio.Device().info.maxInputChannels - 1)
	{
		// Add a ChannelPanel with all the inputs
		auto& _c = EmplaceChannel(true);
		_c.AddChannel(&asio.Inputs()[i]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.Value(0);
		_c.volume.Value(1);
	}

	for (i = 0; i < asio.Device().info.maxOutputChannels - 1; i += 2)
	{
		// Add a ChannelPanel with all the outputs
		auto& _c = EmplaceChannel(false);
		_c.AddChannel(&asio.Outputs()[i]);
		_c.AddChannel(&asio.Outputs()[i + 1]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.Value(0);
		_c.volume.Value(1);
	}

	// if there were an uneven amount of channels, add one last mono channel
	if (i == asio.Device().info.maxOutputChannels - 1)
	{
		// Add a ChannelPanel with all the outputs
		auto& _c = EmplaceChannel(false);
		_c.AddChannel(&asio.Outputs()[i]);

		// Set all parameters of this Channel
		_c.mono.Active(false);
		_c.muted.Active(false);
		_c.pan.Value(0);
		_c.volume.Value(1);
	}

	auto& _soundboardChannel = EmplaceSpecialChannel();
	for (auto& a : asio.SoundboardChannels())
		_soundboardChannel.AddChannel(&a);
}

void ListPanel::Update(const Vec4<int>& s)
{
	for (auto& c = m_Channels.begin(); c != m_Channels.end(); ++c)
	{
		if ((*c)->Delete())
		{
			if ((*c)->IsInput())
				m_Inputs.Erase(std::remove_if(m_Inputs.Components().begin(), m_Inputs.Components().end(),
					[&](const std::unique_ptr<Component>& const a)
					{
						return dynamic_cast<Component*>(*c) == a.get();
					}
			));
			else
				m_Outputs.Erase(std::remove_if(m_Outputs.Components().begin(), m_Outputs.Components().end(),
					[&](const std::unique_ptr<Component>& const a)
					{
						return dynamic_cast<Component*>(*c) == a.get();
					}
			));

			m_Channels.erase(c);
			break;
		}
	}

	if (m_Divider)
		m_Divider->Color(theme->Get(C::Divider));

	if (m_Divider2)
		m_Divider2->Color(theme->Get(C::Divider));

	ScrollPanel::Update(s);
}