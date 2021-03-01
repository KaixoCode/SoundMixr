#pragma once
#include "ui/Graphics.hpp"

static inline float db2lin(float db) { // dB to linear
	return powf(10.0f, 0.05f * db);
}

static inline float lin2db(float lin) { // linear to dB
	return 20.0f * log10f(lin);
}

class Effect : public Panel
{


};

class Dynamics : public Effect
{
public:


	Dynamics()
	{
		Background(Theme<C::Channel>::Get());
	}

	void Update(const Vec4<int>& v)
	{
		Background(Theme<C::Channel>::Get());
		Effect::Update(v);
	}

	static inline const double DC_OFFSET = 1.0E-25;

	double expanderThreshhold = -50;
	double compressThreshhold = -40;
	double expanderRatio = 8.0 / 1.0;
	double compressRatio = 1.0 / 8.0;

	double expanderEnv = DC_OFFSET;
	double compressEnv = DC_OFFSET;

	double sampleRate = 480000;
	double attms = 1;
	double relms = 100;
	double attcoef = exp(-1000.0 / (attms * sampleRate));
	double relcoef = exp(-1000.0 / (relms * sampleRate));

	double makeup = 10;

	float NextSample(float s)
	{
		float _absSample = std::fabs(s);

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
		double _gr = _overdB * (expanderRatio - 1.0);	
		_gr = db2lin(_gr); // convert dB -> linear

		// output gain expander
		s =  s * _gr;

		// Absolute of new sample
		_absSample = std::fabs(s);

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
		_gr = _overdB * (compressRatio - 1.0) + makeup;
		_gr = db2lin(_gr); // convert dB -> linear

		// output gain
		s = s * _gr;

		return s;
	}

};


class EffectsGroup : public Panel
{
public:
	
	EffectsGroup()
	{
		Layout<Layout::Stack>(8);
		AutoResize(false, true);
	}

	template<typename T>
	T& Emplace()
	{
		return Container::Emplace<T>();
	}
};
