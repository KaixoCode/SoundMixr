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
		for (auto& _panel : m_Channels)
		{
			ChannelPanel& _current = _panel.get();
			if (_current.text.Hovering())
			{
				for (auto& _p : m_Channels)
				{
					ChannelPanel& _c = _p.get();
					if (_current.Input())
						_c.Select(_current.InputChannel());
					else
						_c.Select(_current.OutputChannel());

					_c.Selected(false);
				}
				_current.Selected(true);
			}
		}

	};
}

void ListPanel::LoadChannels()
{
	auto& c = Component();
	c.Clear();
	m_Channels.clear();
	for (auto& i : asio.Inputs())
		m_Channels.emplace_back(c.Emplace<ChannelPanel>(i));

	c.Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0);

	for (auto& i : asio.Outputs())
		m_Channels.emplace_back(c.Emplace<ChannelPanel>(i));
}