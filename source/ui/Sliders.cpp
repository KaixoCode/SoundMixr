#include "ui/VolumeSlider.hpp"

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

PanSlider::PanSlider()
{
	m_ValueText.reserve(10);
	Range(Vec2<double>{ -50, 50 });
	Value(0);
	Vertical(false);
	ResetValue(0);

	m_Listener += [this](Event::MouseEntered& e)
	{ m_Hovering = true; };
	m_Listener += [this](Event::MouseExited& e)
	{ m_Hovering = false; };
}

void PanSlider::Update(const Vec4<int>& v)
{
	Component::Update(v);

	char s[10];
	if (Value() == 0)
		m_ValueText = "C";
	else if (Value() < 0) {
		std::sprintf(s, "%.0f", std::abs(Value()));
		m_ValueText = s;
		m_ValueText += "L";
	}
	else  {
		std::sprintf(s, "%.0f", Value());
		m_ValueText = s;
		m_ValueText += "R";
	}

}
