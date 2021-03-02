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
		m_ValueText.reserve(10);
		Range(Vec2<int>{0, 1000000});
		VisibleRange(10);
		SliderValue(1);
		Vertical(true);
		ResetValue(1);
	}

	int    MinBarSize() const    override { return 25; }
	double SliderValue()         override { return std::pow((1.0 - (Value() / RATIO)) * HIGHEST, 4); }
	void   SliderValue(double v) override { Value(static_cast<int>((1.0 - (std::powf(v, 0.25) / HIGHEST)) * RATIO)); }
	double Decibels() { return 20 * std::log10(SliderValue() + 0.00000000000001); };
	std::string& ValueText() override { return m_ValueText; }

	void Update(const Vec4<int>& v)
	{
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

private:
	const double RATIO = 1000000.0;
	const double HIGHEST = 1.412536;

	std::string m_ValueText = "";

};

using VolumeSlider = VolumeSliderG<VolumeSliderGraphics>;

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

class PanSlider : public SliderBase<PanSliderGraphics>
{
public:
	using Parent = SliderBase<PanSliderGraphics>;

	PanSlider();

	int    MinBarSize() const override { return 1; }
	double SliderValue() override { return (Value() - RATIO/2) / 10.0; }
	void   SliderValue(double v) override { Value(10 * (v) + RATIO / 2); }
	std::string& ValueText() override { return m_ValueText; }
	bool   Hovering() const override { return m_Hovering; }

	void Update(const Vec4<int>& v) override;

private:
	const double RATIO = 1000.0;
	bool m_Hovering = false;

	std::string m_ValueText = "";
};

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

class KnobSlider : public SliderBase<KnobSliderGraphics>
{
public:
	using Parent = SliderBase<KnobSliderGraphics>;

	KnobSlider();

	int    MinBarSize() const    override { return 25; }
	double SliderValue()         override { return Value() / RATIO; }
	void   SliderValue(double v) override { Value(v * RATIO); }
	void   SliderRange(const Vec2<int>& r) { m_Range = r; }
	void   Unit(const std::string& u) { m_Unit = u; }
	std::string& ValueText() override { return m_ValueText; }

	void Update(const Vec4<int>& v) override;

private:
	const double RATIO = 1000000.0;

	Vec2<int> m_Range;
	std::string m_ValueText = "", m_Unit = "";

};


class DynamicsSlider : public Container
{
public:
	DynamicsSlider()
	{
		UpdateStrings();

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button != Event::MouseButton::LEFT || e.y < Y() || e.y > Y() + Height())
				return;

			m_PVal = constrain(e.y - Y(), 0, Height());
			if (m_PVal < DbToPixel(threshhold2))
				m_Dragging = RT2, m_PressVal = ratio2;
			else if (m_PVal > DbToPixel(threshhold1))
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

			m_PVal = constrain(e.y - Y(), 0, Height());
			if (m_PVal < DbToPixel(threshhold2))
				ratio2 = 0;
			else if (m_PVal > DbToPixel(threshhold1))
				ratio1 = 0;
			else if (m_PVal < DbToPixel(threshhold2) + m_DragRange)
				threshhold2 = -50;
			else if (m_PVal > DbToPixel(threshhold1) - m_DragRange)
				threshhold1 = -10;

			UpdateStrings();
		};
		m_Listener += [this](Event::MouseMoved& e)
		{
			m_PVal = constrain(e.y - Y(), 0, Height());
			if (m_PVal < DbToPixel(threshhold2))
				m_Cursor = GLFW_RESIZE_NS_CURSOR;
			else if (m_PVal > DbToPixel(threshhold1))
				m_Cursor = GLFW_RESIZE_NS_CURSOR;
			else if (m_PVal < DbToPixel(threshhold2) + m_DragRange)
				m_Cursor = GLFW_HAND_CURSOR;
			else if (m_PVal > DbToPixel(threshhold1) - m_DragRange)
				m_Cursor = GLFW_HAND_CURSOR;
			else
				m_Cursor = GLFW_CURSOR_NORMAL;
		};
		m_Listener += [this](Event::MouseDragged& e)
		{
			int cval = e.y - Y();

			if (m_Dragging == TH2)
			{
				double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Height()));
				m_Cursor = GLFW_HAND_CURSOR;
				threshhold2 = db1;

				if (threshhold2 > threshhold1)
					threshhold1 = threshhold2;
			
				UpdateStrings();
			}
			else if (m_Dragging == TH1)
			{
				double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Height()));
				m_Cursor = GLFW_HAND_CURSOR;
				threshhold1 = db1;

				if (threshhold2 > threshhold1)
					threshhold2 = threshhold1;

				UpdateStrings();
			}
			else if (m_Dragging == RT1)
			{
				double db1 = (cval - m_PVal) * 0.2;
				m_Cursor = GLFW_RESIZE_NS_CURSOR;
				ratio1 = constrain(db1 + m_PressVal, -31, 31);
				UpdateStrings();
			}
			else if (m_Dragging == RT2)
			{
				double db1 = (cval - m_PVal) * 0.2;
				m_Cursor = GLFW_RESIZE_NS_CURSOR;
				ratio2 = constrain(m_PressVal - db1, -31, 31);
				UpdateStrings();
			}
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
		d.Command<Fill>(Theme<C::Dynamics>::Get());
		d.Command<Quad>(Vec4<int>{ X(), Y() - 7, Width(), Height() + 14 });

		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		int _x = Width() - 25;
		int _d = 3;
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
		for (int i = 0; i > -120; i -= _d)
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;

			if (_b)
				d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
			else
				d.Command<Graphics::Fill>(Theme<C::VMeterIndD>::Get());

			int _mdb = DbToPixel(i);
			d.Command<Quad>(Vec4<int>{ _x, _mdb, 5, 1});
			if (_b)
			{
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i, std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(Theme<C::TextSmall>::Get());
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + 7, _mdb});
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{_x, 0, 5, 1});
		d.Command<Graphics::Fill>(Theme<C::TextSmall>::Get());
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + 7, 0});

		int _x2 = 5;
		d.Command<Graphics::Fill>(Theme<C::DynamicsB>::Get());
		int p1 = DbToPixel(threshhold1);
		d.Command<Graphics::Quad>(Vec4<int>{ _x2, p1, _x - _x2 - 5, Height() - p1 });
		double _yp = p1;
		for (int i = 0; i < 8; i++)
		{
			auto c = Theme<C::DynamicsL>::Get();
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ _x2, (int)_yp, _x - _x2 - 5, 1 });
			_yp += ratio1*0.5 + 17;
			if (_yp > Height())
				break;
		}

		int p2 = DbToPixel(threshhold2);
		d.Command<Graphics::Fill>(Theme<C::DynamicsB>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{ _x2, 0, _x - _x2 - 5, p2 });
		_yp = p2;
		for (int i = 0; i < 8; i++)
		{
			auto c = Theme<C::DynamicsL>::Get();
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ _x2, (int)_yp, _x - _x2 - 5, 1 });
			_yp -= ratio2 * 0.5 + 17;
			if (_yp < 0)
				break;

		}
		d.Command<PopMatrix>();
	}

	double PixelToDb(int p)
	{
		double v = ((double)p) / (double)(Height());
		return 20 * std::log10(std::powf(v, 4));
	}

	int DbToPixel(double p)
	{
		return std::pow(std::pow(10, p / 20.0), 0.25) * (Height());
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
	int m_DragRange = 10;
	double m_PressVal = 0;

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
			rt1 = rt1 + 1;
			std::sprintf(s, "%.1f", rt1);
			m_RT1Str = "1:";
			m_RT1Str += s;
		}
		else
		{
			rt1 = std::abs(rt1 - 1);
			std::sprintf(s, "%.1f", rt1);
			m_RT1Str = s;
			m_RT1Str += ":1";
		}
		double rt2 = ratio2;
		if (rt2 >= 0)
		{
			rt2 = rt2 + 1;
			std::sprintf(s, "%.1f", rt2);
			m_RT2Str = "1:";
			m_RT2Str += s;
		}
		else
		{
			rt2 = std::abs(rt2 - 1);
			std::sprintf(s, "%.1f", rt2);
			m_RT2Str = s;
			m_RT2Str += ":1";
		}
	}
};