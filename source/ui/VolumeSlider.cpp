#include "ui/VolumeSlider.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

VolumeSlider::VolumeSlider()
{
	m_ValueText.reserve(10);
	Range(Vec2<int>{0, 1000000});
	VisibleRange(10);
	SliderValue(1);

	m_Listener += [this](Event::MouseClicked& e)
	{
		if (m_Counter > 0) // Double Click
			SliderValue(1);

		if (e.button == Event::MouseButton::LEFT)
			m_Counter = 20;
	};

	m_Listener += [this](Event::MousePressed& e)
	{
		m_PressMouse = e.y;
		m_PressValue = m_Value;

		m_Dragging = true;
	};
}

void VolumeSlider::Update(const Vec4<int>& v)
{
	if (m_Counter > 0)
		m_Counter--;

	char s[10];
	if (Decibels() < -100)
		m_ValueText = "-inf";
	else {
		std::sprintf(s, "%.1f", Decibels());
		m_ValueText = s;
	}
	m_ValueText += "dB";

	Parent::Update(v);
}

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

PanSlider::PanSlider()
{
	m_ValueText.reserve(10);
	Range(Vec2<int>{0, (int)RATIO + 1});
	VisibleRange(1);
	SliderValue(0);

	m_Listener += [this](Event::MouseClicked& e)
	{
		if (m_Counter > 0) // Double Click
			SliderValue(0);

		if (e.button == Event::MouseButton::LEFT)
			m_Counter = 20;
	};

	m_Listener += [this](Event::MousePressed& e)
	{
		m_PressMouse = e.y;
		m_PressValue = m_Value;

		m_Dragging = true;
	};

	m_Listener += [this](Event::MouseEntered& e)
	{ m_Hovering = true; };
	m_Listener += [this](Event::MouseExited& e)
	{ m_Hovering = false; };
}

void PanSlider::Update(const Vec4<int>& v)
{
	if (m_Counter > 0)
		m_Counter--;

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