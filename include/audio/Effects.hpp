#pragma once
#include "pch.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Effect ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Effect : public Panel
{
public:
	static inline double sampleRate = 48000;

	Effect(const std::string& name);

	virtual void Init() = 0;

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;
	
	virtual float NextSample(float, int) = 0;
	virtual void Channels(int c) { m_Channels = c; }

	bool Hovering() { return m_Hovering; }
	bool HoveringDrag() { return m_HoveringDrag; }

	virtual operator json() = 0;
	virtual void operator=(const json& json) = 0;

	bool Delete() { return m_Delete; }

	void SetRightClickMenu(RightClickMenu* r) { m_RightClickMenu = r; }

protected:
	RightClickMenu* m_RightClickMenu = nullptr;
	int m_Channels = -1, m_RealHeight = 0, m_Delete = false;
	bool m_Hovering = false, m_Small = false, m_HoveringDrag = false, m_Enabled = true;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div, * m_Div2;
	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_Minim;
	Button<ToggleButton, ButtonType::Toggle>& m_Enable;
	const std::string m_Name = "";
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

	Effect& Add(Effect* e)
	{
		int p = m_EffectCount;
		m_EffectCount = 0;
		m_Effects.push_back(e);
		e->Channels(m_Channels);
		m_EffectCount = p + 1;
		return *e;
	}

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	int GetIndex(int y)
	{
		int index = 0;
		int _ri = 0;
		for (auto& i : m_Effects)
		{
			if (y < i->Y() + i->Height() / 2)
				_ri = index + 1;

			index++;
		}

		if (m_Dragging && y < m_Dragging->Y() + m_Dragging->Height() / 2)
			_ri -= 1;

		return _ri;
	}

	operator json();
	void operator=(const json& json);

private:
	std::vector<Effect*> m_Effects;
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
