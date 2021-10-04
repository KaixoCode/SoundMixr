#pragma once
#include "pch.hpp"

struct ParameterParser;
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

	Parameter(const Settings& settings = {});

	float Normalize(float v) const { return (v - range.start) / (range.end - range.start); }
	float Unnormalize(float v) const { return v * (range.end - range.start) + range.start; }

private:
	std::chrono::steady_clock::time_point m_ChangeTime;
	float m_PressVal = 0;
	float m_PrevPos = 0;

	Ref<std::string> m_Name = settings.name;
	Ref<Vec2<float>> m_Range = settings.range;
	Ref<float> m_Value = settings.value;
	Ref<float> m_Reset = settings.reset;
	Ref<float> m_Shift = settings.shift;
	Ref<bool> m_Vertical = settings.vertical;
	Ref<Function<float(float)>> m_Scaling = settings.scaling;
	Ref<Function<float(float)>> m_Inverse = settings.inverse;

	friend class ParameterParser;
};

struct ParameterParser : public TagParser
{
	ParameterParser()
	{
		settings.name = "parameter";
		Attribute("name", &Parameter::m_Name);
		Attribute("range", &Parameter::m_Range);
		Attribute("value", &Parameter::m_Value);
		Attribute("reset", &Parameter::m_Reset);
		Attribute("shift", &Parameter::m_Shift);
		Attribute("vertical", &Parameter::m_Vertical);
		Attribute("scaling", &Parameter::m_Scaling);
		Attribute("inverse", &Parameter::m_Inverse);
	}

	Pointer<Component> Create()
	{
		return new Parameter{};
	}
};