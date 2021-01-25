#pragma once
#include "pch.hpp"
#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

class ListPanel : public ScrollPanel
{
public:
	ListPanel(SarAsio& sarasio);

	void LoadChannels();
	void Transparency(bool d) { for (auto& _c : m_Channels) _c.second->Transparency(d); };

	std::unordered_map<int, ChannelPanel*>& Channels() { return m_Channels; };

private:
	SarAsio& asio;
	std::unordered_map<int, ChannelPanel*> m_Channels;
};