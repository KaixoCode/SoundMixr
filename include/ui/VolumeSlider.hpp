#pragma once
#include "pch.hpp"
#include "ui/Slider.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename T>
class VolumeSliderG : public SliderBase<T>
{
public:
	using Parent = SliderBase<T>;

	VolumeSliderG()
	{
		Range(Vec2<double>{0, 3.98107});
		Power(4);
		Value(1);
		ResetValue(1);
		Vertical(true);
	}

	double Decibels() { return 20 * std::log10(std::max(Value(), 0.000001)); };

	void Update(const Vec4<int>& v)
	{
		Component::Update(v);

		char s[10];
		if (Decibels() < -100)
			m_ValueText = "-inf";
		else {
			std::sprintf(s, "%.1f", Decibels());
			m_ValueText = s;
		}
		m_ValueText += "dB";
	}
};

using VolumeSlider = VolumeSliderG<VolumeSliderGraphics>;

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename Graphics>
class PanSliderG : public SliderBase<Graphics>
{
public:
	using Parent = SliderBase<Graphics>;

	PanSliderG(const std::string& name = "")
		: Parent(name)
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

	void Update(const Vec4<int>& v) override
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
		else {
			std::sprintf(s, "%.0f", Value());
			m_ValueText = s;
			m_ValueText += "R";
		}
	}
};

using PanSlider = PanSliderG<PanSliderGraphics>;
using PanKnob = PanSliderG<KnobSliderGraphics>;

using KnobSlider = SliderBase<KnobSliderGraphics>;
using NormalSlider = SliderBase<SliderGraphics>;



class DynamicsSlider : public Container
{
public:
	DynamicsSlider()
	{
		UpdateStrings();

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button != Event::MouseButton::LEFT || e.x < X() || e.x > X() + Width())
				return;

			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(threshhold2) - m_DragRange)
				m_Dragging = RT2, m_PressVal = ratio2;
			else if (m_PVal > DbToPixel(threshhold1) + m_DragRange)
				m_Dragging = RT1, m_PressVal = ratio1;
			else if (m_PVal < DbToPixel(threshhold2) + m_DragRange)
				m_Dragging = TH2, m_PressVal = DbToPixel(threshhold2);
			else if (m_PVal > DbToPixel(threshhold1) - m_DragRange)
				m_Dragging = TH1, m_PressVal = DbToPixel(threshhold1);
		};
		m_Listener += [this](Event::MouseClicked& e)
		{
			int p = m_Click;
			m_Click = 20;
			
			if (!p)
				return; 

			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(threshhold2) - m_DragRange)
				ratio2 = 0;
			else if (m_PVal > DbToPixel(threshhold1) + m_DragRange)
				ratio1 = 0;
			else if (m_PVal < DbToPixel(threshhold2) + m_DragRange)
				threshhold2 = -50, threshhold1 = std::max(threshhold1, -50.0);
			else if (m_PVal > DbToPixel(threshhold1) - m_DragRange)
				threshhold1 = -10, threshhold2 = std::min(threshhold2, -10.0);

			UpdateStrings();
		};
		m_Listener += [this](Event::MouseMoved& e)
		{
			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(threshhold2) - m_DragRange)
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
			else if (m_PVal > DbToPixel(threshhold1) + m_DragRange)
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
			else if (m_PVal < DbToPixel(threshhold2) + m_DragRange)
				m_Cursor = GLFW_HAND_CURSOR;
			else if (m_PVal > DbToPixel(threshhold1) - m_DragRange)
				m_Cursor = GLFW_HAND_CURSOR;
			else
				m_Cursor = GLFW_CURSOR_NORMAL;
		};
		m_Listener += [this](Event::MouseDragged& e)
		{
			int cval = e.x - X();
			
			if (m_Dragging == TH2)
			{
				double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Width()));
				m_Cursor = GLFW_HAND_CURSOR;
				threshhold2 = db1;

				if (threshhold2 > threshhold1)
					threshhold1 = threshhold2;
			
				UpdateStrings();
			}
			else if (m_Dragging == TH1)
			{
				double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Width()));
				m_Cursor = GLFW_HAND_CURSOR;
				threshhold1 = db1;

				if (threshhold2 > threshhold1)
					threshhold2 = threshhold1;

				UpdateStrings();
			}
			else if (m_Dragging == RT1)
			{
				double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
				m_PVal = cval;
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
				ratio1 = constrain(db1 + ratio1, -31, 32);
				UpdateStrings();
			}
			else if (m_Dragging == RT2)
			{
				double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
				m_PVal = cval;
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
				ratio2 = constrain(ratio2 - db1, -31, 32);
				UpdateStrings();
			}
		};

		m_Listener += [this](Event::KeyPressed& e)
		{
			if (e.key == VK_SHIFT)
				m_Shift = true;
		};

		m_Listener += [this](Event::KeyReleased& e)
		{
			if (e.key == VK_SHIFT)
				m_Shift = false;
		};

		m_Listener += [this](Event::MouseReleased& e)
		{
			m_Dragging = 0;
		};
	}

	void Update(const Vec4<int>& v)
	{
		if (m_Click)
			m_Click--;

		Container::Update(v);
	}

	void Render(CommandCollection& d)
	{
		Container::Render(d);
		using namespace Graphics;
		d.Command<Fill>(theme->Get(C::Dynamics));
		d.Command<Quad>(Vec4<int>{ X() - 7, Y() - 20, Width() + 14, Height() + 20 });

		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		int _y = Height() - 22;
		int _d = 3;
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::CENTER, Align::BOTTOM);
		for (int i = 0; i > -120; i -= _d)
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;

			if (_b)
				d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
			else
				d.Command<Graphics::Fill>(theme->Get(C::VMeterIndD));

			int _mdb = DbToPixel(i) - 1;
			d.Command<Quad>(Vec4<int>{ _mdb, _y, 1, 5});
			if (_b)
			{
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i, std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(theme->Get(C::TextSmall));
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_mdb, _y + 7});
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
		d.Command<Graphics::Quad>(Vec4<int>{0, _y, 1, 5});
		d.Command<Graphics::Fill>(theme->Get(C::TextSmall));
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{5, _y + 7});

		int _y2 = 5;
		d.Command<Graphics::Fill>(theme->Get(C::DynamicsL));
		d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, Width(), 1 });
		d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, 1, _y });
		d.Command<Graphics::Quad>(Vec4<int>{ 0, _y, Width(), 1 });
		d.Command<Graphics::Quad>(Vec4<int>{ Width() - 1, 0, 1, _y });
		int p1 = DbToPixel(threshhold1);
		d.Command<Graphics::Fill>(theme->Get(C::DynamicsB));
		d.Command<Graphics::Quad>(Vec4<int>{ p1, _y2, Width() - p1, _y - _y2 - 5 });
		double _xp = p1;
		for (int i = 0; i < 8; i++)
		{
			auto c = theme->Get(C::DynamicsL);
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
			_xp += ratio1 * 0.5 + 17;
			if (_xp > Width())
				break;
		}

		int p2 = DbToPixel(threshhold2);
		d.Command<Graphics::Fill>(theme->Get(C::DynamicsB));
		d.Command<Graphics::Quad>(Vec4<int>{ 0, _y2, p2, _y - _y2 - 5 });
		_xp = p2;
		for (int i = 0; i < 8; i++)
		{
			auto c = theme->Get(C::DynamicsL);
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
			_xp -= ratio2 * 0.5 + 17;
			if (_xp < 0)
				break;

		}
		d.Command<PopMatrix>();
	}

	double PixelToDb(int p)
	{
		double v = ((double)p) / (double)(Width());
		return 20 * std::log10(std::powf(v, 4));
	}

	int DbToPixel(double p)
	{
		return std::pow(std::pow(10, p / 20.0), 0.25) * (Width());
	}

	double threshhold1 = -10;
	double threshhold2 = -50;
	double ratio1 = 0;
	double ratio2 = 0;

	std::string m_TH1Str = "";
	std::string m_TH2Str = "";
	std::string m_RT1Str = "1:8";
	std::string m_RT2Str = "1:8";
private:
	int m_Click = 0;
	int m_PVal = 0;
	int m_Dragging = 0;
	const int TH1 = 1;
	const int TH2 = 2;
	const int RT1 = 3;
	const int RT2 = 4;
	int m_DragRange = 8;
	double m_PressVal = 0;
	bool m_Shift = false;

	std::string m_NegInf = "Inf";
	static inline std::unordered_map<int, std::string> m_Numbers;

	void UpdateStrings()
	{
		char s[10];
		std::sprintf(s, "%.1f", threshhold1);
		m_TH1Str = s;
		m_TH1Str += "dB";
		std::sprintf(s, "%.1f", threshhold2);
		m_TH2Str = s;
		m_TH2Str += "dB";
		double rt1 = ratio1;
		if (rt1 >= 0)
		{
			rt1 = 1.0 / ((rt1 / 32.0) + 1);
			std::sprintf(s, "%.2f", rt1);
			m_RT1Str = "1 : ";
			m_RT1Str += s;
		}
		else
		{
			rt1 = std::abs(rt1 - 1);
			std::sprintf(s, "%.2f", rt1);
			m_RT1Str = "1 : ";
			m_RT1Str += s;
		}
		double rt2 = ratio2;
		if (rt2 >= 0)
		{
			rt2 = 1.0 / ((rt2 / 8.0) + 1);
			std::sprintf(s, "%.2f", rt2);
			m_RT2Str = "1 : ";
			m_RT2Str += s;
		}
		else
		{
			rt2 = std::abs(rt2 - 1);
			std::sprintf(s, "%.2f", rt2);
			m_RT2Str = "1 : ";
			m_RT2Str += s;
		}
	}
};