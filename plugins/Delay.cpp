#define EFFECT_PLUGIN
#include "Base.hpp"
#include "Oscillator.hpp"
#include "Filters.hpp"
#include <Compressor.hpp>

namespace SoundMixr
{
	class Delay : public EffectBase
	{
	public:
		Delay()
			:
			m_DelayKnob(Parameter("Delay", ParameterType::Knob)),
			m_MixKnob(Parameter("Mix", ParameterType::Knob)),
			m_Freq(Parameter("Filter", ParameterType::Slider)),
			m_Width(Parameter("Width", ParameterType::Slider)),
			m_ModAmountKnob(Parameter("Mod", ParameterType::Slider)),
			m_ModRateKnob(Parameter("Rate", ParameterType::Slider)),
			m_FeedbackKnob(Parameter("Feedback", ParameterType::Knob)),
			m_InputGainKnob(Parameter("Input", ParameterType::Knob)),
			m_Normal(RadioButton("Normal", 2, [&] { m_Mode = 0; })),
			m_PingPong(RadioButton("Stereo", 2, [&] { m_Mode = 1; })),
			m_Curve(SimpleFilterCurve(m_Parameters, m_Width, m_Freq)),
			m_EnableFilter(Toggle("")),
			EffectBase("Delay")
		{

			Height(155);
			m_ModAmountKnob.Range({ 0, 100 });
			m_ModAmountKnob.Scaling(2);
			m_ModAmountKnob.ResetValue(0.3);
			m_ModAmountKnob.ResetValue();
			m_ModAmountKnob.Unit(" %");
			m_ModAmountKnob.Size({ 47, 18 });
			m_ModAmountKnob.Decimals(1);
			m_ModAmountKnob.Multiplier(1);
			m_ModAmountKnob.Vertical(false);

			m_ModRateKnob.Range({ 0.1, 15 });
			m_ModRateKnob.ScalingType(ParameterData::Scaling::Log);
			m_ModRateKnob.Scaling(10);
			m_ModRateKnob.ResetValue(3);
			m_ModRateKnob.ResetValue();
			m_ModRateKnob.Unit(" Hz");
			m_ModRateKnob.Size({ 47, 18 });
			m_ModRateKnob.Decimals(2);
			m_ModRateKnob.Multiplier(1);
			m_ModRateKnob.Vertical(false);
			m_ModRateKnob.DisplayName(false);

			m_Freq.Range({ 10, 22000 });
			m_Freq.ScalingType(ParameterData::Scaling::Log);
			m_Freq.Scaling(10);
			m_Freq.ResetValue(1000);
			m_Freq.ResetValue();
			m_Freq.Unit(" Hz");
			m_Freq.Unit(" kHz", 3);
			m_Freq.Size({ 51, 18 });
			m_Freq.Decimals(1);
			m_Freq.Multiplier(1);
			m_Freq.Vertical(false);
			m_Freq.DisplayName(true);

			m_Width.Range({ 0, 1 });
			m_Width.ResetValue(0.3);
			m_Width.ResetValue();
			m_Width.Size({ 29, 18 });
			m_Width.Decimals(2);
			m_Width.Multiplier(1);
			m_Width.Vertical(false);
			m_Width.DisplayName(false);

			m_DelayKnob.Range({ 1, 2000 });
			m_DelayKnob.ScalingType(ParameterData::Scaling::Log);
			m_DelayKnob.Scaling(10);
			m_DelayKnob.ResetValue(300);
			m_DelayKnob.ResetValue();
			m_DelayKnob.Unit(" ms");
			m_DelayKnob.Unit(" s", 3);
			m_DelayKnob.Size({ 30, 30 });
			m_DelayKnob.Decimals(1);
			m_DelayKnob.Multiplier(0.4);

			m_InputGainKnob.Range({ -24, 24 });
			m_InputGainKnob.ResetValue(0);
			m_InputGainKnob.ResetValue();
			m_InputGainKnob.Unit(" dB");
			m_InputGainKnob.Size({ 30, 30 });
			m_InputGainKnob.Decimals(2);
			m_InputGainKnob.Multiplier(0.4);

			m_MixKnob.Range({ 0, 100 });
			m_MixKnob.ResetValue(50);
			m_MixKnob.ResetValue();
			m_MixKnob.Unit(" %");
			m_MixKnob.Size({ 30, 30 });
			m_MixKnob.Decimals(1);
			m_MixKnob.Multiplier(0.4);

			m_FeedbackKnob.Range({ 0, 99 });
			m_FeedbackKnob.ResetValue(10);
			m_FeedbackKnob.ResetValue();
			m_FeedbackKnob.Unit(" %");
			m_FeedbackKnob.Size({ 30, 30 });
			m_FeedbackKnob.Decimals(1);
			m_FeedbackKnob.Multiplier(0.4);

			m_PingPong.Size({ 41, 18 });
			m_Normal.Size({ 41, 18 });
			m_Normal.Selected(true);
			m_Mode = 0;

			m_EnableFilter.Size({ 18, 18 });
			m_EnableFilter.State(true);

			m_Curve.Size({ 169, 77 });

			Div().Align(Div::Alignment::Horizontal);
			Div().Divs(3);
			Div().Dividers(true);
			Div()[0].Align(Div::Alignment::Vertical);
			Div()[0].Divs(2);
			Div()[0].DivSize(58);
			Div()[0][0] = m_InputGainKnob;
			Div()[0][1] = m_DelayKnob;
			Div()[1].Align(Div::Alignment::Vertical);
			Div()[1].Divs(3);
			Div()[1].DivSize(184);
			Div()[1][2].Align(Div::Alignment::Horizontal);
			Div()[1][2].Divs(3);
			Div()[1][2].Dividers(true);
			Div()[1][2].DivSize(71);
			Div()[1][2][0].Align(Div::Alignment::Horizontal);
			Div()[1][2][0].Divs(2);
			Div()[1][2][0].DivSize(66);
			Div()[1][2][0][0].DivSize(6);
			Div()[1][2][0][1].Align(Div::Alignment::Vertical);
			Div()[1][2][0][1].Divs(4);
			Div()[1][2][0][1].DivSize(58);
			Div()[1][2][0][1][0].DivSize(6);
			Div()[1][2][0][1][1].Align(Div::Alignment::Horizontal);
			Div()[1][2][0][1][1].DivSize(22);
			Div()[1][2][0][1][1].Divs(3);
			Div()[1][2][0][1][1][0].DivSize(2);
			Div()[1][2][0][1][1][1].DivSize(22);
			Div()[1][2][0][1][1][1].Align(Div::Alignment::Left);
			Div()[1][2][0][1][1][1] = m_EnableFilter;
			Div()[1][2][0][1][1][2].Align(Div::Alignment::Left);
			Div()[1][2][0][1][1][2] = m_Width;
			Div()[1][2][0][1][2].Align(Div::Alignment::Horizontal);
			Div()[1][2][0][1][2].DivSize(22);
			Div()[1][2][0][1][2].Divs(3);
			Div()[1][2][0][1][2][0].DivSize(0);
			Div()[1][2][0][1][2][1] = m_Freq;
			Div()[1][2][0][1][2][2].DivSize(4);
			Div()[1][2][0][1][3].DivSize(17);
			Div()[1][2][1].Align(Div::Alignment::Vertical);
			Div()[1][2][1].Divs(4);
			Div()[1][2][1].DivSize(62);
			Div()[1][2][1][0].DivSize(6);
			Div()[1][2][1][1].DivSize(22);
			Div()[1][2][1][1] = m_ModRateKnob;
			Div()[1][2][1][2].DivSize(22);
			Div()[1][2][1][2] = m_ModAmountKnob;
			Div()[1][2][1][3].DivSize(17);
			Div()[1][2][2].Align(Div::Alignment::Vertical);
			Div()[1][2][2].Divs(4);
			Div()[1][2][2].DivSize(49);
			Div()[1][2][2][0].DivSize(6);
			Div()[1][2][2][1].DivSize(22);
			Div()[1][2][2][1].Align(Div::Alignment::Right);
			Div()[1][2][2][1] = m_PingPong;
			Div()[1][2][2][2].DivSize(22);
			Div()[1][2][2][2].Align(Div::Alignment::Right);
			Div()[1][2][2][2] = m_Normal;
			Div()[1][2][2][3].DivSize(17);
			Div()[1][1].DivSize(75);
			Div()[1][1] = m_Curve;
			Div()[1][0].DivSize(9);
			Div()[2].Align(Div::Alignment::Vertical);
			Div()[2].Divs(2);
			Div()[2].DivSize(58);
			Div()[2][0] = m_MixKnob;
			Div()[2][1] = m_FeedbackKnob;

			m_Oscillator.wavetable = [](double p) { return std::sin(p * 3.14159265359 * 2); };
		}

		void Update() override
		{
			UpdateParams();
		}

		void Channels(int c) override
		{
			BUFFER_SIZE = m_SampleRate * 10;
			m_Equalizers.reserve(c);
			while (m_Equalizers.size() < c)
				m_Equalizers.emplace_back(m_Parameters.Parameters());
			
			m_Buffers.reserve(c);
			while (m_Buffers.size() < c)
			{
				auto& a = m_Buffers.emplace_back();
				while (a.size() < BUFFER_SIZE)
					a.emplace_back(0);
			}
		}

		int wait = 0;
		float Process(float sin, int c) override
		{
			float in = sin * m_InputGain;
			if (c == 0)
			{
				m_Position = (m_Position + 1) % BUFFER_SIZE;
				m_Oscillator.Process();
			}

			int s = m_Mode == 0 ? 0 : ((c % 2) * 0.5);
			int delayt = ((m_Delay + m_Delay * m_Oscillator.Sample() * m_ModAmount * 0.01 * 0.9) / 1000.0) * m_SampleRate;
			delayt = (std::max(delayt, 1)) % BUFFER_SIZE;

			auto& _buffer = m_Buffers[c];
			int i1 = (int)(m_Position - (m_Mode == 0 ? delayt : (delayt + delayt * (c % 2) * 0.5)) + 3 * BUFFER_SIZE) % BUFFER_SIZE;

			float del1s = _buffer[i1];

			float now = m_FilterOn ? m_Equalizers[c].Apply(del1s) : del1s;

			int next = (m_Mode == 0) ? m_Position : ((int)(m_Position - (c % 2) * delayt * 0.5 - 1 + 3 * BUFFER_SIZE) % BUFFER_SIZE);
			_buffer[m_Position] = in;
			_buffer[next] += now * m_Feedback;

			return sin * (1.0 - m_Mix) + now * m_Mix;
		}

		void UpdateParams()
		{
			m_FilterOn = m_EnableFilter.State();
			if (m_FilterOn)
				m_Freq.Enable(), m_Width.Enable();
			else
				m_Freq.Disable(), m_Width.Disable();
			
			m_Delay = m_DelayKnob.Value();
			m_InputGain = db2lin(m_InputGainKnob.Value());
			m_ModAmount = m_ModAmountKnob.Value();
			m_Oscillator.frequency = m_ModRateKnob.Value();
			m_Feedback = m_FeedbackKnob.Value() * 0.01;
			m_Mix = m_MixKnob.Value() * 0.01;
			m_Parameters.width = m_Width.Value();
			m_Parameters.freq = m_Freq.Value();
			m_Parameters.RecalculateParameters();
			m_FilterOn = m_EnableFilter.State();
		}

	private:
		int BUFFER_SIZE = m_SampleRate * 10;
		std::vector<std::vector<float>> m_Buffers;
		int m_Position = 0;

		double m_Delay = 0,
			m_Mix = 1,
			m_ModAmount = 0,
			m_ModRate = 0,
			m_Feedback = 0,
			m_InputGain = 0;

		bool m_FilterOn = true;

		Oscillator m_Oscillator;


		SimpleFilterParameters m_Parameters;
		std::vector<ChannelEqualizer<2, BiquadFilter<>>> m_Equalizers;

		bool m_Dragging = false;
		int m_Mode = 1;
		SoundMixr::Parameter& m_DelayKnob, & m_MixKnob, & m_Freq, & m_Width;
		SoundMixr::Parameter& m_ModAmountKnob, & m_ModRateKnob, & m_FeedbackKnob, &m_InputGainKnob;
		SoundMixr::RadioButton& m_PingPong, & m_Normal;
		SoundMixr::ToggleButton& m_EnableFilter;
		
		SoundMixr::SimpleFilterCurve& m_Curve;
	};
}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::Delay;
}