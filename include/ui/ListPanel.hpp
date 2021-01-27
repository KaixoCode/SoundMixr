#pragma once
#include "pch.hpp"
#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

class ListPanel : public ScrollPanel
{
public:
	ListPanel(AsioDevice& sarasio);

	void LoadChannels();
	void Transparency(bool d) { for (auto& _c : m_Channels) _c.second->Transparency(d); };
	void Vertical();
	void Horizontal();

	std::unordered_map<int, ChannelPanel*>& Channels() { return m_Channels; };

private:
	AsioDevice& asio;
	std::unordered_map<int, ChannelPanel*> m_Channels;
	MenuAccessories::VerticalDivider* m_Divider;
};