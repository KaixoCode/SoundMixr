#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"
#include "EffectLoader.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Effect ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Effect : public Panel
{
public:
	static inline double sampleRate = 48000;

	Effect(Effects::EffectBase*);
	~Effect();

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	void UpdateEffect() { if (m_Enabled && !m_Bypass) m_Effect->Update(); }
	
	auto  Name() -> const std::string& { return m_Effect->Name(); }
	float NextSample(float s, int c) { return !m_Bypass && m_Enabled ? m_Effect->NextSample(s, c) : s; };
	void  Channels(int c) { m_Effect->Channels(c); m_Channels = c; }
	bool  HoveringDrag() { return m_HoveringDrag; }
	void  Bypass(bool b);
	bool  Delete() { return m_Delete; }
	void  Delete(bool b) { m_Delete = b; }

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

protected:
	int m_Channels = -1, 
		m_RealHeight = 0, 
		m_Delete = false;
	
	bool m_PSmall = false, 
		m_Small = false, 
		m_HoveringDrag = false,
		m_PEnabled = true,
		m_Enabled = true,
		m_Bypass = false;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_Minim;
	Button<SoundMixrGraphics::Menu, ButtonType::Normal>* m_SavePreset;
	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;
	Effects::EffectBase* m_Effect;
	std::vector<Vec4<int>> m_Dividers;

	std::filesystem::path m_Preset{""};
	std::string m_Name;

	std::unordered_map<int, int> m_RadioButtonKeys;
	std::unordered_map<int, std::vector<Effects::RadioButton*>> m_RadioButtons;

	// The code for the Layout interpretter for Effects.
	void Init();
	void InitDiv(Effects::Div& div, const Vec4<int>& dim);
	void SetObject(Effects::Div& div, const Vec4<int>& dim);
	Component* m_PrevObj = nullptr;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Group --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectsGroup : public Panel
{
public:
	EffectsGroup();
	~EffectsGroup();

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	void UpdateEffects() { for (auto& _c : m_Effects) _c->UpdateEffect(); };

	bool  Lock()	const { if (!m_Dead) m_Mutex.lock(); return !m_Dead; }
	void  Unlock()	const { m_Mutex.unlock(); }

	float NextSample(float a, int ch);
	void  Channels(int channels);
	bool  Hovering();
	bool  Dragging() { return m_Dragging != nullptr; }
	void  Enable() { m_Enabled = true; for (auto& _c : m_Effects) _c->Bypass(false); }
	void  Disable() { m_Enabled = false; for (auto& _c : m_Effects) _c->Bypass(true); }
	bool  Enabled() const { return m_Enabled; }
	auto  Add(Effects::EffectBase* e) -> Effect&;
	int   GetIndex(int y) const;
	auto  Name() -> std::string& const{ return m_Name; }
	void  Name(const std::string& n) { m_Name = n; }

	void Clear() { for (auto& _c : m_Effects) _c->Delete(true); }

	virtual void Focused(bool v) override;
	virtual bool Focused() const override { return m_Focused; }
	virtual void Hovering(bool v) override;
	virtual bool Hovering() const override { return m_Hovering; }

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

private:
	mutable std::mutex m_Mutex;

	int m_Channels = 0, 
		m_EffectCount = 0,
		m_InsertIndex = -1;

	bool m_Dead = false,
		m_Enabled = true;

	double m_MouseY = 0;
	
	Effect* m_HoveringComponent = nullptr,
		* m_FocusedComponent = nullptr,
		* m_Dragging = nullptr;

	std::string m_Name;
	std::vector<std::unique_ptr<Effect>> m_Effects;

	void Determine(Event& e);
};
