#pragma once
#include "pch.hpp"
#include "ui/Slider.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

class VolumeSlider : public SliderBase<VolumeSliderGraphics, ScrollbarType::Vertical>
{
public:
	using Parent = SliderBase<VolumeSliderGraphics, ScrollbarType::Vertical>;

	VolumeSlider();

	int    MinBarSize() const    override { return 25; }
	double SliderValue()         override { return std::pow((1.0 - (Value() / RATIO)) * HIGHEST, 4); }
	void   SliderValue(double v) override { Value(static_cast<int>((1.0 - (std::powf(v, 0.25) / HIGHEST)) * RATIO)); }
	double Decibels() { return 20 * std::log10(SliderValue()); };
	std::string& ValueText() override { return m_ValueText; }

	void Update(const Vec4<int>& v) override;

private:
	const double RATIO = 1000000.0;
	const double HIGHEST = 1.412536;

	std::string m_ValueText = "";

	int m_Counter = 0;
};

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

class PanSlider : public SliderBase<PanSliderGraphics, ScrollbarType::Horizontal>
{
public:
	using Parent = SliderBase<PanSliderGraphics, ScrollbarType::Horizontal>;

	PanSlider();

	int    MinBarSize() const override { return 1; }
	double SliderValue() override { return Value() - RATIO/2; }
	void   SliderValue(double v) override { Value(v + RATIO / 2); }
	std::string& ValueText() override { return m_ValueText; }
	bool   Hovering() const override { return m_Hovering; }

	void Update(const Vec4<int>& v) override;

private:
	const double RATIO = 100.0;
	bool m_Hovering = false;

	std::string m_ValueText = "";

	int m_Counter = 0;
};