#include "ui/VolumeSlider.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\


// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

PanSlider::PanSlider()
{
	m_ValueText.reserve(10);
	Range(Vec2<int>{0, (int)RATIO + 1});
	VisibleRange(1);
	SliderValue(0);
	Vertical(false);
	ResetValue(0);

	m_Listener += [this](Event::MouseEntered& e)
	{ m_Hovering = true; };
	m_Listener += [this](Event::MouseExited& e)
	{ m_Hovering = false; };
}

void PanSlider::Update(const Vec4<int>& v)
{
	char s[10];
	if (SliderValue() == 0)
		m_ValueText = "C";
	else if (SliderValue() < 0) {
		std::sprintf(s, "%.0f", std::abs(SliderValue()));
		m_ValueText = s;
		m_ValueText += "L";
	}
	else  {
		std::sprintf(s, "%.0f", SliderValue());
		m_ValueText = s;
		m_ValueText += "R";
	}

	Parent::Update(v);
}

// -------------------------------------------------------------------------- \\
// ---------------------------- Knob Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

KnobSlider::KnobSlider()
{
	m_ValueText.reserve(10);
	Range(Vec2<int>{0, 1000000});
	VisibleRange(10);
	SliderValue(1);
	Vertical(true);
	ResetValue(1);
}

void KnobSlider::Update(const Vec4<int>& v)
{
	char s[10];
	std::sprintf(s, "%.1f", SliderValue() * (m_Range.y - m_Range.x) + m_Range.x);
	m_ValueText = s;
	m_ValueText += m_Unit;

	Parent::Update(v);
}