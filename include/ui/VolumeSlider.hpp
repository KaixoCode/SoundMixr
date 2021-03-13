#pragma once
#include "pch.hpp"
#include "ui/Slider.hpp"
#include "ui/Graphics.hpp"

class ToggleButtonComponent : public Button<ToggleButtonG, ButtonType::Toggle>
{
public:
	ToggleButtonComponent(ToggleButton& t)
		: m_Toggle(t), Button<ToggleButtonG, ButtonType::Toggle>(&t.state, t.Name())
	{}

	void Update(const Vec4<int>& v)
	{
		m_Size = { m_Toggle.Size().width, m_Toggle.Size().height };
		m_Pos = { m_Toggle.Position().x, m_Toggle.Position().y };

		Button<ToggleButtonG, ButtonType::Toggle>::Update(v);
	}

	ToggleButton& m_Toggle;
};

class VolumeSliderComponent : public SliderBase<VolumeSliderGraphics>
{
public:
	VolumeSliderComponent(VolumeSlider& s)
		: SliderBase<VolumeSliderGraphics>(s), m_Slider(s)
	{
		m_Parameter.Range({ 0.0, 3.98107 });
		m_Parameter.Power(4);
		m_Parameter.Value(1);
		m_Parameter.ResetValue(1);
		m_Parameter.Vertical(true);
	}

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

	void Update(const Vec4<int>& v)
	{
		m_Size = { m_Parameter.Size().width, m_Parameter.Size().height };
		m_Pos = { m_Parameter.Position().x, m_Parameter.Position().y };

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

	void Render(CommandCollection& d) override
	{
		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		int _channels = m_Slider.Channels();

		int _width = m_Slider.Size().width - 16;
		int _x = 6;
		int _y = 5;
		int _rh = Height();
		int _w = (_width / _channels) - 2;
		int _0db = ((std::powf(1, 0.25) / 1.412536) * (_rh)) + _y;

		// Draw all audio meters
		for (int i = 0; i < _channels; i++)
		{
			_x = 9 + i * (_w + 2);
			float _level1 = std::powf(m_Slider.Values()[i], 0.25);
			float _level2 = std::powf(m_Slider.Reduces()[i], 0.25);

			int _h1 = (std::min(_level1, 1.412536f) / 1.412536) * (_rh);
			int _h2 = (std::min(_level2, 1.412536f) / 1.412536) * (_rh);
			d.Command<Graphics::Fill>(theme->Get(C::VMeter));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
			d.Command<Graphics::Fill>(theme->Get(C::Channel));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFillC1));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h1});
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFill));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h2});

		}

		// db numbers besides volume meter
		int _d = 3;
		if (Height() < 200)
			_d = 6;
			
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
		for (int i = 12; i > -120; i -= _d)
		{
			if (Height() < 200)
			{
				if (i < -11)
					_d = 12;
				if (i < -35)
					_d = 36;
				if (i < -86)
					break;
			}
			else
			{
				if (i < -11)
					_d = 6;
				if (i < -47)
					_d = 12;
				if (i < -71)
					_d = 24;
			}

			if (_b)
				d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
			else
				d.Command<Graphics::Fill>(theme->Get(C::VMeterIndD));

			int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
			d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
			if (_b)
			{
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i, std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(theme->Get(C::TextOff));
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(theme->Get(C::VMeterIndB));
		d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});
		d.Command<Graphics::Fill>(theme->Get(C::TextOff));
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y});
		d.Command<PopMatrix>();
		SliderBase<VolumeSliderGraphics>::Render(d);
	}
private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	VolumeSlider& m_Slider;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename T>
class VolumeSliderG : public SliderBase<T>
{
public:
	using Parent = SliderBase<T>;

	VolumeSliderG()
		: Parent(m_Parameter)
	{
		m_Parameter.Range({0.0, 3.98107});
		m_Parameter.Power(4);
		m_Parameter.Value(1);
		m_Parameter.ResetValue(1);
		m_Parameter.Vertical(true);
	}

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

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

private:
	Parameter m_Parameter;
};

using OldVolumeSlider = VolumeSliderG<VolumeSliderGraphics>;

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename Graphics>
class PanSliderG : public SliderBase<Graphics>
{
public:
	using Parent = SliderBase<Graphics>;

	PanSliderG(const std::string& name = "")
		: m_Parameter(name, ParameterType::Slider), Parent(m_Parameter)
	{
		m_ValueText.reserve(10);
		m_Parameter.Range({ -50, 50 });
		m_Parameter.Value(0);
		m_Parameter.Vertical(false);
		m_Parameter.ResetValue(0);

		m_Listener += [this](Event::MouseEntered& e)
		{ m_Hovering = true; };
		m_Listener += [this](Event::MouseExited& e)
		{ m_Hovering = false; };
	}

	void Update(const Vec4<int>& v) override
	{
		Component::Update(v);

		char s[10];
		if (m_Parameter.Value() == 0)
			m_ValueText = "C";
		else if (m_Parameter.Value() < 0) {
			std::sprintf(s, "%.0f", std::abs(m_Parameter.Value()));
			m_ValueText = s;
			m_ValueText += "L";
		}
		else {
			std::sprintf(s, "%.0f", m_Parameter.Value());
			m_ValueText = s;
			m_ValueText += "R";
		}
	}
private:
	Parameter m_Parameter;
};

using PanSlider = PanSliderG<PanSliderGraphics>;
using PanKnob = PanSliderG<KnobSliderGraphics>;

using KnobSlider = SliderBase<KnobSliderGraphics>;
using NormalSlider = SliderBase<SliderGraphics>;



class DynamicsSlider : public Container
{
public:
	DynamicsSlider(DynamicsObject& o)
		: m_Object(o)
	{
		UpdateStrings();

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button != Event::MouseButton::LEFT || e.x < X() || e.x > X() + Width())
				return;

			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
				m_Dragging = RT2, m_PressVal = m_Object.ExpanderRatio();
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
				m_Dragging = RT1, m_PressVal = m_Object.CompressorRatio();
			else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
				m_Dragging = TH2, m_PressVal = DbToPixel(m_Object.ExpanderThreshhold());
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
				m_Dragging = TH1, m_PressVal = DbToPixel(m_Object.CompressorThreshhold());
		};
		m_Listener += [this](Event::MouseClicked& e)
		{
			int p = m_Click;
			m_Click = 20;
			
			if (!p)
				return; 

			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
				m_Object.ExpanderRatio(0);
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
				m_Object.CompressorRatio(0);
			else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
				m_Object.ExpanderThreshhold(-50), m_Object.CompressorThreshhold(std::max(m_Object.CompressorThreshhold(), -50.0));
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
				m_Object.CompressorThreshhold(-10), m_Object.ExpanderThreshhold(std::min(m_Object.ExpanderThreshhold(), -10.0));

			UpdateStrings();
		};
		m_Listener += [this](Event::MouseMoved& e)
		{
			m_PVal = constrain(e.x - X(), 0, Width());
			if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
			else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
				m_Cursor = GLFW_HAND_CURSOR;
			else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
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
				m_Object.ExpanderThreshhold(db1);

				if (m_Object.ExpanderThreshhold() > m_Object.CompressorThreshhold())
					m_Object.CompressorThreshhold(m_Object.ExpanderThreshhold());
			
				UpdateStrings();
			}
			else if (m_Dragging == TH1)
			{
				double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Width()));
				m_Cursor = GLFW_HAND_CURSOR;
				m_Object.CompressorThreshhold(db1);

				if (m_Object.ExpanderThreshhold() > m_Object.CompressorThreshhold())
					m_Object.ExpanderThreshhold(m_Object.CompressorThreshhold());

				UpdateStrings();
			}
			else if (m_Dragging == RT1)
			{
				double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
				m_PVal = cval;
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
				m_Object.CompressorRatio(constrain(db1 + m_Object.CompressorRatio(), -31, 32));
				UpdateStrings();
			}
			else if (m_Dragging == RT2)
			{
				double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
				m_PVal = cval;
				m_Cursor = GLFW_RESIZE_EW_CURSOR;
				m_Object.ExpanderRatio(constrain(m_Object.ExpanderRatio() - db1, -31, 32));
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
		m_Pos.x = m_Object.Position().x;
		m_Pos.y = m_Object.Position().y;
		m_Size.width = m_Object.Size().width;
		m_Size.height = m_Object.Size().height;

		if (m_Click)
			m_Click--;

		UpdateStrings();

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
		int p1 = DbToPixel(m_Object.CompressorThreshhold());
		d.Command<Graphics::Fill>(theme->Get(C::DynamicsB));
		d.Command<Graphics::Quad>(Vec4<int>{ p1, _y2, Width() - p1, _y - _y2 - 5 });
		double _xp = p1;
		for (int i = 0; i < 8; i++)
		{
			auto c = theme->Get(C::DynamicsL);
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
			_xp += m_Object.CompressorRatio() * 0.5 + 17;
			if (_xp > Width())
				break;
		}

		int p2 = DbToPixel(m_Object.ExpanderThreshhold());
		d.Command<Graphics::Fill>(theme->Get(C::DynamicsB));
		d.Command<Graphics::Quad>(Vec4<int>{ 0, _y2, p2, _y - _y2 - 5 });
		_xp = p2;
		for (int i = 0; i < 8; i++)
		{
			auto c = theme->Get(C::DynamicsL);
			c.a *= (8 - i) / 8.0f;
			d.Command<Graphics::Fill>(c);
			d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
			_xp -= m_Object.ExpanderRatio() * 0.5 + 17;
			if (_xp < 0)
				break;
		}
		d.Command<Fill>(theme->Get(C::TextSmall));
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<TextAlign>(Align::LEFT, Align::TOP);
		d.Command<Text>(&m_TH2Str, Vec2<int>{0, -3});
		d.Command<Text>(&m_RT2Str, Vec2<int>{60, -3});
		d.Command<Text>(&m_RT1Str, Vec2<int>{Width() - 100, -3});
		d.Command<TextAlign>(Align::RIGHT, Align::TOP);
		d.Command<Text>(&m_TH1Str, Vec2<int>{Width(), -3});

		int _channels = m_Object.Channels();
		if (_channels)
		{
			int _x = 0;
			int _rw = m_Object.Size().width;
			int _p = 16;
			int _h = ((m_Object.Size().height - 21 - _p * 2) / _channels) - (_channels > 4 ? 1 : 2);
			_y = _p;

			// Draw all audio meters
			for (int i = 0; i < _channels; i++)
			{
				_y = _p + i * (_h + (_channels > 4 ? 1 : 2));
				float _level = std::powf(m_Object.Levels()[i], 0.25);

				int _w = (std::min(_level, 1.0f) / 1.0f) * (_rw);
				d.Command<Graphics::Fill>(theme->Get(C::VMeterFill));
				d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
			}
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

	std::string m_TH1Str = "";
	std::string m_TH2Str = "";
	std::string m_RT1Str = "1:8";
	std::string m_RT2Str = "1:8";
private:
	DynamicsObject& m_Object;
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
		std::sprintf(s, "%.1f", m_Object.CompressorThreshhold());
		m_TH1Str = s;
		m_TH1Str += "dB";
		std::sprintf(s, "%.1f", m_Object.ExpanderThreshhold());
		m_TH2Str = s;
		m_TH2Str += "dB";
		double rt1 = m_Object.CompressorRatio();
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
		double rt2 = m_Object.ExpanderRatio();
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