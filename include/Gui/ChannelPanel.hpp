#pragma once
#include "pch.hpp"
#include "GuiCode2/Components/Panel.hpp"
#include "Audio/Channel.hpp"

struct ChannelPanel : public Panel
{
	ChannelPanel();
	ChannelPanel(ChannelPanel&& other) = delete;
	ChannelPanel(const ChannelPanel& other) = delete;
	ChannelPanel& operator=(ChannelPanel&& other) = delete;
	ChannelPanel& operator=(const ChannelPanel& other) = delete;

	Panel inputs;
	Panel outputs;
	Menu menu;

	void Init();
	void Update() override;

private:
	bool m_Search = true;
};