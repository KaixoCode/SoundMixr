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

	void SortChannels()
	{
		std::sort(m_Inputs.Components().begin(), m_Inputs.Components().end(),
			[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
			{
				InputChannelPanel* _ina = dynamic_cast<InputChannelPanel*>(a.get());
				InputChannelPanel* _inb = dynamic_cast<InputChannelPanel*>(b.get());

				if (_ina != nullptr && _inb != nullptr)
					return _ina->Channels().ID() < _inb->Channels().ID();

				return -1;
			});

		std::sort(m_Outputs.Components().begin(), m_Outputs.Components().end(),
			[](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
			{
				OutputChannelPanel* _ina = dynamic_cast<OutputChannelPanel*>(a.get());
				OutputChannelPanel* _inb = dynamic_cast<OutputChannelPanel*>(b.get());

				if (_ina != nullptr && _inb != nullptr)
					return _ina->Channels().ID() < _inb->Channels().ID();

				return -1;
			});
	}

	template<typename T>
	T& EmplaceChannel();

	template<>
	InputChannelPanel& EmplaceChannel()
	{
		auto& panl = m_InputChannels.emplace_back(&m_Inputs.Emplace<InputChannelPanel>(this));
		panl->SmartPanel(true);
		return *panl;
	}

	template<>
	OutputChannelPanel& EmplaceChannel()
	{
		auto& panl = m_OutputChannels.emplace_back(&m_Outputs.Emplace<OutputChannelPanel>(this));
		panl->SmartPanel(true);
		return *panl;
	}

	void ResetGrouping();
	void Transparency(bool d) { for (auto& _c : m_InputChannels) _c->Transparency(d); };
	void Vertical();
	void Horizontal();

	std::vector<InputChannelPanel*>&  InputChannels() { return m_InputChannels; };
	std::vector<OutputChannelPanel*>& OutputChannels() { return m_OutputChannels; };

	void Clear() { m_Inputs.Clear(); m_InputChannels.clear(); m_Outputs.Clear(); m_OutputChannels.clear(); };

	void Update(const Vec4<int>& s) override 
	{ 
		for (auto& c = m_InputChannels.begin(); c != m_InputChannels.end(); ++c)
		{
			if ((*c)->Delete())
			{
				m_Inputs.Erase(std::remove_if(m_Inputs.Components().begin(), m_Inputs.Components().end(),
					[&](const std::unique_ptr<Component>& const a)
					{
						return dynamic_cast<Component*>(*c) == a.get();
					}
				));
				m_InputChannels.erase(c);
				break;
			}
		}

		for (auto& c = m_OutputChannels.begin(); c != m_OutputChannels.end(); ++c)
			if ((*c)->Delete())
			{
				m_Outputs.Erase(std::remove_if(m_Outputs.Components().begin(), m_Outputs.Components().end(),
					[&](const std::unique_ptr<Component>& const a)
					{
						return dynamic_cast<Component*>(*c) == a.get();
					}
				));
				m_OutputChannels.erase(c);
				break;
			}

		if (m_Divider)
			m_Divider->Color(Theme<C::Divider>::Get()); 
		
		ScrollPanel::Update(s); 
	}

private:
	AsioDevice& asio;
	::Panel& m_Inputs;
	MenuAccessories::VerticalDivider* m_Divider = nullptr;
	::Panel& m_Outputs;
	std::vector<InputChannelPanel*> m_InputChannels;
	std::vector<OutputChannelPanel*> m_OutputChannels;
};