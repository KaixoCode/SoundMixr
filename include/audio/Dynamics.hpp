#pragma once
#include "audio/Effects.hpp"

class Compressor
{
public:
	static inline const double DC_OFFSET = 1.0E-25;

	double expanderThreshhold = -50;
	double compressThreshhold = -40;
	double expanderRatio = 8.0 / 1.0;
	double compressRatio = 1.0 / 8.0;

	double expanderEnv = DC_OFFSET;
	double compressEnv = DC_OFFSET;
	double attms = 1;
	double relms = 100;
	double attcoef = std::exp(-1.0 / ((attms / 1000.0) * Effect::sampleRate));
	double relcoef = std::exp(-1.0 / ((relms / 1000.0) * Effect::sampleRate));

	double pregain = 0;
	double postgain = 0;
	double mix = 0;

	double expanderMult = 1;
	double compressMult = 1;

	double biggest = 0.0;

	int zerocounter = 0;

	void Attack(float ms)
	{
		double newval = ms;
		if (newval != attms)
		{
			attms = newval;
			attcoef = std::exp(-1.0 / ((attms / 1000.0) * Effect::sampleRate));
		}

	}

	void Release(float ms)
	{
		float newval = ms;
		if (newval != relms)
		{
			relms = newval;
			relcoef = std::exp(-1.0 / ((relms / 1000.0) * Effect::sampleRate));
		}
	}

	float NextSample(float sin, int c)
	{
		if (sin == 0 && zerocounter <= 100)
			zerocounter++;
		else if (sin != 0)
			zerocounter = 0;

		if (zerocounter > 100)
			return 0;

		float out = 0;
		if (c != 0)
		{
			float abs = sin;
			myabs(abs);
			if (biggest < abs)
				biggest = abs;
			out = sin * pregain * compressMult * expanderMult * postgain;
		}
		else
		{
			double s = biggest;
			biggest = 0;
			s *= pregain;
			float _absSample = s;
			myabs(_absSample);

			// convert key to dB
			_absSample += DC_OFFSET;	// add DC offset to avoid log( 0 )
			double _absSampledB = lin2db(_absSample); // convert linear -> dB

			// threshold
			double _overdB = _absSampledB - expanderThreshhold;
			if (_overdB > 0.0)
				_overdB = 0.0;

			// attack/release
			_overdB += DC_OFFSET; // add DC offset to avoid denormal	
			if (_overdB > expanderEnv)
				expanderEnv = _overdB + attcoef * (expanderEnv - _overdB);
			else
				expanderEnv = _overdB + relcoef * (expanderEnv - _overdB);

			_overdB = expanderEnv - DC_OFFSET; // subtract DC offset

				// transfer function
			double _gr = _overdB * (expanderRatio - 1.0) * mix;
			expanderMult = db2lin(_gr); // convert dB -> linear

			// output gain expander
			s *= expanderMult;

			// Absolute of new sample
			_absSample = s;
			myabs(_absSample);

			// convert key to dB
			_absSample += DC_OFFSET;	// add DC offset to avoid log( 0 )
			_absSampledB = lin2db(_absSample); // convert linear -> dB

			// threshold
			_overdB = _absSampledB - compressThreshhold;
			if (_overdB < 0.0)
				_overdB = 0.0;

			// attack/release
			_overdB += DC_OFFSET; // add DC offset to avoid denormal	
			if (_overdB > compressEnv)
				compressEnv = _overdB + attcoef * (compressEnv - _overdB);
			else
				compressEnv = _overdB + relcoef * (compressEnv - _overdB);
			_overdB = compressEnv - DC_OFFSET;// subtract DC offset

			// transfer function
			_gr = _overdB * (compressRatio - 1.0) * mix;
			compressMult = db2lin(_gr); // convert dB -> linear

			// output gain
			s;
			out = sin * pregain * compressMult * expanderMult * postgain;
		}
	}

	float Coeficient(float ms) { return std::exp(-1.0 / ((ms / 1000.0) * Effect::sampleRate)); }
};


// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Dynamics : public Effect
{
public:
	Dynamics();

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	void Channels(int c) override;
	float NextSample(float sin, int c) override;

	void UpdateParams()
	{
		m_Compressor.compressRatio = m_Slider.ratio1 >= 0 ? m_Slider.ratio1 / 32.0 + 1 : (-1.0 / (m_Slider.ratio1 - 1.0));
		m_Compressor.expanderRatio = m_Slider.ratio2 >= 0 ? m_Slider.ratio2 + 1 : (-1.0 / (m_Slider.ratio2 / 8.0 - 1.0));
		m_Compressor.compressThreshhold = m_Slider.threshhold1;
		m_Compressor.expanderThreshhold = m_Slider.threshhold2;
		m_Compressor.Attack(m_Attack.Value());
		m_Compressor.Release(m_Release.Value());
		m_Compressor.pregain = db2lin(m_PreGain.Value());
		m_Compressor.postgain = db2lin(m_PostGain.Value());
		m_Compressor.mix = m_Mix.Value() * 0.01;
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Dynamics";
		_json["expt"] = m_Slider.threshhold2;
		_json["comt"] = m_Slider.threshhold1;
		_json["expr"] = m_Slider.ratio2;
		_json["comr"] = m_Slider.ratio1;
		_json["att"] = m_Attack.Value();
		_json["ret"] = m_Release.Value();
		_json["prg"] = m_PreGain.Value();
		_json["pog"] = m_PostGain.Value();
		_json["mix"] = m_Mix.Value();
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
		m_Attack.Value(p5);
		m_Release.Value(p6);
		m_PreGain.Value(p7);
		m_PostGain.Value(p8);
		m_Mix.Value(p9);
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
		&m_PreGain,
		&m_Attack,
		&m_Release,
		&m_PostGain,
		&m_Mix;

	DynamicsSlider& m_Slider;

	std::vector<float> m_Levels;
	std::vector<float> m_Peaks;

	int counter = 0;
	double r = 0.9;

	Compressor m_Compressor;
};
