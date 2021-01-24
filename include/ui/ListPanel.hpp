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
	}

	void LoadChannels()
	{
		auto& c = Component();
		c.Clear();
		for (auto& i : asio.Inputs())
			c.Emplace<ChannelPanel>(i);

		c.Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0);

		for (auto& i : asio.Outputs())
			c.Emplace<ChannelPanel>(i);
	}

private:
	SarAsio& asio;
};