#pragma once
#include "pch.hpp"
#include "GuiCode2/Panel.hpp"
#include "Audio/Channel.hpp"

struct ChannelPanel : public Panel
{
	static inline int ID = 8919451;
	ChannelPanel();
	ChannelPanel(ChannelPanel&& other);

	Pointer<Panel> inputs;
	Pointer<Panel> outputs;
	Pointer<Menu> menu = new Menu;

	void Init();
	void Update() override;
};

struct ChannelPanelParser : public PanelParser
{
	ChannelPanelParser();
	Pointer<Component> Create();
	void Append(Component& c, Pointer<Component>&& obj) override;
};