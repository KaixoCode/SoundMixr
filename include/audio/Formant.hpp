#pragma once
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Formant : public Effect
{
public:
	Formant()
		: m_Knob1(Emplace<KnobSlider>("Shift")),
		m_Knob2(Emplace<KnobSlider>("Mix")),
		Effect("Formant")
	{
		Height(80);
		m_Knob1.Range({ 1, 1024 });
		m_Knob1.ResetValue(512);
		m_Knob1.ResetValue();
		m_Knob1.Unit("");
		m_Knob1.Size({ 30, 30 });
		m_Knob1.Multiplier(0.4);

		m_Knob2.Range({ 0, 100 });
		m_Knob2.ResetValue(50);
		m_Knob2.ResetValue();
		m_Knob2.Unit("%");
		m_Knob2.Size({ 30, 30 });
		m_Knob2.Multiplier(0.4);
	}

	void Update(const Vec4<int>& v) override 
	{
		m_Freq = m_Knob1.Value();
		m_Mix = m_Knob2.Value() * 0.01;
		Background(Theme<C::Channel>::Get());
		m_Knob1.Position({ Width() - 45, 20 });
		m_Knob2.Position({ Width() - 105, 20 });
		Effect::Update(v);
	}
	
	void Render(CommandCollection& d) override 
	{
		Effect::Render(d);
	}

	void Channels(int c) override
	{
		m_Buffers.reserve(c);
		while (m_Buffers.size() < c)
		{
			auto& v = m_Buffers.emplace_back();
			for (int i = 0; i < 1024; i++)
				v.push_back(i);
		}
		m_Buffers2.reserve(c);
		while (m_Buffers2.size() < c)
		{
			auto& v = m_Buffers2.emplace_back();
			for (int i = 0; i < 1024; i++)
				v.push_back(i);
		}
		m_BuffersPointers.reserve(c);
		while (m_BuffersPointers.size() < c)
			m_BuffersPointers.emplace_back();
		
		m_BuffersPointers2.reserve(c);
		while (m_BuffersPointers2.size() < c)
			m_BuffersPointers2.emplace_back();

		Effect::Channels(c);
	}
	
	std::vector<std::vector<float>> m_Buffers;
	std::vector<std::vector<float>> m_Buffers2;
	std::vector<int> m_BuffersPointers;
	std::vector<int> m_BuffersPointers2;
	int m_Freq = 128;
	double m_Mix = 0.5;
	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		auto& buffer = m_Buffers[c];
		auto& buffer2 = m_Buffers2[c];
		auto& bufferptr = m_BuffersPointers[c];
		auto& bufferptr2 = m_BuffersPointers2[c];

		buffer[bufferptr] = sin;
		bufferptr = (bufferptr + 1) % 1024;
		if (bufferptr == 1024 - 1)
		{
			for (int i = 0; i < 1024; i++)
				buffer2[i] = buffer[i];
		}

		bufferptr2 = (bufferptr2 + 1) % m_Freq;

		float out = 0;
		for (int i = 0; i < 64; i++)
			out += 1.0 / 64.0 * (buffer2[bufferptr2] + buffer[bufferptr]);

		return sin * (1.0 - m_Mix) + m_Mix * out;
	}

	void UpdateParams()
	{
		
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Formant";
		_json["shft"] = m_Knob1.Value();
		return _json;
	}

	void operator=(const json& json)
	{
		double p1 = json["shft"].get<double>();
		m_Knob1.Value(p1);
		UpdateParams();
	}

private:
	static inline std::string
		m_Knob1Name = "Shift";

	KnobSlider
		& m_Knob1, & m_Knob2;
};
