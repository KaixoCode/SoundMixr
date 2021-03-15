#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Effect ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Effect : public Panel
{
public:
	static inline double sampleRate = 48000;

	Effect(Effects::EffectBase*);

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;
	
	auto Name() -> const std::string& { return m_Effect->Name(); }
	float NextSample(float s, int c) { return m_Enabled ? m_Effect->NextSample(s, c) : s; };
	void Channels(int c) { m_Effect->Channels(c); m_Channels = c; }

	bool Hovering() { return m_Hovering; }
	bool HoveringDrag() { return m_HoveringDrag; }

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

	bool Delete() { return m_Delete; }

protected:
	int m_Channels = -1, m_RealHeight = 0, m_Delete = false;
	bool m_Hovering = false, m_Small = false, m_HoveringDrag = false, m_Enabled = true;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div, * m_Div2;
	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_Minim;
	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;
	Effects::EffectBase* m_Effect;
	std::vector<Vec4<int>> m_Dividers;

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

	int GetIndex(int y);

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

private:
	std::vector<std::unique_ptr<Effect>> m_Effects;
	int m_Channels = 0, m_EffectCount = 0;

	Effect* m_Hovering;
	Effect* m_Focussed;

	bool m_Dead = false;

	mutable std::mutex m_Mutex;

	Effect* m_Dragging = nullptr;
	int m_InsertIndex = -1;

	double m_MouseY = 0;

	void Determine(Event& e);
};
