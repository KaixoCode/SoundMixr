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
	Effect(int channels, const std::string& name);

	void Render(CommandCollection& d) override;

	virtual float NextSample(float, int) = 0;
	virtual void Channels(int c) { m_Channels = c; LOG("WOOF"); }

	bool Hovering() { return m_Hovering; }

	virtual operator json() = 0;
	virtual void operator=(const json& json) = 0;

protected:
	static inline double sampleRate = 480000;
	int m_Channels = -1;
	bool m_Hovering = false;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div;
	const std::string m_Name = "";
};

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Group --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectsGroup : public Panel
{
public:
	EffectsGroup();

	float NextSample(float a, int ch);
	void  Channels(int channels);
	bool  Hovering();

	template<typename T>
	T& Emplace()
	{
		int p = m_EffectCount;
		m_EffectCount = 0;
		auto& _t = m_Effects.emplace_back(std::make_unique<T>(m_Channels));
		_t->Channels(m_Channels);
		m_EffectCount = p + 1;
		return *dynamic_cast<T*>(_t.get());;
	}

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	operator json();
	void operator=(const json& json);

private:
	std::vector<std::unique_ptr<Effect>> m_Effects;
	int m_Channels = 0, m_EffectCount = 0;

	void Determine(Event& e);
};
