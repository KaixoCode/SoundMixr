#pragma once
#include "pch.hpp"
#include "GuiCode2/Components/Panel.hpp"
#include "Audio/Channel.hpp"

struct ChannelPanel : public Panel
{
	static inline int CHANNEL_PANEL = 0xA9FC3911;
	static inline int INPUT_PANEL = 0xA9FC3912;
	static inline int OUTPUT_PANEL = 0xA9FC3913;
	ChannelPanel();
	ChannelPanel(ChannelPanel&& other) = delete;
	ChannelPanel(const ChannelPanel& other) = delete;
	ChannelPanel& operator=(ChannelPanel&& other) = delete;
	ChannelPanel& operator=(const ChannelPanel& other) = delete;

	Pointer<Panel> inputs;
	Pointer<Panel> outputs;
	Pointer<Menu> menu = new Menu;

	void Init();
	void Update() override;

private:
	bool m_Search = true;
};

struct ChannelPanelParser : public PanelParser
{
	ChannelPanelParser();
	Pointer<Component> Create();
};