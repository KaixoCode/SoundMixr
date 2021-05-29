#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"
#include "PluginLoader.hpp"
#include "FileDialog.hpp"
#include "audio/Plugin.hpp"

/**
 * Simple wrapper for the Effects::EffectBase object, this is a panel and will 
 * display the EffectBase object.
 */
class Effect : public Plugin
{
public:

	/**
	 * Constructor
	 * @param effect EffectBase from a DLL
	 */
	Effect(SoundMixr::EffectBase* effect);

	/**
	 * Generate the rightclick menu for this effect.
	 * used by the effect chain, which adds a remove button at the end because
	 * an effect cannot remove itself from the effect chain.
	 * @param menu menu to add to
	 */
	void GetMenu(MenuBase& menu) override;

	/**
	 * Generate the next sample given the input sample and channel.
	 * @param s input sample
	 * @param c channel
	 */
	double Process(double s, int c);

	/**
	 * Bypass this effect. Different from enable/disable, this is used when the
	 * entire effect chain is bypassed because it also disables the enable/disable button.
	 */
	void Bypass(bool b) override;

	/**
	 * Returns true when hovering over the draggable area.
	 * @return true when over drag area
	 */
	bool HoveringDrag() { return m_HoveringDrag; }

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	virtual operator nlohmann::json() override;
	virtual void operator=(const nlohmann::json& json) override;

private:	
	int m_RealHeight = 0;

	bool m_PSmall = false,
		m_Small = false,
		m_HoveringDrag = false;

	SoundMixr::EffectBase* m_Effect;

	Button<NOTHING, ButtonType::Toggle>* m_MinimB;

};