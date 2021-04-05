#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "audio/EndpointChannel.hpp"
#include "ui/Graphics.hpp"

class AudioController : public Panel
{
public:
	AudioController()
		: m_ChannelScrollPanel(Emplace<SMXRScrollPanel>()),
		m_InputsPanel(m_ChannelScrollPanel.Panel<Panel>().Emplace<Panel>()), 
		m_Divider(m_ChannelScrollPanel.Panel().Emplace<MenuDivider>()),
		m_OutputsPanel(m_ChannelScrollPanel.Panel().Emplace<Panel>())
	{}




private:
	AsioDevice m_AsioDevice;

	SMXRScrollPanel& m_ChannelScrollPanel;
	Panel& m_InputsPanel;
	MenuDivider& m_Divider;
	Panel& m_OutputsPanel;
};