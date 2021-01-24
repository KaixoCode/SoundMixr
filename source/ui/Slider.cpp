#include "ui/Slider.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Slider ------------------------------------ \\
// -------------------------------------------------------------------------- \\

Slider::Slider()
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

void Slider::Update(const Vec4<int>& v)
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

void Slider::Render(CommandCollection& d)
{
	using namespace Graphics;
	int _p = 6;
	int _h = max((VisibleRange() - Range().start) / (float)(Range().end - Range().start) * Height(), (float)MinBarSize());
	int _y = (Value() - Range().start) / (float)(Range().end - Range().start - VisibleRange()) * -(Height() - _h) + Y() + Height() - _h;

	d.Command<Fill>(Color{ 80, 80, 80, 255 });

	int _w = Width() - _p * 2;
	int _he = _h - _p * 2;
	d.Command<Triangle>(Vec4<int>{X() + _p, _y + _p, 8, _he}, 0);
	d.Command<Triangle>(Vec4<int>{X() - _p + Width(), _y + _p, 8, _he}, 180);
	d.Command<Quad>(Vec4<int>{X() + _p, _y + _p - 1, Width() - _p * 2, 3});
	
	if (Hovering())
	{
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Color{ 200, 200, 200, 255 });
		d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
		d.Command<Text>(&m_ValueText, Vec2<int>{X() + _p + _w / 2, _y + _p + 4});
	}
}
