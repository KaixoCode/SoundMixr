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

/**
 * Radio button is part of a group where only 1 of the buttons in the group
 * can be selected at a time.
 */
class RadioButton : public Button<RadioButtonGraphics, ButtonType::List>
{
public:
	/**
	 * Constructor
	 * @param t reference to Effects::RadioButton
	 * @param keys conversion map from effect RadioButton group key to ButtonType::List key
	 * @param buttons map from key to group of buttons
	 */
	RadioButton(Effects::RadioButton& t, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons);

	void Update(const Vec4<int>& v);

private:
	// Store a refernce to the keymap and buttongroup map received from the effect this RadioButton
	// belongs to so we can unselect the Effects::RadioButton when pressing this RadioButton.
	std::unordered_map<int, int> &m_Keys;
	std::unordered_map<int, std::vector<Effects::RadioButton*>>& m_RButtons;

	Effects::RadioButton& m_RadioButton;

	/**
	 * Look in the keymap to see if the key in m_RadioButton already exists, if it does, it takes
	 * out the converted key and adds m_RadioButton to the vector at the location of that key in the button map
	 * and then it returns the key. If it was not found it will generate a new one using ButtonType::List::NewKey(), 
	 * emplace a vector to the button map, then add the button to the map at the generated key, 
	 * and then return the new key.
	 * @param k m_RadioButton, since it cannot be initialized because this is called in the super constructor
	 * @param keys key map received from the effect
	 * @param buttons button map received from the effect
	 */
	static inline int GetKey(Effects::RadioButton& k, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons);
};

/**
 * Very simple toggle button for an effect.
 */
class ToggleButton : public Button<ToggleButtonGraphics, ButtonType::Toggle>
{
public:

	/**
	 * Constructor
	 * @param t Effects::ToggleButton
	 */
	ToggleButton(Effects::ToggleButton& t);

	void Update(const Vec4<int>& v);

private:
	Effects::ToggleButton& m_Toggle;
};

/**
 * Simple 2 dimensional controller, with a linked parameter for the x-axis and y-axis 
 */
class XYController : public Container
{
public:
	
	/**
	 * Constructor
	 * @param c Effects::XYController
	 */
	XYController(Effects::XYController& c);

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;

private:
	int m_Click = 0;
	bool m_Dragging = false,
		m_Hovering = false;

	Effects::XYController& controller;
};

/**
 * Simple volume slider, practically just a preset for a standard Parameter with some
 * additional functionality for converting the linear value to logarithmic decibels.
 */
class VolumeSlider : public Parameter<VolumeSliderGraphics>
{
public:

	/**
	 * Constructor
	 * @param s Effects::VolumeSlider
	 */
	VolumeSlider(Effects::VolumeSlider& s);

	/**
	 * Get the value of the parameter in decibels.
	 * @return linear value to decibels
	 */
	double Decibels() const { return 20 * std::log10(std::max(m_Parameter.Value(), 0.000001)); };

	/**
	 * Returns the Effects::VolumeSlider.
	 */
	Effects::VolumeSlider& Slider() { return m_Slider; }

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	virtual operator nlohmann::json() { return m_Parameter.operator nlohmann::json(); };
	virtual void operator=(const nlohmann::json& json) { m_Parameter = json; };

private:

	// Statically store the numbers to not reallocate it for each object
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	Effects::VolumeSlider& m_Slider;
};

/**
 * Only used in the channels, stores its own Effects::Parameter, basically a preset
 * for a normal Parameter, with no additional functionality.
 */
class PanSlider : public Parameter<PanSliderGraphics>
{
public:
	PanSlider();

	virtual operator nlohmann::json() { return m_Parameter.operator nlohmann::json(); };
	virtual void operator=(const nlohmann::json& json) { m_Parameter = json; };

private:
	Effects::Parameter m_Parameter{ "Pan", Effects::ParameterType::Slider };
};

/**
 * The visuals/functionality for the dynamics slider in the Dynamics effect.
 */
class DynamicsSlider : public Container
{
public:

	/**
	 * Constuctor
	 * @param o Effects::DynamicsSlider
	 */
	DynamicsSlider(Effects::DynamicsSlider& o);

	/**
	 * Convert the pixel on the screen to decibels.
	 * @param p pixel
	 * @return decibels
	 */
	double PixelToDb(int p);

	/**
 	 * Convert decibels to a pixel on the screen.
	 * @param p decibles
	 * @return pixel
	 */
	int DbToPixel(double p);

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

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

	/**
	 * Updates all the value strings.
	 */
	void UpdateStrings();
};

/**
 * Filter Curve. WIP TODO
 */
class FilterCurve : public Component
{
public:

	/**
	 * Constructor
	 */
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

/**
 * Simple Filter Curve for a bandpass with width filter only.
 */
class SimpleFilterCurve : public Component
{
public:

	/**
	 * Constructor
	 * @param params Effects::SimpleFilterCurve
	 */
	SimpleFilterCurve(Effects::SimpleFilterCurve& params);
	
	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

private:
	static inline const int m_Log = 10;
	static inline const double m_Logg = std::log(m_Log);

	int m_Scale = 4,
		m_Click = 0;

	bool m_Dragging = false, 
		m_Hovering = false, 
		m_Update = false;

	double m_PrevWidth = 0, 
		m_PrevFreq = 0;

	std::vector<float> m_Mags;

	Effects::SimpleFilterCurve& m_Curve;

	/**
	 * Updates all the magnitudes if it is necessary.
	 */
	void UpdateMags();

	/**
	 * Converts a frequency to a position on the screen.
	 * @param freq frequency
	 * @return position
	 */
	float FreqToPos(float freq);

	/**
	 * Calculates the magnitude
	 */
	float Magnitude(float freq);

};