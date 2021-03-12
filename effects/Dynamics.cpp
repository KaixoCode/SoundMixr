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
		m_Slider(Emplace<DynamicsSlider>()),
		EffectBase("Dynamics")
	{
		Height(200);
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

		UpdateParams();

		m_PreGain.Position({ Width() - 282, 20 });
		m_PostGain.Position({ Width() - 227, 20 });
		m_Attack.Position({ Width() - 164, 20 });
		m_Release.Position({ Width() - 109, 20 });
		m_Mix.Position({ Width() - 46, 20 });
		m_Slider.Position({ 12, 93 });
		m_Slider.Size({ (Width() - 24), Height() - 123 });
	}

	void Update()
	{
		UpdateParams();
	}

	void Render(CommandCollection& d) override 
	{
		Effect::Render(d);
		if (m_Small)
			return;

		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Fill>(theme->Get(C::Divider));
		d.Command<Quad>(Vec4<int>{(m_PostGain.X() + m_PostGain.Width() + m_Attack.X()) / 2, 10, 1, 50});
		d.Command<Quad>(Vec4<int>{(m_Release.X() + m_Release.Width() + m_Mix.X()) / 2, 10, 1, 50});
		d.Command<Fill>(theme->Get(C::TextSmall));
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<TextAlign>(Align::LEFT, Align::BOTTOM);
		d.Command<Text>(&m_Slider.m_TH2Str, Vec2<int>{10, 77});
		d.Command<Text>(&m_Slider.m_RT2Str, Vec2<int>{70, 77});
		d.Command<Text>(&m_Slider.m_RT1Str, Vec2<int>{Width() - 110, 77});
		d.Command<TextAlign>(Align::RIGHT, Align::BOTTOM);
		d.Command<Text>(&m_Slider.m_TH1Str, Vec2<int>{Width() - 10, 77});

		int _channels = m_Channels;
		int _x = 12;
		int _rw = Width() - 24;
		int _p = 16;
		int _h = ((Height() - 123 - 22 - _p * 2) / _channels) - (_channels > 4 ? 1 : 2);
		int _y = 94 + _p;

		// Draw all audio meters
		for (int i = 0; i < _channels; i++)
		{
			_y = 94 + _p + i * (_h + (_channels > 4 ? 1 : 2));
			float _level = std::powf(m_Levels[i], 0.25);

			int _w = (std::min(_level, 1.0f) / 1.0f) * (_rw);
			d.Command<Graphics::Fill>(theme->Get(C::VMeterFill));
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		}

		d.Command<PopMatrix>();
	}

	void Channels(int c) override 
	{
		while (m_Levels.size() < c)
			m_Levels.push_back(0);

		while (m_Peaks.size() < c)
			m_Peaks.push_back(0);

		m_Channels = c;
	}

	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

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

	operator json() override
	{
		json _json = json::object();
		_json["enabled"] = m_Enable.Active();
		_json["small"] = m_Minim->Active();
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

	void operator=(const json& json) override
	{
		m_Enable.Active(json.at("enabled").get<bool>());
		m_Minim->Active(json.at("small").get<bool>());
		double p1 = json.at("expt").get<double>();
		double p2 = json.at("comt").get<double>();
		double p3 = json.at("expr").get<double>();
		double p4 = json.at("comr").get<double>();
		double p5 = json.at("att").get<double>();
		double p6 = json.at("ret").get<double>();
		double p7 = json.at("prg").get<double>();
		double p8 = json.at("pog").get<double>();
		double p9 = json.at("mix").get<double>();
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
	
	::Parameter
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

extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Dynamics();
	}
}