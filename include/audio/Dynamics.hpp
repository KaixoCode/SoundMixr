#pragma once
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Dynamics : public Effect
{
public:
	Dynamics(int channels);

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	float Coeficient(float ms) { return std::exp(-1.0 / ((ms / 1000.0) * sampleRate)); }

	void Channels(int c) override;

	float NextSample(float sin, int c) override;

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
