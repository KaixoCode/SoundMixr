#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "audio/Dynamics.hpp"
#include "audio/Equalizer.hpp"
#include "audio/Formant.hpp"
#include "audio/Utility.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectPanel : public Panel
{
public:
	EffectPanel();

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectsGroup = e; }
	auto EffectsGroup() -> ::EffectsGroup* { return m_EffectsGroup; }

	template<typename T>
	void AddEffect()
	{
		if (m_EffectsGroup)
			m_EffectsGroup->Emplace<T>();
	}

private:
	::EffectsGroup* m_EffectsGroup = nullptr;
};

// -------------------------------------------------------------------------- \\
// ------------------------ Effect Scroll Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectScrollPanel : public SMXRScrollPanel
{
public:
	EffectScrollPanel();

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectPanel.EffectsGroup(e); }

private:
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div = nullptr, * m_Div2 = nullptr;
	EffectPanel& m_EffectPanel;
	double m_MouseY = 0;
};
