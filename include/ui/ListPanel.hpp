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

	void Update(const Vec4<int>& s) override 
	{ 
		if (m_Divider)
			m_Divider->Color(Theme<C::Divider>::Get()); 
		
		ScrollPanel::Update(s); 
	}

private:
	AsioDevice& asio;
	std::unordered_map<int, ChannelPanel*> m_Channels;
	MenuAccessories::VerticalDivider* m_Divider;
};