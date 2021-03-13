#pragma once
#include <nlohmann/json.hpp>

#define DLLDIR  __declspec(dllexport)   // export DLL information

#define constrain(x, y, z) (x < y ? y : x > z ? z : x)

template<typename T>
class Tuple
{
public:
	union { T x, start, r, width; };
	union { T y, end, g, height; };
};

using namespace nlohmann;

enum class ParameterType
{
	Knob, Slider, VolumeSlider, TwoWaySlider
};

class EffectObject
{
public:
	virtual void   Size(const Tuple<int>& size) { m_Size = size; }
	virtual auto   Size() -> Tuple<int> { return m_Size; }
	virtual void   Position(const Tuple<int>& p) { m_Position = p; }
	virtual auto   Position() -> Tuple<int> { return m_Position; }

private:
	Tuple<int> m_Size{ 30, 30 }, m_Position{ 0, 0 };
};


namespace EffectLayout
{
	enum class Align
	{
		LEFT, RIGHT, TOP, BOTTOM, CENTER
	};

	enum class Type
	{
		ROWS, COLS
	};

	struct Div;
	struct Div : public EffectObject
	{
		static inline int AUTO = -1;

		enum class Type
		{
			SUB, OBJECT
		};


		Div(EffectLayout::Type t = EffectLayout::Type::COLS)
			: m_CellType(t)
		{}

		~Div()
		{}

		void CellType(EffectLayout::Type t) { m_CellType = t; }
		auto CellType() -> EffectLayout::Type { return m_CellType; }
		int  Cells() { return m_Cells; }
		void SubCellType(EffectLayout::Type t) { for (auto& i : m_Divs) i->CellType(t); }
		void SubCells(int t) { for (auto& i : m_Divs) i->Cells(t); }
		auto DivType() -> Type { return m_Type; }
		void Align(EffectLayout::Align a) { m_Align= a; }
		auto Align() -> EffectLayout::Align { return m_Align; }
		auto Object() -> EffectObject& { return *m_Object; }
		void Object(::EffectObject* o) { m_Object = o; }
		void Object(::EffectObject& o) { m_Object = &o; }
		auto Divs() ->std::vector<std::unique_ptr<Div>>& { return m_Divs; }
		void Padding(int s) { m_Padding = s; }
		int  Padding() { return m_Padding; }
		void Dividers(bool s) { m_Dividers = s; }
		bool Dividers() { return m_Dividers; }
		void CellSize(int s) { m_CellSize = s; }
		int  CellSize() 
		{
			return m_CellSize;
		}

		int  CellSize(EffectLayout::Type t)
		{
			if (m_Type == Type::OBJECT)
				return 0;

			if (m_CellSize != AUTO && t == m_CellType)
				return m_CellSize;

			int csize = t == m_CellType ? m_Padding * 2 : 0;
			for (auto& i : Divs())
				csize += i->CellSize(t);

			return csize;
		}

		void Cells(int i)
		{
			m_Cells = i;
			m_Type = Type::SUB;
			while (m_Divs.size() < i)
				m_Divs.emplace_back(std::make_unique<Div>());
		}

		Div& operator[](int index)
		{
			return *m_Divs[index];
		}

		void operator=(::EffectObject& o)
		{
			m_Type = Type::OBJECT;
			m_Object = &o;
		}

		void operator=(::EffectObject* o)
		{
			m_Type = Type::OBJECT;
			m_Object = o;
		}

	private:

		std::vector<std::unique_ptr<Div>> m_Divs;
		::EffectObject* m_Object;
		

		Type m_Type = Type::SUB;
		EffectLayout::Align m_Align = EffectLayout::Align::CENTER;
		EffectLayout::Type m_CellType = EffectLayout::Type::COLS;
		int m_CellSize = AUTO;
		int m_Cells = 0;
		int m_Padding = 0;
		bool m_Dividers = false;
	};

}

class Parameter : public EffectObject
{
public:
	Parameter(const std::string& name = "", ParameterType t = ParameterType::Knob)
		: m_Name(name), m_Type(t)
	{}

	virtual void   Name(const std::string& n) { m_Name = n; }
	virtual auto   Name() -> std::string& { return m_Name; }
	virtual void   Range(const Tuple<double>& r) { m_Range = r; ConstrainValue(); }
	virtual auto   Range() -> Tuple<double> const { return m_Range; }
	virtual void   Value(double v) { m_Value = Normalize(v); ConstrainValue(); }
	virtual double Value() const { return Convert(m_Value); }
	virtual double NormalizedValue() const { return m_Value; }
	virtual void   NormalizedValue(double v) { m_Value = v; ConstrainValue(); }
	virtual void   ResetValue(double v) { m_ResetValue = v; }
	virtual void   ResetValue() { m_Value = Normalize(m_ResetValue); }
	virtual void   Multiplier(double v) { m_Mult = v; }
	virtual double Multiplier() { return m_Mult; }
	virtual void   Power(double v) { m_Power = v; }
	virtual double Power() { return m_Power; }
	virtual bool   Vertical() { return m_Vertical; }
	virtual void   Vertical(bool v) { m_Vertical = v; }
	virtual void   Unit(const std::string& str, int tenp = 0) { m_Units.emplace(tenp, str); }
	virtual auto   Units() -> std::unordered_map<int, std::string>& { return m_Units; }
	virtual void   Decimals(int d) { m_Decimals = d; }
	virtual int    Decimals() { return m_Decimals; }
	virtual bool   DisplayValue() { return m_DisplayValue; }
	virtual void   DisplayValue(bool v) { m_DisplayValue = v; }
	virtual bool   DisplayName() { return m_DisplayName; }
	virtual void   DisplayName(bool v) { m_DisplayName = v; }
	virtual bool   Disabled() { return m_Disabled; }
	virtual void   Disable() { m_Disabled = true; }
	virtual void   Enable() { m_Disabled = false; }
	virtual auto   Type() -> ParameterType { return m_Type; }

protected:
	int m_Decimals = 1;

	Tuple<double> m_Range{ 0, 100 };

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

class VolumeSlider : public Parameter
{
public:
	void Channels(int n) 
	{
		m_Channels = n; 
		while (m_Values.size() < n) 
			m_Values.push_back(0);
		while (m_Reduces.size() < n)
			m_Reduces.push_back(0);
	}

	int  Channels() { return m_Channels; }
	auto Values() -> std::vector<float> { return m_Values; }
	auto Reduces() -> std::vector<float> { return m_Reduces; }
	void SetValue(int i, float v) { m_Values[i] = v; }
	void SetReduce(int i, float v) { m_Reduces[i] = v; }

private:
	int m_Channels;
	std::vector<float> m_Values;
	std::vector<float> m_Reduces;
};

class DropDown : public EffectObject
{
public:
	class Option
	{
	public:
		Option(const std::string& name, int i)
			: id(i), name(name) 
		{}

		int id;
		std::string name;
	};

	void AddOption(const std::string& name, int i)
	{
		if (m_Selected == -1)
			m_Selected = (int)i;
		m_Options.emplace_back(name, (int)i);
	}

	void Select(int i)
	{
		m_Selected = (int)i;
	}

	int Selected() { return m_Selected; }
	auto Options() -> std::vector<Option>& { return m_Options; }

private:
	int m_Selected = -1;
	std::vector<Option> m_Options;
};

class ToggleButton : public EffectObject
{
public:
	ToggleButton(const std::string& n)
		: m_Name(n)
	{}

	void State(bool s) { state = s; }
	bool State() { return state; }
	auto Name() -> std::string& { return m_Name; }

	bool state = false;

private:
	std::string m_Name;
};

class DynamicsObject : public EffectObject
{
public:


	void   ExpanderThreshhold(double v) { expanderThreshhold = v; }
	double ExpanderThreshhold() { return expanderThreshhold; }
	void   CompressorThreshhold(double v) { compressThreshhold = v; }
	double CompressorThreshhold() { return compressThreshhold; }
	void   ExpanderRatio(double r) { expanderRatio = r; }
	double ExpanderRatio() { return expanderRatio; }
	void   CompressorRatio(double r) { compressRatio = r; }
	double CompressorRatio() { return compressRatio; }
	void   AttackTime(double a) { attms = a; }
	double AttackTime() { return attms; }
	void   ReleaseTime(double a) { relms = a; }
	double ReleaseTime() { return relms; }
	void   PreGain(double a) { pregain = a; }
	double PreGain() { return pregain; }
	void   PostGain(double a) { postgain = a; }
	double PostGain() { return postgain; }
	void   Mix(double a) { mix = a; }
	double Mix() { return mix; }
	double Channels() { return channels; }
	auto   Levels() -> std::vector<float>& { return levels; }
	void   Level(int i, float v) { levels[i] = v; }

	void Channels(int i)
	{
		channels = i;
		while (levels.size() < i)
			levels.push_back(0);
	}

private:
	double expanderThreshhold = -50;
	double compressThreshhold = -10;
	double expanderRatio = 8.0 / 1.0;
	double compressRatio = 1.0 / 8.0;

	double attms = 1;
	double relms = 100;

	double pregain = 0;
	double postgain = 0;
	double mix = 0;

	int channels = 0;
	std::vector<float> levels;
};

class EffectBase
{
public:
	EffectBase(const std::string& name) : 
		m_Name(name) 
	{};

	virtual void Update() = 0;

	virtual float NextSample(float, int) = 0;
	virtual void Channels(int c) {}

	virtual operator json() = 0;
	virtual void operator=(const json& json) = 0;

	void   SampleRate(double s) { m_SampleRate = s; }
	double SampleRate() { return m_SampleRate; }
	auto   Name() -> const std::string& { return m_Name; }
	void   Height(int h) { m_Height = h; }
	int    Height() { return m_Height; }
	int    Width() { return 298; }

	auto   Div() -> EffectLayout::Div& { return m_Div; }



	::Parameter& Parameter(const std::string& name, ParameterType type)
	{
		return dynamic_cast<::Parameter&>(*m_EffectObjects.emplace_back(std::make_unique<::Parameter>(name, type)));
	}

	::DropDown& DropDown()
	{
		return dynamic_cast<::DropDown&>(*m_EffectObjects.emplace_back(std::make_unique<::DropDown>()));
	}

	::ToggleButton& Toggle(const std::string& name)
	{
		return dynamic_cast<::ToggleButton&>(*m_EffectObjects.emplace_back(std::make_unique<::ToggleButton>(name)));
	}

	::VolumeSlider& VolumeSlider()
	{
		return dynamic_cast<::VolumeSlider&>(*m_EffectObjects.emplace_back(std::make_unique<::VolumeSlider>()));
	}

	::DynamicsObject& Dynamics()
	{
		return dynamic_cast<::DynamicsObject&>(*m_EffectObjects.emplace_back(std::make_unique<::DynamicsObject>()));
	}

	std::vector<std::unique_ptr<::EffectObject>>& Parameters()
	{
		return m_EffectObjects;
	}

protected:
	EffectLayout::Div m_Div;
	std::vector<std::unique_ptr<::EffectObject>> m_EffectObjects;
	const std::string m_Name = "";
	double m_SampleRate = 48000;
	int m_Height;
};