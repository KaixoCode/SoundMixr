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

	d.Command<Fill>(Color{ 50, 50, 50, 255 });
	d.Command<Quad>(X() + _p * 2, _p, Width() - _p * 4, _y);

	if (Hovering() && Mouse() >= _y && Mouse() <= _y + _h)
	{
		d.Command<Fill>(Color{ 75, 75, 75, 255 });
		_p = 4;
	}
	else
		d.Command<Fill>(Color{ 55, 55, 55, 255 });

	int _w = Width() - _p * 2;
	int _he = _h - _p * 2;
	d.Command<Quad>(X() + _p, _y + _p, _w, _he);
	d.Command<Font>(Fonts::Gidole14, 14.0f);
	d.Command<Fill>(Color{ 255, 255, 255, 255 });
	d.Command<TextAlign>(Align::CENTER, Align::CENTER);
	d.Command<Text>(&m_ValueText, Vec2<int>{X() + _p + _w / 2, _y + _p + _he / 2});
}
