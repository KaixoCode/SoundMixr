#include "EffectBase.hpp"
#include "Compressor.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Dynamics : public EffectBase
{
public:
	Dynamics()
		: m_PreGain(Parameter("PreGain", ParameterType::Knob)),
		m_Attack(Parameter("Attack", ParameterType::Knob)),
		m_Release(Parameter("Release", ParameterType::Knob)),
		m_PostGain(Parameter("PostGain", ParameterType::Knob)),
		m_Mix(Parameter("Mix", ParameterType::Knob)),
		m_Slider(EffectBase::Dynamics()),
		EffectBase("Dynamics")
	{
		Height(175);
		m_PreGain.Range({ -24, 24 });
		m_PreGain.ResetValue(0);
		m_PreGain.ResetValue();
		m_PreGain.Unit("dB");
		m_PreGain.Size({ 30, 30 });
		m_PreGain.Multiplier(0.4);
		m_PreGain.Decimals(2);

		m_Attack.Range({ .1f, 30 });
		m_Attack.Power(2);
		m_Attack.Multiplier(0.4);
		m_Attack.ResetValue(3);
		m_Attack.ResetValue();
		m_Attack.Unit(" ms");
		m_Attack.Size({ 30, 30 });

		m_Release.Range({ 1, 300 });
		m_Release.Power(2);
		m_Release.ResetValue(30);
		m_Release.ResetValue();
		m_Release.Multiplier(0.4);
		m_Release.Unit(" ms");
		m_Release.Size({ 30, 30 });

		m_PostGain.Range({ -24, 24 });
		m_PostGain.Unit("dB");
		m_PostGain.ResetValue(0);
		m_PostGain.ResetValue();
		m_PostGain.Size({ 30, 30 });
		m_PostGain.Multiplier(0.4);
		m_PostGain.Decimals(2);

		m_Mix.Range({ 0, 100 });
		m_Mix.Multiplier(0.4);
		m_Mix.ResetValue(100);
		m_Mix.ResetValue();
		m_Mix.Unit(" %");
		m_Mix.Size({ 30, 30 });
		m_Mix.Decimals(0);

		m_Slider.Size({ (Width() - 24), 77 });

		UpdateParams();


		Div().CellType(EffectLayout::Type::ROWS);
		Div().Cells(2);
		Div()[1].CellType(EffectLayout::Type::ROWS);
		Div()[1].Align(EffectLayout::Align::CENTER);
		Div()[1].Cells(2);
		Div()[1][0].CellSize(15);
		Div()[1][1] = m_Slider;
		Div()[0].CellType(EffectLayout::Type::COLS);
		Div()[0].CellSize(70);
		Div()[0].Cells(3);
		Div()[0].Dividers(true);
		Div()[0][0].CellType(EffectLayout::Type::COLS);
		Div()[0][0].CellSize(118);
		Div()[0][0].Cells(2);
		Div()[0][0][0] = m_PreGain;
		Div()[0][0][1] = m_PostGain;
		Div()[0][1].CellType(EffectLayout::Type::COLS);
		Div()[0][1].CellSize(118);
		Div()[0][1].Cells(2);
		Div()[0][1][0] = m_Attack;
		Div()[0][1][1] = m_Release;
		Div()[0][2].CellType(EffectLayout::Type::COLS);
		Div()[0][2].Cells(1);
		Div()[0][2][0] = m_Mix;
	}

	void Update()
	{
		for (int i = 0; i < m_Levels.size(); i++)
			m_Slider.Level(i, m_Levels[i]);

		UpdateParams();
	}

	void Channels(int c) override 
	{
		while (m_Levels.size() < c)
			m_Levels.push_back(0);

		while (m_Peaks.size() < c)
			m_Peaks.push_back(0);

		m_Slider.Channels(c);
	}

	float NextSample(float sin, int c) override
	{
		if (c == 0)
		{
			if (counter > 512)
				counter = 0;

			counter++;
		}

		float out = m_Compressor.NextSample(sin, c);

		float abs = out;
		myabs(abs);
		auto& l = m_Peaks[c];
		if (abs > l)
			l = abs;

		if (counter > 512)
			m_Levels[c] = m_Levels[c] * r + m_Peaks[c] * (1.0 - r),
			m_Peaks[c] = 0;

		return out;
	}

	void UpdateParams()
	{
		m_Compressor.compressRatio = m_Slider.CompressorRatio() >= 0 ? m_Slider.CompressorRatio() / 32.0 + 1 : (-1.0 / (m_Slider.CompressorRatio() - 1.0));
		m_Compressor.expanderRatio = m_Slider.ExpanderRatio() >= 0 ? m_Slider.ExpanderRatio() + 1 : (-1.0 / (m_Slider.ExpanderRatio() / 8.0 - 1.0));
		m_Compressor.compressThreshhold = m_Slider.CompressorThreshhold();
		m_Compressor.expanderThreshhold = m_Slider.ExpanderThreshhold();
		m_Compressor.Attack(m_Attack.Value());
		m_Compressor.Release(m_Release.Value());
		m_Compressor.pregain = db2lin(m_PreGain.Value());
		m_Compressor.postgain = db2lin(m_PostGain.Value());
		m_Compressor.mix = m_Mix.Value() * 0.01;
	}

	operator json() override
	{
		json _json = json::object();
		_json["type"] = "Dynamics";
		_json["expt"] = m_Slider.ExpanderThreshhold();
		_json["comt"] = m_Slider.CompressorThreshhold();
		_json["expr"] = m_Slider.ExpanderRatio();
		_json["comr"] = m_Slider.CompressorRatio();
		_json["att"] = m_Attack.Value();
		_json["ret"] = m_Release.Value();
		_json["prg"] = m_PreGain.Value();
		_json["pog"] = m_PostGain.Value();
		_json["mix"] = m_Mix.Value();
		return _json;
	}

	void operator=(const json& json) override
	{
		double p1 = json.at("expt").get<double>();
		double p2 = json.at("comt").get<double>();
		double p3 = json.at("expr").get<double>();
		double p4 = json.at("comr").get<double>();
		double p5 = json.at("att").get<double>();
		double p6 = json.at("ret").get<double>();
		double p7 = json.at("prg").get<double>();
		double p8 = json.at("pog").get<double>();
		double p9 = json.at("mix").get<double>();
		m_Slider.ExpanderThreshhold(p1);
		m_Slider.CompressorThreshhold(p2);
		m_Slider.ExpanderRatio(p3);
		m_Slider.CompressorRatio(p4);
		m_Attack.Value(p5);
		m_Release.Value(p6);
		m_PreGain.Value(p7);
		m_PostGain.Value(p8);
		m_Mix.Value(p9);
		UpdateParams();
	}

private:
	static inline const double DC_OFFSET = 1.0E-25;
	
	::Parameter
		&m_PreGain,
		&m_Attack,
		&m_Release,
		&m_PostGain,
		&m_Mix;

	DynamicsObject& m_Slider;

	std::vector<float> m_Levels;
	std::vector<float> m_Peaks;

	int counter = 0;
	double r = 0.9;

	Compressor m_Compressor;
};

extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Dynamics();
	}
}