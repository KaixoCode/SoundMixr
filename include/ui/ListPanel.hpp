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
	void Transparency(bool d) { for (auto& _c : m_Channels) _c.get().Transparency(d); };

	std::vector<std::reference_wrapper<ChannelPanel>>& Channels() { return m_Channels; };

private:
	SarAsio& asio;
	std::vector<std::reference_wrapper<ChannelPanel>> m_Channels;
};