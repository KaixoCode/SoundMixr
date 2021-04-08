#pragma once
#include "pch.hpp"
#include "ui/Parameter.hpp"
#include "ui/Graphics.hpp"

/**
 * This file contains all components used by Effects, they're all gui wrappers for the
 * components in the EffectBase project.
 */

using Knob = Parameter<KnobGraphics>;
using Slider = Parameter<SliderGraphics>;

// -------------------------------------------------------------------------- \\
// ---------------------------- Radio Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

class RadioButton : public Button<RadioButtonGraphics, ButtonType::List>
{
public:
	RadioButton(Effects::RadioButton& t, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons);

	void Update(const Vec4<int>& v);

private:
	std::unordered_map<int, int> &m_Keys;
	std::unordered_map<int, std::vector<Effects::RadioButton*>>& m_RButtons;

	Effects::RadioButton& m_Toggle;

	static inline int GetKey(Effects::RadioButton& k, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons);
};

// -------------------------------------------------------------------------- \\
// --------------------------- Toggle Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

class ToggleButton : public Button<ToggleButtonGraphics, ButtonType::Toggle>
{
public:
	ToggleButton(Effects::ToggleButton& t);

	void Update(const Vec4<int>& v);

private:
	Effects::ToggleButton& m_Toggle;
};

// -------------------------------------------------------------------------- \\
// --------------------------- XY Controller -------------------------------- \\
// -------------------------------------------------------------------------- \\

class XYController : public Container
{
public:
	XYController(Effects::XYController& c);

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;

private:
	int m_Click = 0;
	bool m_Dragging = false,
		m_Hovering = false;
	Effects::XYController& controller;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

class VolumeSlider : public Parameter<VolumeSliderGraphics>
{
public:
	VolumeSlider(Effects::VolumeSlider& s);

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };
	Effects::VolumeSlider& Slider() { return m_Slider; }

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	Effects::VolumeSlider& m_Slider;
};

// -------------------------------------------------------------------------- \\
// ------------------------- Old Volume Slider ------------------------------ \\
// -------------------------------------------------------------------------- \\

class OldVolumeSlider : public Parameter<VolumeSliderGraphics>
{
public:
	using Parent = Parameter<VolumeSliderGraphics>;

	OldVolumeSlider();

	double Decibels() { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

	void Update(const Vec4<int>& v);

	virtual operator nlohmann::json() { return m_Parameter.operator nlohmann::json(); };
	virtual void operator=(const nlohmann::json& json) { m_Parameter = json; };

private:
	Effects::Parameter m_Parameter{ "Volume", Effects::ParameterType::Slider };
};

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

class PanSlider : public Parameter<PanSliderGraphics>
{
public:
	PanSlider();

	virtual operator nlohmann::json() { return m_Parameter.operator nlohmann::json(); };
	virtual void operator=(const nlohmann::json& json) { m_Parameter = json; };

private:
	Effects::Parameter m_Parameter{ "Pan", Effects::ParameterType::Slider };
};

// -------------------------------------------------------------------------- \\
// -------------------------- Dynamics Slider ------------------------------- \\
// -------------------------------------------------------------------------- \\

class DynamicsSlider : public Container
{
public:
	DynamicsSlider(Effects::DynamicsSlider& o);

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	double PixelToDb(int p);
	int DbToPixel(double p);

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";
	static constexpr int 
		TH1 = 1,
		TH2 = 2,
		RT1 = 3,
		RT2 = 4;

	Effects::DynamicsSlider& m_Object;
	
	std::string 
		m_TH1Str = "",
		m_TH2Str = "",
		m_RT1Str = "1:8",
		m_RT2Str = "1:8";

	int m_Click = 0,
		m_PVal = 0,
		m_Dragging = 0,
		m_DragRange = 8;
	
	double m_PressVal = 0;
	bool m_Shift = false;

	void UpdateStrings();
};


// -------------------------------------------------------------------------- \\
// ---------------------------- Filter Curve -------------------------------- \\
// -------------------------------------------------------------------------- \\

class FilterCurve : public Component
{
public:

	FilterCurve(Effects::FilterCurve& params)
		: m_Curve(params)
	{}


	void Update(const Vec4<int>& v)
	{
		m_Size = { m_Curve.Size().width, m_Curve.Size().height };
		m_Pos = { m_Curve.Position().width, m_Curve.Position().height };
		UpdateMags();
		Component::Update(v);
	}

	void Render(CommandCollection& d)
	{
		Component::Render(d);

		int size = Width() / m_Scale;
		d.Command<Graphics::Fill>(Color{ 255, 255, 0, 255 });
		for (int i = 0; i < size-1; i++)
		{
			float y1 = DbToY(m_Mags[i]);
			float y2 = DbToY(m_Mags[i + 1]);
			float x1 = X() + i * m_Scale;
			float x2 = X() + (i + 1) * m_Scale;
			d.Command<Graphics::Line>(Vec4<float>{ x1, y1, x2, y2 }, 3.0f);
		}

	}


private:

	void UpdateMags()
	{
		int size = Width() / m_Scale;
		while (m_Mags.size() < size)
			m_Mags.push_back(0);

		for (int i = 0; i < size; i++)
		{
			float freq = PosToFreq(i * m_Scale);
			float nfreq = PosToFreq((i+1) * m_Scale);
			
			float min = 24;
			int count = 0;
			for (float j = freq; j < nfreq; j += (nfreq - freq) * 0.1)
			{
				count++;
				float ma = Magnitude(j);
				if (ma < min) min = ma;
			}
			m_Mags[i] = min;
		}
	}

	float PosToFreq(int x)
	{
		if (x <= 0)
			return 0;

		return (x / (float)Width()) * 22000;

		return std::pow(10, (x / (float)Width()) * LOG1022) + 10;
	}

	float DbToY(float db)
	{
		if (db < -12)
			return Y() + Height();
		if (db > 12)
			return Y();

		return Y() + Height() - ((db + 12) / 24.0) * Height();
	}

	float FreqToPos(float freq)
	{
		return std::log10((freq / 22000)) * Width() - 1.0f;
	}

	float Magnitude(float freq);

	static inline const float LOG1022 = std::log10(21990);

	int m_Scale = 1;

	std::vector<float> m_Mags;

	Effects::FilterCurve& m_Curve;
};


// -------------------------------------------------------------------------- \\
// ---------------------------- Filter Curve -------------------------------- \\
// -------------------------------------------------------------------------- \\

class SimpleFilterCurve : public Component
{
public:

	SimpleFilterCurve(Effects::SimpleFilterCurve& params)
		: m_Curve(params)
	{
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT && e.x > X() && e.x < X() + Width() && e.y > Y() && e.y < Y() + Height())
				m_Dragging = true;

		};

		m_Listener += [this](Event::MouseClicked& e)
		{
			if (e.button == Event::MouseButton::LEFT)
			{
				if (m_Click > 0)
					m_Curve.freq.ResetValue(), m_Curve.width.ResetValue();
				m_Click = 20;
			}
		};

		m_Listener += [this](Event::MouseEntered& e)
		{
			m_Hovering = true;
		};

		m_Listener += [this](Event::MouseExited& e)
		{
			m_Hovering = false;
		};

		m_Listener += [this](Event::MouseDragged& e)
		{
			if (m_Dragging)
			{
				double _x = constrain((e.x - X() - 8) / (Width() - 8 * 2.0), 0, 1);
				double _y = constrain((e.y - Y() - 8) / (Height() - 8 * 2.0), 0, 1);
				if (!m_Curve.freq.Disabled()) 
					m_Curve.freq.NormalizedValue(_x);
				if (!m_Curve.width.Disabled())
					m_Curve.width.NormalizedValue(_y);
			}
		};

		m_Listener += [this](Event::MouseReleased& e)
		{
			m_Dragging = false;
		};
		m_Listener += [this](Event::KeyPressed& e)
		{
			if (!Focused())
				return;

			double amt = 0.01;
			if (e.keymod & Event::Mod::CONTROL)
				amt *= 4;
			else if (e.keymod & Event::Mod::SHIFT)
				amt *= 0.1;

			if (e.key == Key::LEFT)
				m_Curve.freq.NormalizedValue(m_Curve.freq.NormalizedValue() - amt);

			if (e.key == Key::RIGHT)
				m_Curve.freq.NormalizedValue(m_Curve.freq.NormalizedValue() + amt);

			if (e.key == Key::UP)
				m_Curve.width.NormalizedValue(m_Curve.width.NormalizedValue() + amt);

			if (e.key == Key::DOWN)
				m_Curve.width.NormalizedValue(m_Curve.width.NormalizedValue() - amt);
	
		};
	}
	


	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

private:

	void UpdateMags()
	{
		if (m_Dragging) m_Update = true;


		int size = std::ceil(Width() / m_Scale) + m_Scale;
		while (m_Mags.size() < size)
			m_Mags.push_back(0), m_Update = true;

		if (m_Update)
		{
			for (int i = 0; i < size; i++)
			{
				float ma = Magnitude(i * m_Scale);

				m_Mags[i] = ma;
			}
			m_Update = false;
		}

		if (m_PrevFreq != m_Curve.freq.Value())
			m_PrevFreq = m_Curve.freq.Value(), m_Update = true;

		if (m_PrevWidth != m_Curve.width.Value())
			m_PrevWidth = m_Curve.width.Value(), m_Update = true;
	}

	float PosToFreq(int x)
	{
		if (x <= 0)
			return 0;

		return ((std::powf(m_Log, x / (float)Width()) - 1.0) / (m_Log - 1.0)) * (21990) + 10;
	}

	int DbToY(float db)
	{
		if (db < -12)
			return Y() + Height();
		if (db > 12)
			return Y();

		return Y() + Height() - db * Height();
	}

	float FreqToPos(float freq);

	float Magnitude(float freq);

	int m_Scale = 4;

	const int m_Log = 10;
	const double m_Logg = std::log(m_Log);

	int m_Click = 0;
	bool m_Dragging = false, m_Hovering = false, m_Update = false;

	double m_PrevWidth = 0, m_PrevFreq = 0;

	std::vector<float> m_Mags;

	Effects::SimpleFilterCurve& m_Curve;
};