#pragma once
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Dynamics : public Effect
{
public:
	Dynamics();

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	float Coeficient(float ms) { return std::exp(-1.0 / ((ms / 1000.0) * sampleRate)); }

	void Channels(int c) override;
	float NextSample(float sin, int c) override;

	void UpdateParams()
	{
		compressRatio = m_Slider.ratio1 >= 0 ? m_Slider.ratio1 / 32.0 + 1 : (-1.0 / (m_Slider.ratio1 - 1.0));
		expanderRatio = m_Slider.ratio2 >= 0 ? m_Slider.ratio2 + 1 : (-1.0 / (m_Slider.ratio2 / 8.0 - 1.0));
		compressThreshhold = m_Slider.threshhold1;
		expanderThreshhold = m_Slider.threshhold2;
		double newval = m_Knob2.Value();
		if (newval != attms)
		{
			attms = newval;
			attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
		}
		newval = m_Knob3.Value();
		if (newval != relms)
		{
			relms = newval;
			relcoef = std::exp(-1.0 / ((relms / 1000.0) * sampleRate));
		}

		pregain = db2lin(m_Knob.Value());
		postgain = db2lin(m_Knob4.Value());
		mix = m_Knob5.Value() * 0.01;
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Dynamics";
		_json["expt"] = m_Slider.threshhold2;
		_json["comt"] = m_Slider.threshhold1;
		_json["expr"] = m_Slider.ratio2;
		_json["comr"] = m_Slider.ratio1;
		_json["att"] = m_Knob2.Value();
		_json["ret"] = m_Knob3.Value();
		_json["prg"] = m_Knob.Value();
		_json["pog"] = m_Knob4.Value();
		_json["mix"] = m_Knob5.Value();
		return _json;
	}

	void operator=(const json& json)
	{
		double p1 = json["expt"].get<double>();
		double p2 = json["comt"].get<double>();
		double p3 = json["expr"].get<double>();
		double p4 = json["comr"].get<double>();
		double p5 = json["att"].get<double>();
		double p6 = json["ret"].get<double>();
		double p7 = json["prg"].get<double>();
		double p8 = json["pog"].get<double>();
		double p9 = json["mix"].get<double>();
		m_Slider.threshhold2 = p1;
		m_Slider.threshhold1 = p2;
		m_Slider.ratio2 = p3;
		m_Slider.ratio1 = p4;
		m_Knob2.Value(p5);
		m_Knob3.Value(p6);
		m_Knob.Value(p7);
		m_Knob4.Value(p8);
		m_Knob5.Value(p9);
		UpdateParams();
	}

private:
	static inline const double DC_OFFSET = 1.0E-25;

	static inline std::string
		m_Knob1Name = "PreGain",
		m_Knob2Name = "Attack",
		m_Knob3Name = "Release",
		m_Knob4Name = "PostGain",
		m_Knob5Name = "Mix";

	KnobSlider
		&m_Knob,
		&m_Knob2,
		&m_Knob3,
		&m_Knob4,
		&m_Knob5;

	DynamicsSlider& m_Slider;

	std::vector<float> m_Levels;
	std::vector<float> m_Peaks;

	double expanderThreshhold = -50;
	double compressThreshhold = -40;
	double expanderRatio = 8.0 / 1.0;
	double compressRatio = 1.0 / 8.0;

	double expanderEnv = DC_OFFSET;
	double compressEnv = DC_OFFSET;
	double attms = 1;
	double relms = 100;
	double attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
	double relcoef = std::exp(-1.0 / ((relms / 1000.0) * sampleRate));

	double pregain = 0;
	double postgain = 0;
	double mix = 0;

	double expanderMult = 1;
	double compressMult = 1;

	int counter = 0;
	double biggest = 0.0;
	double r = 0.9;
};
