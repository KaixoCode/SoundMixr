#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"
#include "PluginLoader.hpp"
#include "FileDialog.hpp"

/**
 * Simple wrapper for the Effects::EffectBase object, this is a panel and will 
 * display the EffectBase object.
 */
class Effect : public Panel
{
public:

	/**
	 * Constructor
	 * @param effect EffectBase from a DLL
	 */
	Effect(SoundMixr::EffectBase* effect);

	/**
	 * Destructor
	 * Makes sure the effect is being destroyed and doesn't cause memory leaks
	 */
	~Effect() { m_Effect->Destroy(); }

	/**
	 * Generate the rightclick menu for this effect.
	 * used by the effect chain, which adds a remove button at the end because
	 * an effect cannot remove itself from the effect chain.
	 * @param menu menu to add to
	 */
	void GetMenu(MenuBase& menu);

	/**
	 * Get the name of the effect.
	 * @return name
	 */
	auto Name() -> const std::string& { return m_Effect->Name(); }

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
	void Bypass(bool b);

	/**
	 * Set the amount of lines in this channel.
	 * @param c count
	 */
	void Lines(int c) { m_Effect->Channels(c); }

	/**
	 * Returns true when hovering over the draggable area.
	 * @return true when over drag area
	 */
	bool HoveringDrag() { return m_HoveringDrag; }

	/**
	 * Update the effect, calls the Effects::EffectBase::Update() method, this is separate
	 * from the normal update because it happens even when the effect is not being displayed.
	 * Doesn't call Update if bypassed or disabled.
	 */
	void UpdateEffect();

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

private:	
	int m_RealHeight = 0,
		m_Delete = false;

	bool m_PSmall = false,
		m_Small = false,
		m_HoveringDrag = false,
		m_PEnabled = true,
		m_Enabled = true,
		m_Bypass = false;

	SoundMixr::EffectBase* m_Effect;

	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;

	// Preset and name of this effect.
	std::filesystem::path m_Preset{ "" };
	std::string m_Name;

	// Radio button keys are stored in the effect, keeps track of radio
	// button groups and their keys, convert to ListButton keys.
	std::unordered_map<int, int> m_RadioButtonKeys;
	std::unordered_map<int, std::vector<SoundMixr::RadioButton*>> m_RadioButtons;

	// Used for tab objects when initializing the effect div, and dividers
	Component* m_PrevObj = nullptr;
	std::vector<Vec4<int>> m_Dividers;

	/**
	 * Init the layout of the effect, uses the same mechanics as Layout::Divs to creaté
	 * a complex layout.
	 */
	void Effect::Init();

	/**
	 * Recursive init method for a single div.
	 * @param div div to init
	 * @param dim dimensions this div gets
	 */
	void Effect::InitDiv(SoundMixr::Div& div, const Vec4<int>& dim);

	/**
	 * Set the object in the given div.
	 * @param div the div
	 * @param dim dimensions this object gets
	 */
	void Effect::SetObject(SoundMixr::Div& div, const Vec4<int>& dim);
};