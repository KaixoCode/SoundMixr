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

	std::vector<std::reference_wrapper<ChannelPanel>>& Channels() { return m_Channels; };

private:
	SarAsio& asio;
	std::vector<std::reference_wrapper<ChannelPanel>> m_Channels;
};