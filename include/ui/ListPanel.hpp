#pragma once
#include "pch.hpp"
#include "ui/ChannelPanel.hpp"

class ChannelListPanel : public ScrollPanel
{
public:
	ChannelListPanel(SarAsio& sarasio)
		: asio(sarasio)
	{
		// Styling
		Background(Theme::Get(Theme::VIEW_BACKGROUND));
		EnableScrollbars(true, false);


		m_Listener += [this](Event::MousePressed& e)
		{
			for (auto& cp : m_Channels)
			{
				ChannelPanel& t = cp.get();
				if (t.text.Hovering())
				{
					for (auto& cp : m_Channels)
					{
						if (t.input)
							cp.get().Select(t.inputChannel);
						else
							cp.get().Select(t.outputChannel);

						cp.get().Selected(false);
					}
					t.Selected(true);
				}
			}
			
		};
	}

	void LoadChannels()
	{
		auto& c = Component();
		c.Clear();
		for (auto& i : asio.Inputs())
			m_Channels.emplace_back(c.Emplace<ChannelPanel>(i));

		c.Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0);

		for (auto& i : asio.Outputs())
			m_Channels.emplace_back(c.Emplace<ChannelPanel>(i));
	}

private:
	SarAsio& asio;
	std::vector<std::reference_wrapper<ChannelPanel>> m_Channels;
};