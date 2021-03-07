#pragma once
#include "audio/Effects.hpp"
#include "ui/Dropdown.hpp"
#include "audio/Equalizer.hpp"
#include "audio/Dynamics.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Utility : public Effect
{
public:
	Utility()
		: 
		m_Low(Emplace<KnobSlider>("Low")),
		m_Mid(Emplace<KnobSlider>("Mid")),
		m_High(Emplace<KnobSlider>("High")),
		m_Release(Emplace<KnobSlider>("Release")),
		m_LowFreq(Emplace<NormalSlider>("Freq")),
		m_HighFreq(Emplace<NormalSlider>("Freq")),
		m_Pan(Emplace<PanKnob>("Pan")),
		m_Gain(Emplace<KnobSlider>("Gain")),
		m_Limiter(Emplace<VolumeSlider>()),
		m_Mono(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "Mono")),
		m_Mute(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "Mute")),
		m_PhaseInvert(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "Phase")),
		m_EnableEq(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "EQ")),
		Effect("Utility")
	{
		Height(170);

		m_Release.Size({ 30, 30 });
		m_Release.Range({ 1, 5000 });
		m_Release.Power(3);
		m_Release.ResetValue(300);
		m_Release.ResetValue();
		m_Release.Multiplier(0.4);
		m_Release.Unit(" ms");
		m_Release.Unit(" s", 3);

		m_Low.Size({ 30, 30 });
		m_Low.Range({ -12, 12 });
		m_Low.Power(1);
		m_Low.ResetValue(0);
		m_Low.ResetValue();
		m_Low.Unit("dB");
		m_Low.Multiplier(0.4);

		m_Mid.Size({ 30, 30 });
		m_Mid.Range({ -12, 12 });
		m_Mid.Power(1);
		m_Mid.ResetValue(0);
		m_Mid.ResetValue();
		m_Mid.Unit("dB");
		m_Mid.Multiplier(0.4);

		m_High.Size({ 30, 30 });
		m_High.Range({ -12, 12 });
		m_High.Power(1);
		m_High.ResetValue(0);
		m_High.ResetValue();
		m_High.Unit("dB");
		m_High.Multiplier(0.4);

		m_Pan.Size({ 30, 30 });
		m_Pan.Vertical(true);
		m_Pan.Multiplier(0.4);

		m_Gain.Size({ 30, 30 });
		m_Gain.Range({ -24, 24 });
		m_Gain.Power(1);
		m_Gain.ResetValue(0);
		m_Gain.ResetValue();
		m_Gain.Unit("dB");
		m_Gain.Multiplier(0.4);

		m_Mute.Size({ 45, 18 });
		m_Mono.Size({ 45, 18 });
		m_EnableEq.Size({ 70, 18 });
		m_PhaseInvert.Size({ 45, 18 });

		m_LowFreq.Size({ 58, 18 });
		m_LowFreq.Range({ 10, 6000 });
		m_LowFreq.Power(3);
		m_LowFreq.ResetValue(600);
		m_LowFreq.ResetValue();
		m_LowFreq.Unit("Hz");
		m_LowFreq.Unit("kHz", 3);
		m_LowFreq.Decimals(2);
		m_LowFreq.Multiplier(1);
		m_LowFreq.Vertical(false);
		m_LowFreq.DisplayName(false);
		
		m_HighFreq.Size({ 58, 18 });
		m_HighFreq.Range({ 2000, 22000 });
		m_HighFreq.Power(3);
		m_HighFreq.ResetValue(7000);
		m_HighFreq.ResetValue();
		m_HighFreq.Unit("Hz");
		m_HighFreq.Unit("kHz", 3);
		m_HighFreq.Decimals(2);
		m_HighFreq.Multiplier(1);
		m_HighFreq.Vertical(false);
		m_HighFreq.DisplayName(false);

		m_Limiter.Size({ 30, 120 });
	}

	void Update(const Vec4<int>& v) override
	{
		if (!m_EnableEq.Active())
		{
			m_Low.Disable();
			m_LowFreq.Disable();
			m_Mid.Disable();
			m_High.Disable();
			m_HighFreq.Disable();
		}
		else
		{
			m_Low.Enable();
			m_LowFreq.Enable();
			m_Mid.Enable();
			m_High.Enable();
			m_HighFreq.Enable();
		}

		m_EnableEq.Position({ 33, 117 });
		m_Low.Position({ 10, 52 });
		m_LowFreq.Position({ 5, Height() - 160 });
		m_Mid.Position({ 50, 52 });
		m_High.Position({ 90, 52 });
		m_HighFreq.Position({ 67, 10 });

		m_Pan.Position({ 255, 89 });
		m_Mono.Position({ 247, 32 });
		m_Mute.Position({ 247, Height() - 160 });
		m_PhaseInvert.Position({ 247, 54 });

		m_Release.Position({ 148, 21 });
		m_Gain.Position({ 148, 89 });
		m_Limiter.Position({ 188, 6 });
		Background(Color{ 0, 0, 0, 0 });
		UpdateParams();
		Effect::Update(v);
	}

	void Render(CommandCollection& d) override
	{

		if (!m_Small)
		{
			d.Command<Graphics::PushMatrix>();
			d.Command<Graphics::Translate>(Position());
			d.Command<Graphics::Fill>(Theme<C::Channel>::Get());
			d.Command<Graphics::Quad>(0, 0, Width(), Height());
			d.Command<Graphics::Fill>(Theme<C::VMeter>::Get());

			int _x = m_Limiter.X() + 9;
			int _y = m_Limiter.Y() + 5;
			int _w = m_Limiter.Width() - 18;
			int _rh = m_Limiter.Height();
			int _0db = ((std::powf(m_Limiter.Value(), 0.25) / 1.412536) * (_rh)) + _y;

			d.Command<Graphics::Quad>(Vec4<int>{ _x, _y, _w, _rh });
		
			float _level1 = std::powf(m_Level1, 0.25);
			float _level2 = std::powf(m_Level2, 0.25);

			int _h1 = (std::min(_level1, 1.412536f) / 1.412536) * (_rh);
			int _h2 = (std::min(_level2, 1.412536f) / 1.412536) * (_rh);
			

			d.Command<Graphics::Fill>(Theme<C::VMeterFill>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{ _x, _y, _w, _h1 });
			d.Command<Graphics::Fill>(Theme<C::VMeterFillC1>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{ _x, _y + _h1, _w, _h2 - _h1 });

			// db numbers besides volume meter
			int _d = 6;
			bool _b = true;
			d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
			d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
			for (int i = 12; i > -120; i -= _d)
			{
				if (i < -11)
					_d = 12;
				if (i < -35)
					_d = 36;
				if (i < -86)
					break;

				if (_b)
					d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
				else
					d.Command<Graphics::Fill>(Theme<C::VMeterIndD>::Get());

				int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
				d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
				if (_b)
				{
					if (m_Numbers.find(i) == m_Numbers.end())
					{
						m_Numbers.emplace(i, std::to_string(std::abs(i)));
					}
					d.Command<Graphics::Fill>(Theme<C::TextOff>::Get());
					d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
				}
				_b ^= true;
			}
			d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});
			d.Command<Graphics::Fill>(Theme<C::TextOff>::Get());
			d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y + 5});

			d.Command<Graphics::PopMatrix>();
		}
		
		Effect::Render(d);
		if (m_Small)
			return;

		d.Command<Graphics::PushMatrix>();
		d.Command<Graphics::Translate>(Position());
		d.Command<Graphics::Fill>(Theme<C::Divider>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{(m_HighFreq.X() + m_HighFreq.Width()) + 7, 10, 1, Height() - 45});
		d.Command<Graphics::Quad>(Vec4<int>{m_Mono.X() - 7, 10, 1, Height() - 45});
				
		d.Command<Graphics::PopMatrix>();
	}

	void Channels(int c) override
	{
		m_Pans.reserve(c);
		while (m_Pans.size() < c)
			m_Pans.emplace_back(0);

		m_Equalizers.reserve(c);
		while (m_Equalizers.size() < c)
			m_Equalizers.emplace_back(m_Params);

		m_PreDelay.reserve(c);
		while (m_PreDelay.size() < c)
		{
			auto& a = m_PreDelay.emplace_back();
			while (a.size() < 1024)
				a.push_back(0);
		}
		Effect::Channels(c);
	}

	int m_Freq = 128;
	double m_Mix = 0.5;
	float NextSample(float sin, int c) override
	{	
		if (!m_Enabled)
			return sin;
		
		if (m_Mute.Active())
			return 0;

		float filter = m_EnableEq.Active() ? m_Equalizers[c].Apply(sin) : sin;
		
		if (c == 0)
		{
			if (counter > 512)
				counter = 0;

			counter++;
		}

		// Predelay
		float predelay = m_PreDelay[c][(m_PreDC + 512) % 1024] = filter * m_PreGain;
		m_Compressor.NextSample(predelay, c);

		float prelimit = m_PreDelay[c][(m_PreDC) % 1024];
		float limit = prelimit * m_Compressor.compressMult;
		if (limit > m_Limiter.Value())
			limit = m_Limiter.Value();

		if (-limit > m_Limiter.Value())
			limit = -m_Limiter.Value();

		m_PreDC = (m_PreDC + 1) % 1024;
		float sina = prelimit;
		float sinb = limit;
		myabs(sina);
		myabs(sinb);
		if (sina > m_Peak1)
			m_Peak1 = sina;

		if (sinb > m_Peak2)
			m_Peak2 = sinb;

		if (counter > 512)
			m_Level1 = m_Level1 * r + m_Peak1 * (1.0 - r),
			m_Level2 = m_Level2 * r + m_Peak2 * (1.0 - r),
			m_Peak1 = 0,
			m_Peak2 = 0;

		m_MonoTemp += limit;
		if (c == 0)
		{
			m_MonoValue = m_MonoTemp / (double)m_Channels;
			m_MonoTemp = 0;
		}

		return (m_PhaseInvert.Active() ? -1 : 1) * (m_Mono.Active() ? m_MonoValue : limit) * m_Pans[c];
	}

	void UpdateParams()
	{
		m_Params[0].f0 = m_LowFreq.Value();
		m_Params[0].Q = 1.3;
		m_Params[0].dbgain = m_Low.Value();
		m_Params[0].type = FilterType::LowShelf;
		m_Params[0].RecalculateParameters();

		m_Params[1].f0 = 2000;
		m_Params[1].Q = 3;
		m_Params[1].dbgain = m_Mid.Value();
		m_Params[1].type = FilterType::PeakingEQ;
		m_Params[1].RecalculateParameters();

		m_Params[2].f0 = m_HighFreq.Value();
		m_Params[2].Q = 1.3;
		m_Params[2].dbgain = m_High.Value();
		m_Params[2].type = FilterType::HighShelf;
		m_Params[2].RecalculateParameters();

		m_Compressor.Attack(0.1);
		m_Compressor.Release(m_Release.Value());
		m_Compressor.postgain = 1;
		m_Compressor.pregain = 1;
		m_Compressor.expanderRatio = 1;
		m_Compressor.expanderThreshhold = -100;
		m_Compressor.compressThreshhold = m_Limiter.Decibels();
		m_Compressor.compressRatio = 0;
		m_Compressor.mix = 1;
		m_PreGain = db2lin(m_Gain.Value());

		int _panv = m_Pan.Value();
		int _index = 0;
		double _p = _panv / 50.0;
		double _a = 1.0 - std::abs((m_Channels - 1.0) / 2.0 * _p);
		for (int i = 0; i < m_Channels; i++)
		{
			float _pan = constrain(_p * (_index - (m_Channels - 1.0) / 2.0) + _a, 0.0, 1.0);
			m_Pans[i] = _pan;
			_index++;
		}
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Utility";
		_json["enabled"] = m_Enable.Active();
		_json["small"] = m_Minim->Active();
		_json["eqlow"] = m_Low.Value();
		_json["eqmid"] = m_Mid.Value();
		_json["eqhigh"] = m_High.Value();
		_json["eqon"] = m_EnableEq.Active();
		_json["eqlowf"] = m_LowFreq.Value();
		_json["eqhighf"] = m_HighFreq.Value();
		_json["limgain"] = m_Gain.Value();
		_json["limrel"] = m_Release.Value();
		_json["limth"] = m_Limiter.Value();
		_json["pan"] = m_Pan.Value();
		_json["phase"] = m_PhaseInvert.Active();
		_json["mono"] = m_Mono.Active();
		_json["mute"] = m_Mute.Active();
		return _json;
	}

	void operator=(const json& json)
	{
		m_Enable.Active(json.at("enabled").get<bool>());
		m_Minim->Active(json.at("small").get<bool>());
		m_Low.Value(json.at("eqlow").get<double>());
		m_Mid.Value(json.at("eqmid").get<double>());
		m_High.Value(json.at("eqhigh").get<double>());
		m_EnableEq.Active(json.at("eqon").get<bool>());
		m_LowFreq.Value(json.at("eqlowf").get<double>());
		m_HighFreq.Value(json.at("eqhighf").get<double>());
		m_Gain.Value(json.at("limgain").get<double>());
		m_Release.Value(json.at("limrel").get<double>());
		m_Limiter.Value(json.at("limth").get<double>());
		m_Pan.Value(json.at("pan").get<double>());
		m_PhaseInvert.Active(json.at("phase").get<bool>());
		m_Mono.Active(json.at("mono").get<bool>());
		m_Mute.Active(json.at("mute").get<bool>());
		UpdateParams();
	}

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	int counter = 0;
	double r = 0.9;

	KnobSlider& m_Low, & m_Mid, & m_High, & m_Gain, & m_Release;
	NormalSlider& m_LowFreq, & m_HighFreq;
	PanKnob & m_Pan;
	VolumeSlider& m_Limiter;
	Button<ToggleButton, ButtonType::Toggle>& m_Mono, & m_Mute, & m_EnableEq, & m_PhaseInvert;

	std::vector<float> m_Pans;
	float m_Level1 = 0;
	float m_Level2 = 0;
	float m_Peak1 = 0;
	float m_Peak2 = 0;
	float m_PreGain = 1;

	float m_MonoValue = 0;
	float m_MonoTemp = 0;

	Compressor m_Compressor;

	int m_PreDC = 0;

	BiquadParameters m_Params[3];
	std::vector<std::vector<float>> m_PreDelay;
	std::vector<ChannelEqualizer<3, BiquadFilter<>>> m_Equalizers;
};
