#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

ListPanel::ListPanel(SarAsio& sarasio)
	: asio(sarasio)
{
	Background(Theme::Get(Theme::VIEW_BACKGROUND));
	EnableScrollbars(true, false);

	m_Listener += [this](Event::MousePressed& e)
	{
		bool s = false;
		for (auto& _panel : m_Channels)
		{
			ChannelPanel& _current = *_panel.second;
			if (_current.WithinBounds(Vec2<int>{e.x + (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Value()), e.y - (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Height()) }))
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
		if (!s && !m_ScrollbarX->Hovering())
			for (auto& _p : m_Channels)
			{
				_p.second->Selected(false);
				_p.second->Unselect();
			}
	};

	m_Listener += [this](Event::Unfocused& e)
	{
		if (!m_ScrollbarX->Hovering())
		for (auto& _p : m_Channels)
		{
			_p.second->Selected(false);
			_p.second->Unselect();
		}
	};
}

void ListPanel::LoadChannels()
{
	auto& c = Component();
	c.Clear();
	m_Channels.clear();
	for (auto& i : asio.Inputs())
		m_Channels.emplace(i.ID(), &c.Emplace<ChannelPanel>(i));

	c.Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0);

	for (auto& i : asio.Outputs())
		m_Channels.emplace(-i.ID() - 2, &c.Emplace<ChannelPanel>(i));
}