#pragma once
#include "pch.hpp"
#include "Midi.hpp"
#include "Audio/Parameter.hpp"

struct Parameter : public Component
{
	struct Settings
	{
		std::string name;
		Vec2<float> range{ 0, 100 };
		float value = 0;
		float reset = 0;
		float shift = 0.25;
		bool vertical = true;
		Function<float(float)> scaling = [](float in) { return in; };
		Function<float(float)> inverse = [](float in) { return in; };
		Audio::Parameter::MidiLink midi;
	};

	Settings settings;

	std::string& name = settings.name;
	Vec2<float>& range = settings.range;
	float& value = settings.value;
	float& reset = settings.reset;
	float& shift = settings.shift;
	bool& vertical = settings.vertical;
	Function<float(float)>& scaling = settings.scaling;
	Function<float(float)>& inverse = settings.inverse;
	Audio::Parameter::MidiLink& midi = settings.midi;
	
	Menu menu;

	Parameter(const Settings& settings = {});
	Parameter(Parameter&&) = delete;
	Parameter(const Parameter&) = delete;

	float Normalize(float v) const { return (inverse(v) - range.start) / (range.end - range.start); }
	float Unnormalize(float v) const { return scaling(v) * (range.end - range.start) + range.start; }

private:
	std::chrono::steady_clock::time_point m_ChangeTime;
	float m_PressVal = 0;
	float m_PrevPos = 0;
	bool m_Linking = false;
	Midi::EventStorage m_CCLink;
};

using Param = Parameter::Settings;