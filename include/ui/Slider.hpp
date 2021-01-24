#pragma once
#include "pch.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Slider ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Slider : public Scrollbar<ScrollbarGraphics::Normal, ScrollbarType::Vertical>
{
public:
	using Parent = Scrollbar<ScrollbarGraphics::Normal, ScrollbarType::Vertical>;

	Slider();

	int    MinBarSize() const override { return Width(); }
	double SliderValue() { return std::pow((1.0 - (Value() / RATIO)) * HIGHEST, 4); }
	void   SliderValue(double v) { Value(static_cast<int>((1.0 - (std::powf(v, 0.25) / HIGHEST)) * RATIO)); }
	double Decibels() { return 20 * std::log10(SliderValue()); }

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

private:
	const double RATIO = 1000000.0;
	const double HIGHEST = 1.412536;

	std::string m_ValueText = "";

	int m_Counter = 0;
};
