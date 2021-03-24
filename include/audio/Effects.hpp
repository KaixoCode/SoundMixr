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

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;

	void UpdateEffect() { if (m_Enabled && !m_Bypass) m_Effect->Update(); }
	
	auto Name() -> const std::string& { return m_Effect->Name(); }
	float NextSample(float s, int c) { return !m_Bypass && m_Enabled ? m_Effect->NextSample(s, c) : s; };
	void Channels(int c) { m_Effect->Channels(c); m_Channels = c; }

	bool Hovering() { return m_Hovering; }
	bool HoveringDrag() { return m_HoveringDrag; }

	void Bypass(bool b) 
	{
		m_Bypass = b;
		if (!m_Bypass)
		{
			m_Enable->Enable();
			if (!m_Enabled)
				return;

			for (auto& i : Components())
				if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Enable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Enable();
			m_Effect->Update();
		}
		else
		{
			for (auto& i : Components())
				if (i.get() == m_MinimB)
					continue;
				else if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Disable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Disable();
		}
	}

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

	bool Delete() { return m_Delete; }

protected:
	int m_Channels = -1, 
		m_RealHeight = 0, 
		m_Delete = false;
	
	bool m_Hovering = false, 
		m_PSmall = false, 
		m_Small = false, 
		m_HoveringDrag = false,
		m_PEnabled = true,
		m_Enabled = true,
		m_Bypass = false;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_Minim;
	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;
	Effects::EffectBase* m_Effect;
	std::vector<Vec4<int>> m_Dividers;

	std::unordered_map<int, int> m_RadioButtonKeys;
	std::unordered_map<int, std::vector<Effects::RadioButton*>> m_RadioButtons;

	// The code for the Layout interpretter for Effects.
	void Init();
	void InitDiv(Effects::Div& div, const Vec4<int>& dim);
	void SetObject(Effects::Div& div, const Vec4<int>& dim);
};

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Group --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectsGroup : public Panel
{
public:
	EffectsGroup();
	~EffectsGroup();

	bool  Lock()	const { if (!m_Dead) m_Mutex.lock(); return !m_Dead; }
	void  Unlock()	const { m_Mutex.unlock(); }
	float NextSample(float a, int ch);
	void  Channels(int channels);
	bool  Hovering();
	bool  Dragging() { return m_Dragging != nullptr; }

	Effect& Add(Effects::EffectBase* e);

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	void UpdateEffects() { for (auto& _c : m_Effects) _c->UpdateEffect(); };

	int GetIndex(int y);

	void Enable() { m_Enabled = true; for (auto& _c : m_Effects) _c->Bypass(false); }
	void Disable() { m_Enabled = false; for (auto& _c : m_Effects) _c->Bypass(true); }
	bool Enabled() { return m_Enabled; }

	std::string& Name() { return m_Name; }
	void Name(const std::string& n) { m_Name = n; }

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

private:
	std::string m_Name;
	std::vector<std::unique_ptr<Effect>> m_Effects;
	int m_Channels = 0, m_EffectCount = 0;

	Effect* m_Hovering;
	Effect* m_Focussed;

	bool m_Dead = false,
		m_Enabled = true;

	mutable std::mutex m_Mutex;

	Effect* m_Dragging = nullptr;
	int m_InsertIndex = -1;

	double m_MouseY = 0;

	void Determine(Event& e);
};
