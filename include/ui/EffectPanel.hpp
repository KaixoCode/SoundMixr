#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectPanel : public Panel<>
{
public:
	EffectPanel();

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectsGroup = e; }
	auto EffectsGroup() -> ::EffectsGroup* { return m_EffectsGroup; }

	virtual void Focused(bool v) override { Panel::Focused(v); if (m_EffectsGroup) m_EffectsGroup->Focused(v); };
	virtual bool Focused() const override { return m_Focused; }
	virtual void Hovering(bool v) override { Panel::Hovering(v); if (m_EffectsGroup) m_EffectsGroup->Hovering(v); };
	virtual bool Hovering() const override { return m_Hovering; }

private:
	::EffectsGroup* m_EffectsGroup = nullptr;
};

// -------------------------------------------------------------------------- \\
// ------------------------ Effect Scroll Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectScrollPanel : public SMXRScrollPanel<EffectPanel>
{
public:
	EffectScrollPanel();

	void Update(const Vec4<int>& v) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectPanel.EffectsGroup(e); }

private:
	EffectPanel& m_EffectPanel;
	double m_MouseY = 0;
};

// -------------------------------------------------------------------------- \\
// ------------------------- Effect Frame Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectFramePanel : public Panel<>
{
public:
	EffectFramePanel();

	void SetupMenu();

	void Update(const Vec4<int>& v);
	void Render(CommandCollection& d) override;

	void EffectsGroup(EffectsGroup* e);

	void UpdateEffects();

private:
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	::ButtonBase* m_Toggle;
	::EffectsGroup* m_EffectGroup;
	std::string m_Name = "";
	EffectScrollPanel& m_Scroll;
};