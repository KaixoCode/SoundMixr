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
class Plugin : public Panel
{
public:

	/**
	 * Constructor
	 * @param plugin PluginBase from a DLL
	 */
	Plugin(SoundMixr::PluginBase* plugin);

	/**
	 * Destructor
	 * Makes sure the plugin is being destroyed and doesn't cause memory leaks
	 */
	virtual ~Plugin() { m_Plugin->Destroy(); }

	/**
	 * Generate the rightclick menu for this plugin.
	 * used by the plugin chain, which adds a remove button at the end because
	 * an plugin cannot remove itself from the plugin chain.
	 * @param menu menu to add to
	 */
	virtual void GetMenu(MenuBase& menu);

	/**
	 * Get the name of the plugin.
	 * @return name
	 */
	auto Name() -> const std::string& { return m_Plugin->Name(); }

	/**
	 * Bypass this effect. Different from enable/disable, this is used when the
	 * entire effect chain is bypassed because it also disables the enable/disable button.
	 */
	virtual void Bypass(bool b);

	/**
	 * Set the amount of lines in this channel.
	 * @param c count
	 */
	void Lines(int c) { m_Plugin->Channels(c); }

	/**
	 * Update the plugin, calls the SoundMixr::PluginBase::Update() method, this is separate
	 * from the normal update because it happens even when the plugin is not being displayed.
	 * Doesn't call Update if bypassed or disabled.
	 */
	void UpdatePlugin();

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	virtual operator nlohmann::json();
	virtual void operator=(const nlohmann::json& json);

protected:
	bool m_PEnabled = true,
		m_Enabled = true,
		m_Bypass = false;

	Vec4<int> m_PrevDim;

	SoundMixr::PluginBase* m_Plugin;

	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;

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

	void SavePreset();
	void SavePresetAs();
	void LoadPreset();
	void DefaultPreset();

	/**
	 * Init the layout of the effect, uses the same mechanics as Layout::Divs to creaté
	 * a complex layout.
	 */
	void Update();

	/**
	 * Recursive init method for a single div.
	 * @param div div to init
	 * @param dim dimensions this div gets
	 */
	void UpdateDiv(SoundMixr::Div& div, const Vec4<int>& dim);

	/**
	 * Set the object in the given div.
	 * @param div the div
	 * @param dim dimensions this object gets
	 */
	void SetObject(SoundMixr::Div& div, const Vec4<int>& dim);
};