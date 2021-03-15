#pragma once
#include "pch.hpp"
#include "ui/Parameter.hpp"
#include "ui/Graphics.hpp"

using Knob = Parameter<KnobGraphics>;
using Slider = Parameter<SliderGraphics>;

// -------------------------------------------------------------------------- \\
// ---------------------------- Radio Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

class RadioButton : public Button<RadioButtonGraphics, ButtonType::List>
{
public:
	RadioButton(Effects::RadioButton& t);

	void Update(const Vec4<int>& v);

private:
	static inline std::unordered_map<int, int> m_Keys;
	static inline std::unordered_map<int, std::vector<Effects::RadioButton*>> m_RButtons;

	Effects::RadioButton& m_Toggle;

	static inline int GetKey(Effects::RadioButton& k);
};

// -------------------------------------------------------------------------- \\
// --------------------------- Toggle Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

class ToggleButton : public Button<ToggleButtonGraphics, ButtonType::Toggle>
{
public:
	ToggleButton(Effects::ToggleButton& t);

	void Update(const Vec4<int>& v);

private:
	Effects::ToggleButton& m_Toggle;
};

// -------------------------------------------------------------------------- \\
// --------------------------- XY Controller -------------------------------- \\
// -------------------------------------------------------------------------- \\

class XYController : public Container
{
public:
	XYController(Effects::XYController& c);

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;

private:
	int m_Click = 0;
	bool m_Dragging = false;
	Effects::XYController& controller;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

class VolumeSlider : public Parameter<VolumeSliderGraphics>
{
public:
	VolumeSlider(Effects::VolumeSlider& s);

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	Effects::VolumeSlider& m_Slider;
};

// -------------------------------------------------------------------------- \\
// ------------------------- Old Volume Slider ------------------------------ \\
// -------------------------------------------------------------------------- \\

class OldVolumeSlider : public Parameter<VolumeSliderGraphics>
{
public:
	using Parent = Parameter<VolumeSliderGraphics>;

	OldVolumeSlider();

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

	void Update(const Vec4<int>& v);

private:
	Effects::Parameter m_Parameter;
};

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

class PanSlider : public Parameter<PanSliderGraphics>
{
public:
	PanSlider();

private:
	Effects::Parameter m_Parameter;
};

// -------------------------------------------------------------------------- \\
// -------------------------- Dynamics Slider ------------------------------- \\
// -------------------------------------------------------------------------- \\

class DynamicsSlider : public Container
{
public:
	DynamicsSlider(Effects::DynamicsSlider& o);

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	double PixelToDb(int p);
	int DbToPixel(double p);

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";
	static constexpr int 
		TH1 = 1,
		TH2 = 2,
		RT1 = 3,
		RT2 = 4;

	Effects::DynamicsSlider& m_Object;
	
	std::string 
		m_TH1Str = "",
		m_TH2Str = "",
		m_RT1Str = "1:8",
		m_RT2Str = "1:8";

	int m_Click = 0,
		m_PVal = 0,
		m_Dragging = 0,
		m_DragRange = 8;
	
	double m_PressVal = 0;
	bool m_Shift = false;

	void UpdateStrings();
};