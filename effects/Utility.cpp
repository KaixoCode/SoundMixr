#include "EffectBase.hpp"
#include "Compressor.hpp"
#include "Filters.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

namespace Effects
{

	class Utility : public EffectBase
	{
	public:

		Utility()
			:
			m_Low(Parameter("Low", ParameterType::Knob)),
			m_Mid(Parameter("Mid", ParameterType::Knob)),
			m_High(Parameter("High", ParameterType::Knob)),
			m_Release(Parameter("Release", ParameterType::Knob)),
			m_LowFreq(Parameter("Low Freq", ParameterType::Slider)),
			m_HighFreq(Parameter("High Freq", ParameterType::Slider)),
			m_Pan(Parameter("Pan", ParameterType::Knob)),
			m_Gain(Parameter("Gain", ParameterType::Knob)),
			m_Limiter(VolumeSlider()),
			m_Mono(Toggle("Mono")),
			m_Mute(Toggle("Mute")),
			m_PhaseInvert(Toggle("Phase")),
			m_EnableEq(Toggle("EQ")),
			EffectBase("Utility")
		{
			Height(145);

			m_Limiter.Name("Limit Level");

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
			m_Pan.Range({ -50, 50 });
			m_Pan.Power(1);
			m_Pan.Decimals(-1);
			m_Pan.ResetValue(0);
			m_Pan.ResetValue();
			m_Pan.Unit("L", -1);
			m_Pan.Unit("C", INT_MAX);
			m_Pan.Unit("R", 0);
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
			m_Limiter.Channels(1);
			m_Limiter.ResetValue(1);
			m_Limiter.ResetValue();

			Div().Divs(3);
			Div().Dividers(true);
			Div()[0].Align(Div::Alignment::Vertical);
			Div()[0].DivSize(132);
			Div()[0].Padding(4);
			Div()[0].Divs(3);
			Div()[0][2] = m_EnableEq;
			Div()[0][2].DivSize(34);
			Div()[0][1].Align(Div::Alignment::Horizontal);
			Div()[0][1].Divs(3);
			Div()[0][1][0] = m_Low;
			Div()[0][1][1] = m_Mid;
			Div()[0][1][2] = m_High;
			Div()[0][0].DivSize(34);
			Div()[0][0].Align(Div::Alignment::Horizontal);
			Div()[0][0].Divs(2);
			Div()[0][0][0] = m_LowFreq;
			Div()[0][0][1] = m_HighFreq;
			Div()[1].Align(Div::Alignment::Horizontal);
			Div()[1].DivSize(110);
			Div()[1].Divs(2);
			Div()[1][0].Align(Div::Alignment::Vertical);
			Div()[1][0].Divs(2);
			Div()[1][0].DivSize(60);
			Div()[1][0][1] = m_Gain;
			Div()[1][0][0] = m_Release;
			Div()[1][1].Align(Div::Alignment::Left);
			Div()[1][1] = m_Limiter;
			Div()[2].Align(Div::Alignment::Vertical);
			Div()[2].Divs(4);
			Div()[2][0].Align(Div::Alignment::Top);
			Div()[2][0].DivSize(26);
			Div()[2][0] = m_Mute;
			Div()[2][1].Align(Div::Alignment::Top);
			Div()[2][1] = m_Mono;
			Div()[2][2].Align(Div::Alignment::Top);
			Div()[2][2] = m_PhaseInvert;
			Div()[2][3].DivSize(72);
			Div()[2][3] = m_Pan;
			UpdateParams();
		}

		void Update() override
		{
			if (!m_EnableEq.State())
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

			m_Limiter.SetValue(0, m_Level1);
			m_Limiter.SetReduce(0, m_Level2);
			UpdateParams();
		}

		void Channels(int c) override
		{
			m_Channels = c;
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
				while (a.size() < PRE_BUFFER_SIZE)
					a.push_back(0);
			}
		}

		float NextSample(float sin, int c) override
		{
			if (m_Mute.State())
				return 0;

			float filter = m_EnableEq.State() ? m_Equalizers[c].Apply(sin) : sin;

			if (c == 0)
			{
				if (counter > 512)
					counter = 0;

				counter++;
			}

			// Predelay
			auto& _pd = m_PreDelay[c];
			float predelay = _pd[(m_PreDC + 144) % PRE_BUFFER_SIZE] = filter * m_PreGain;
			m_Compressor.NextSample(predelay, c);

			float prelimit = _pd[(m_PreDC) % PRE_BUFFER_SIZE];
			float limit = prelimit * m_Compressor.compressMult;
			if (limit > m_LimiterValue)
				limit = m_LimiterValue;

			if (-limit > m_LimiterValue)
				limit = -m_LimiterValue;

			m_PreDC = (m_PreDC + 1) % PRE_BUFFER_SIZE;
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

			return (m_PhaseInvert.State() ? -1 : 1) * (m_Mono.State() ? m_MonoValue : limit) * m_Pans[c];
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
			m_Compressor.compressThreshhold = lin2db(std::max(m_Limiter.Value(), 0.000001));
			m_Compressor.compressRatio = 0;
			m_Compressor.mix = 1;
			m_PreGain = db2lin(m_Gain.Value());

			m_LimiterValue = m_Limiter.Value();

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

		void operator=(const nlohmann::json& json) override
		{
			EffectBase::operator=(json);
			UpdateParams();
		}

	private:
		static inline constexpr int PRE_BUFFER_SIZE = 256;
		static inline std::unordered_map<int, std::string> m_Numbers;
		static inline std::string m_NegInf = "Inf";

		int counter = 0;
		double r = 0.9;
		int m_Channels = 0;

		Effects::Parameter& m_Low, & m_Mid, & m_High, & m_Gain, & m_Release;
		Effects::Parameter& m_LowFreq, & m_HighFreq;
		Effects::Parameter& m_Pan;
		Effects::VolumeSlider& m_Limiter;
		Effects::ToggleButton& m_Mono, & m_Mute, & m_EnableEq, & m_PhaseInvert;

		std::vector<float> m_Pans;
		float m_Level1 = 0;
		float m_Level2 = 0;
		float m_Peak1 = 0;
		float m_Peak2 = 0;
		float m_PreGain = 1;

		float m_LimiterValue = 0;
		float m_MonoValue = 0;
		float m_MonoTemp = 0;

		Compressor m_Compressor;

		int m_PreDC = 0;

		BiquadParameters m_Params[3];
		std::vector<std::vector<float>> m_PreDelay;
		std::vector<ChannelEqualizer<3, BiquadFilter<>>> m_Equalizers;
	};
}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new Effects::Utility;
}