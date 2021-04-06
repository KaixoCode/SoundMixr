//#pragma once
//#include "pch.hpp"
//#include "ui/ChannelPanel.hpp"
//#include "ui/EffectPanel.hpp"
//
//// -------------------------------------------------------------------------- \\
//// ---------------------------- List Panel ---------------------------------- \\
//// -------------------------------------------------------------------------- \\
//
//class ListPanel : public SMXRScrollPanel
//{
//public:
//	ListPanel(AsioDevice& sarasio);
//
//	void ReloadEffects() { m_Effect.SetupMenu(); }
//	void SortChannels();
//
//	ChannelPanel& EmplaceChannel(bool IsInput);
//	ChannelPanel& EmplaceSpecialChannel(bool IsInput = true);
//
//	void ResetGrouping();
//	void Transparency(bool d) { for (auto& _c : m_Channels) _c->Transparency(d); };
//
//	std::vector<ChannelPanel*>& Channels() { return m_Channels; }
//
//	void Clear();
//
//	void Update(const Vec4<int>& s) override;
//
//	void UpdateEffects() { m_Effect.UpdateEffects(); for (auto& i : Channels()) i->UpdateMidiParameters(); };
//
//	void ShowEffectsPanel(EffectsGroup& effects);
//
//private:
//	AsioDevice& asio;
//	::SMXRScrollPanel& m_ChannelsPanel;
//	::EffectFramePanel& m_Effect;
//	::Panel& m_Inputs;
//	VerticalMenuDivider* m_Divider = nullptr;
//	::Panel& m_Outputs;
//	VerticalMenuDivider* m_Divider2 = nullptr;
//	::Panel& m_Specials;
//	std::vector<ChannelPanel*> m_Channels;
//};