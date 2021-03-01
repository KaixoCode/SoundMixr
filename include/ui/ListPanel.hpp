#pragma once
#include "pch.hpp"
#include "ui/ChannelPanel.hpp"

class SMXRScrollPanel : public ScrollPanel
{
public:

	Vec2<bool> ScrollbarNotNecessary()
	{
		return { m_ScrollbarX->NotNecessary(), m_ScrollbarY->NotNecessary() };
	}

	// Overwrite these methods from ScrollPanel for custom ScrollbarGraphics	
	::Panel& Panel() const { return *m_Panel; }
	template<typename T, typename ...Args>
	T& Panel(Args&&... args)
	{
		if (m_Panel != nullptr)
			return dynamic_cast<T&>(*m_Panel);

		auto& _t = Emplace<T>(std::forward<Args>(args)...);
		m_Panel = &_t;
		m_ScrollbarX = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Horizontal>>();
		m_ScrollbarY = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Vertical>>();
		return _t;
	}
};

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

class ListPanel : public SMXRScrollPanel
{
public:
	ListPanel(AsioDevice& sarasio);

	void SortChannels();

	template<typename T>
	T& EmplaceChannel();

	template<>
	InputChannelPanel& EmplaceChannel()
	{
		auto& panl = m_InputChannels.emplace_back(&m_Inputs.Emplace<InputChannelPanel>(this));
		//panl->SmartPanel(true);
		return *panl;
	}

	template<>
	OutputChannelPanel& EmplaceChannel()
	{
		auto& panl = m_OutputChannels.emplace_back(&m_Outputs.Emplace<OutputChannelPanel>(this));
		//panl->SmartPanel(true);
		return *panl;
	}

	void ResetGrouping();
	void Transparency(bool d) { for (auto& _c : m_InputChannels) _c->Transparency(d); };

	std::vector<InputChannelPanel*>&  InputChannels() { return m_InputChannels; };
	std::vector<OutputChannelPanel*>& OutputChannels() { return m_OutputChannels; };

	void Clear() { m_Inputs.Clear(); m_InputChannels.clear(); m_Outputs.Clear(); m_OutputChannels.clear(); };
	void Update(const Vec4<int>& s) override;
	void Render(CommandCollection& d) override
	{
		//d.Command<Graphics::FrameBuffer>(1234, true, Vec4<int>{Position(), Size() + Vec2<int>{ 10, 10 }});
		//d.Command<Graphics::FrameBufferEnd>();
		ScrollPanel::Render(d);
	}

	void ShowEffectsPanel()
	{
		m_Effect.Visible(true);
	}

private:
	AsioDevice& asio;
	::SMXRScrollPanel& m_Channels;
	::Panel& m_Effect;
	::Panel& m_Inputs;
	MenuAccessories::VerticalDivider* m_Divider = nullptr;
	::Panel& m_Outputs;
	std::vector<InputChannelPanel*> m_InputChannels;
	std::vector<OutputChannelPanel*> m_OutputChannels;
};