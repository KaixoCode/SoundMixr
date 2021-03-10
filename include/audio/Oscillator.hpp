#pragma once
#include "pch.hpp"
#include "Effects.hpp"

class Oscillator
{
public:
	float NextSample()
	{
        double delta = frequency / Effect::sampleRate;
        phase = std::fmod(1 + phase + delta, 1);
        sample = wavetable(phase);
        return sample;
	}

    double(*wavetable)(double) = [](double p) { return 2 * (-p + 0.5); };

    double sample = 0;
	double frequency = 0;
	double phase = 0;
};