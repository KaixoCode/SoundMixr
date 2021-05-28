#include "Base.hpp"
#include "Oscillator.hpp"
#include "Filters.hpp"

namespace SoundMixr
{
	class Flanger : public EffectBase
	{
	public:
		Flanger()
			: m_DelayKnob(Parameter("Time", ParameterType::Slider)),
			m_FeedbackKnob(Parameter("Feedback", ParameterType::Slider)),
			m_MixKnob(Parameter("Mix", ParameterType::Knob)),
			m_Highpass(Parameter("Hi Pass", ParameterType::Knob)),
			m_AmountKnob(Parameter("LFO", ParameterType::Knob)),
			m_RateKnob(Parameter("Rate", ParameterType::Knob)),
			m_PhaseKnob(Parameter("Phase", ParameterType::Knob)),
			m_PolarityP(RadioButton("+", 1, [&] { m_Polarity = 1; })),
			m_PolarityM(RadioButton("-", 1, [&] { m_Polarity = -1; })),
			m_Controller(XYController(m_DelayKnob, m_FeedbackKnob)),
			m_Wavetable(DropDown("Shape")),
			EffectBase("Flanger")
		{
			m_Parameters.emplace_back();

			Height(145);
			m_AmountKnob.Range({ 0, 100 });
			m_AmountKnob.ResetValue(100);
			m_AmountKnob.ResetValue();
			m_AmountKnob.Unit(" %");
			m_AmountKnob.Size({ 30, 30 });
			m_AmountKnob.Decimals(1);
			m_AmountKnob.Multiplier(0.4);

			m_RateKnob.Range({ 0.01, 5 });
			m_RateKnob.Log(2);
			m_RateKnob.ResetValue(0.1);
			m_RateKnob.ResetValue();
			m_RateKnob.Unit(" Hz");
			m_RateKnob.Size({ 30, 30 });
			m_RateKnob.Decimals(2);
			m_RateKnob.Multiplier(0.4);

			m_PhaseKnob.Range({ 0, 360 });
			m_PhaseKnob.ResetValue(180);
			m_PhaseKnob.ResetValue();
			m_PhaseKnob.Size({ 30, 30 });
			m_PhaseKnob.Decimals(0);
			m_PhaseKnob.Multiplier(0.4);

			m_Highpass.Range({ 10, 22000 });
			m_Highpass.Log(10);
			m_Highpass.ResetValue(100);
			m_Highpass.ResetValue();
			m_Highpass.Unit(" Hz");
			m_Highpass.Unit(" kHz", 3);
			m_Highpass.Size({ 30, 30 });
			m_Highpass.Decimals(2);
			m_Highpass.Multiplier(0.4);

			m_DelayKnob.Range({ 0.1, 50 });
			m_DelayKnob.Log(2);
			m_DelayKnob.ResetValue(0.6);
			m_DelayKnob.ResetValue();
			m_DelayKnob.Unit(" ms");
			m_DelayKnob.Size({ 55, 18 });
			m_DelayKnob.Decimals(2);
			m_DelayKnob.Multiplier(1);
			m_DelayKnob.Vertical(false);
			m_DelayKnob.DisplayName(false);

			m_MixKnob.Range({ 0, 100 });
			m_MixKnob.Power(1);
			m_MixKnob.ResetValue(50);
			m_MixKnob.ResetValue();
			m_MixKnob.Unit(" %");
			m_MixKnob.Size({ 30, 30 });
			m_MixKnob.Decimals(1);
			m_MixKnob.Multiplier(0.4);

			m_FeedbackKnob.Range({ 0, 99 });
			m_FeedbackKnob.Power(1);
			m_FeedbackKnob.ResetValue(10);
			m_FeedbackKnob.ResetValue();
			m_FeedbackKnob.Unit(" %");
			m_FeedbackKnob.Size({ 42, 18 });
			m_FeedbackKnob.Decimals(1);
			m_FeedbackKnob.Multiplier(1);
			m_FeedbackKnob.Vertical(false);
			m_FeedbackKnob.DisplayName(false);

			m_PolarityP.Selected(true);
			m_PolarityP.Size({ 21, 18 });
			m_PolarityM.Size({ 21, 18 });
			m_Polarity = 1;

			m_Wavetable.Size({ 48, 18 });
			m_Wavetable.AddOption("Sine", 1);
			m_Wavetable.AddOption("Square", 2);
			m_Wavetable.AddOption("Triangle", 3);
			m_Wavetable.AddOption("Saw", 4);
			m_Wavetable.Select(1);

			m_Controller.Size({ 103, 103 });

			Div().Align(Div::Alignment::Horizontal);
			Div().Divs(4);
			Div().Dividers(true);
			Div()[0].Align(Div::Alignment::Vertical);
			Div()[0].DivSize(60);
			Div()[0].Divs(2);
			Div()[0][1] = m_Highpass;
			Div()[0][0] = m_MixKnob;
			Div()[1].Align(Div::Alignment::Vertical);
			Div()[1].DivSize(118);
			Div()[1].Padding(4);
			Div()[1].Divs(3);
			Div()[1][0].DivSize(4);
			Div()[1][1].Align(Div::Alignment::Horizontal);
			Div()[1][1].DivSize(26);
			Div()[1][1].Divs(2);
			Div()[1][1][0].DivSize(62);
			Div()[1][1][0] = m_DelayKnob;
			Div()[1][1][1] = m_FeedbackKnob;
			Div()[1][2].Align(SoundMixr::Div::Alignment::Bottom);
			Div()[1][2] = m_Controller;
			Div()[2].Align(Div::Alignment::Vertical);
			Div()[2].DivSize(60);
			Div()[2].Divs(2);
			Div()[2][1] = m_AmountKnob;
			Div()[2][0] = m_RateKnob;
			Div()[3].Align(Div::Alignment::Vertical);
			Div()[3].Divs(2);
			Div()[3].Padding(4);
			Div()[3][1].Align(Div::Alignment::Horizontal);
			Div()[3][1].DivSize(33);
			Div()[3][1].Divs(2);
			Div()[3][1][0] = m_PolarityP;
			Div()[3][1][1] = m_PolarityM;
			Div()[3][0].Align(Div::Alignment::Vertical);
			Div()[3][0].Divs(3);
			Div()[3][0][0].DivSize(4);
			Div()[3][0][1] = m_PhaseKnob;
			Div()[3][0][2] = m_Wavetable;

			m_Oscillator.wavetable = Wavetables::Sine;
		}

		int prev = 0;
		void Update() override
		{
			if (prev != m_Wavetable.Selected())
			{
				prev = m_Wavetable.Selected();
				if (prev == 1)
					m_Oscillator.wavetable = Wavetables::Sine;
				else if (prev == 2)
					m_Oscillator.wavetable = Wavetables::Square;
				else if (prev == 3)
					m_Oscillator.wavetable = Wavetables::Triangle;
				else if (prev == 4)
					m_Oscillator.wavetable = Wavetables::Saw;
			}
			UpdateParams();
		}

		void Channels(int c) override
		{
			m_Equalizers.reserve(c);
			while (m_Equalizers.size() < c)
				m_Equalizers.emplace_back(m_Parameters);

			m_Buffers.reserve(c);
			while (m_Buffers.size() < c)
			{
				auto& a = m_Buffers.emplace_back();
				while (a.size() < BUFFER_SIZE)
					a.emplace_back(0);
			}
		}

		float NextSample(float sin, int c) override
		{
			if (c == 0)
			{
				m_Position = (m_Position + 1) % BUFFER_SIZE;
				m_Oscillator.NextSample();
			}

			int delayt = ((m_Delay + m_Delay * m_Oscillator.Sample(c * m_Phase) * m_Amount * 0.01 * 0.9) / 1000.0) * m_SampleRate;
			delayt = (std::max(delayt, 1)) % BUFFER_SIZE;

			auto& _buffer = m_Buffers[c];

			int i1 = (m_Position - delayt + BUFFER_SIZE) % BUFFER_SIZE;

			float del1s = _buffer[i1];

			float now = del1s;

			_buffer[m_Position] = sin + m_Polarity * now * m_Feedback;

			return sin * (1.0 - m_Mix) + m_Equalizers[c].Apply(now * m_Mix);
		}

		void UpdateParams()
		{
			m_Delay = m_DelayKnob.Value();
			m_Amount = m_AmountKnob.Value();
			m_Oscillator.frequency = m_RateKnob.Value();
			m_Feedback = m_FeedbackKnob.Value() * 0.01;
			m_Mix = m_MixKnob.Value() * 0.01;
			m_Phase = m_PhaseKnob.NormalizedValue();
			m_Parameters[0].type = FilterType::HighPass;
			m_Parameters[0].Q = 1;
			m_Parameters[0].f0 = m_Highpass.Value();
			m_Parameters[0].dbgain = 0;
			m_Parameters[0].RecalculateParameters();
		}

	private:
		static inline constexpr int BUFFER_SIZE = 4096;
		std::vector<std::vector<float>> m_Buffers;
		int m_Position = 0;
		double m_Delay = 5;
		double m_Amount = 3;
		double m_Feedback = 0.3;
		double m_Phase = 0.3;
		double m_Mix = 0.5;
		Oscillator m_Oscillator;

		std::vector<BiquadParameters> m_Parameters;
		std::vector<ChannelEqualizer<1, BiquadFilter<>>> m_Equalizers;

		int m_Polarity = 1;
		SoundMixr::Parameter& m_DelayKnob, & m_FeedbackKnob, & m_MixKnob, & m_Highpass;
		SoundMixr::Parameter& m_AmountKnob, & m_RateKnob, & m_PhaseKnob;
		SoundMixr::DropDown& m_Wavetable;
		SoundMixr::RadioButton& m_PolarityP, & m_PolarityM;
		SoundMixr::XYController& m_Controller;
	};
}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::Flanger;
}