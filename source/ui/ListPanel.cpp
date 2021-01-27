#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

ListPanel::ListPanel(AsioDevice& sarasio)
	: asio(sarasio)
{
	Background(Theme::Get(Theme::VIEW_BACKGROUND));
	EnableScrollbars(true, false);

	m_Listener += [this](Event::MousePressed& e)
	{
		Vec2<int> translated = Vec2<int>{ 
			e.x + (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Value()),
			e.y - (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Height()) - (m_ScrollbarY->NotNecessary() ? 0 : Component().Y()) };

		if (e.button == Event::MouseButton::LEFT)
		{
			bool s = false;
			for (auto& _panel : m_Channels)
			{
				ChannelPanel& _current = *_panel.second;
				if (_current.WithinBounds(translated))
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_Channels)
						{
							ChannelPanel& _c = *_p.second;
							if (_current.Input())
								_c.Select(_current.InputChannel());
							else
								_c.Select(_current.OutputChannel());

							_c.Selected(false);
						}
						_current.Selected(true);
					}
					s = true;
				}
			}
			if (!s && !m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
				for (auto& _p : m_Channels)
				{
					_p.second->Selected(false);
					_p.second->Unselect();
				}
		}
	};

	m_Listener += [this](Event::Unfocused& e)
	{
		if (!m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
		for (auto& _p : m_Channels)
		{
			_p.second->Selected(false);
			_p.second->Unselect();
		}
	};
}

void ListPanel::Vertical()
{
	EnableScrollbars(false, true);
	for (auto& _p : m_Channels)
		_p.second->Vertical();

	m_Divider->Height(1);
}

void ListPanel::Horizontal()
{
	EnableScrollbars(true, false);
	for (auto& _p : m_Channels)
		_p.second->Horizontal();

	m_Divider->Width(10);
}

void ListPanel::LoadChannels()
{
	auto& c = Component();
	c.Clear();
	m_Channels.clear();
	for (auto& i : asio.Inputs())
		m_Channels.emplace(i.ID(), &c.Emplace<ChannelPanel>(asio, i));

	m_Divider = &c.Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0);

	for (auto& i : asio.Outputs())
		m_Channels.emplace(-i.ID() - 2, &c.Emplace<ChannelPanel>(asio, i));
}