#pragma once
#include <nlohmann/json.hpp>
#include <GuiCode/data/Vec.hpp>

using namespace nlohmann;

enum class ParameterType
{
	Knob, Slider
};

class Parameter
{
public:
	Parameter(const std::string& name = "", ParameterType t = ParameterType::Knob)
		: m_Name(name), m_Type(t)
	{}

	virtual void   Name(const std::string& n) { m_Name = n; }
	virtual auto   Name() -> std::string& { return m_Name; }
	virtual void   Range(const Vec2<double>& r) { m_Range = r; ConstrainValue(); }
	virtual auto   Range() -> Vec2<double> const { return m_Range; }
	virtual void   Value(double v) { m_Value = Normalize(v); ConstrainValue(); }
	virtual double Value() const { return Convert(m_Value); }
	virtual double NormalizedValue() const { return m_Value; }
	virtual void   NormalizedValue(double v) { m_Value = v; }
	virtual void   ResetValue(double v) { m_ResetValue = v; }
	virtual void   ResetValue() { m_Value = Normalize(m_ResetValue); }
	virtual void   Multiplier(double v) { m_Mult = v; }
	virtual double Multiplier() { return m_Mult; }
	virtual void   Power(double v) { m_Power = v; }
	virtual double Power() { return m_Power; }
	virtual bool   Vertical() { return m_Vertical; }
	virtual void   Vertical(bool v) { m_Vertical = v; }
	virtual void   Unit(const std::string& str, int tenp = 0) { m_Units.emplace(tenp, str); }
	virtual void   Decimals(int d) { m_Decimals = d; }
	virtual int    Decimals() { return m_Decimals; }
	virtual bool   DisplayValue() { return m_DisplayValue; }
	virtual void   DisplayValue(bool v) { m_DisplayValue = v; }
	virtual bool   DisplayName() { return m_DisplayName; }
	virtual void   DisplayName(bool v) { m_DisplayName = v; }
	virtual bool   Disabled() { return m_Disabled; }
	virtual void   Disable() { m_Disabled = true; }
	virtual void   Enable() { m_Disabled = false; }
	virtual void   Size(const Vec2<int>& size) { m_Size = size; }

protected:
	int m_Decimals = 1;

	Vec2<double> m_Range{ 0, 100 };

	Vec2<int> m_Size{ 30, 30 };

	double m_Value = 0,
		m_Power = 1,
		m_ResetValue = 0,
		m_Mult = 1;

	bool m_Vertical = true,
		m_DisplayValue = true,
		m_DisplayName = true,
		m_Disabled = false;

	std::string m_Name;
	std::unordered_map<int, std::string> m_Units;

	const ParameterType m_Type;

	void ConstrainValue() { m_Value = constrain(m_Value, 0, 1); }
	double Convert(double v) const { return std::powf(v, m_Power) * (m_Range.end - m_Range.start) + m_Range.start; }
	double Normalize(double v) const { return std::powf((v - m_Range.start) / (m_Range.end - m_Range.start), 1.0 / m_Power); }
};

class EffectBase
{
public:
	EffectBase(const std::string& name) : 
		m_Name(name) 
	{};

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual float NextSample(float, int) = 0;
	virtual void Channels(int c) {}

	virtual operator json() = 0;
	virtual void operator=(const json& json) = 0;

	void   SampleRate(double s) { m_SampleRate = s; }
	double SampleRate() { return m_SampleRate; }

protected:
	const std::string m_Name = "";
	double m_SampleRate = 48000;
};