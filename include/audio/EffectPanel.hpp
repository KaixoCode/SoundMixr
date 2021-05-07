#pragma once
#include "pch.hpp"
#include "audio/EffectChain.hpp"

/**
 * This is the panel that contains a pointer to the EffectChain of the selected channel
 * and displays it/forwards events etc.
 */
class EffectPanel : public Panel
{
public:

	/**
	 * Constructor.
	 */
	EffectPanel();

	/**
	 * Set the name to display at the top of the effect panel.
	 * @param name name
	 */
	void Name(const std::string& name) { m_Name = name; }

	/**
	 * Get the name that is being displayed at the top of this effect panel.
	 * @return name
	 */
	auto Name() -> std::string& { return m_Name; }

	/**
	 * Set the effect chain for this effect panel.
	 * @param e effect chain
	 */
	void EffectChain(::EffectChain* e);

	/**
	 * Get the effect chain currently in this effect panel.
	 * @return effect chain
	 */
	auto EffectChain() -> ::EffectChain* { return m_EffectChain; }

	/**
	 * Override the focused to automatically focus the effect 
	 * chain as well if not nullptr.
	 * @param v focused
	 */
	virtual void Focused(bool v) override { Panel::Focused(v); if (m_EffectChain) m_EffectChain->Focused(v); };
	virtual bool Focused() const override { return m_Focused; }

	/**
	 * Override the hovering to automatically hvoer the effect
	 * chain as well if not nullptr.
	 * @param v hovering
	 */
	virtual void Hovering(bool v) override { Panel::Hovering(v); if (m_EffectChain) m_EffectChain->Hovering(v); };
	virtual bool Hovering() const override { return m_Hovering; }

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

private:
	::EffectChain* m_EffectChain = nullptr;
	::ButtonBase* m_BypassChain;
	std::string m_Name;

	bool m_ShowSidebar = true;
	friend class Controller;
};