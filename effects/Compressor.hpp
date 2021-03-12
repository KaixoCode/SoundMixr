#pragma once
#include <algorithm>
#include <cmath>


#define db2lin(db) std::powf(10.0f, 0.05 * (db))
#define lin2db(lin) (20.0f * std::log10f(lin))
#define myabs(f) if (f < 0) f = -f;


class Compressor
{
public:
	double sampleRate = 48000;
	static inline const double DC_OFFSET = 1.0E-25;

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

	double biggest = 0.0;

	int zerocounter = 0;

	void Attack(float ms)
	{
		double newval = ms;
		if (newval != attms)
		{
			attms = newval;
			attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
		}

	}

	void Release(float ms)
	{
		float newval = ms;
		if (newval != relms)
		{
			relms = newval;
			relcoef = std::exp(-1.0 / ((relms / 1000.0) * sampleRate));
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

		return out;
	}

	float Coeficient(float ms) { return std::exp(-1.0 / ((ms / 1000.0) * sampleRate)); }
};