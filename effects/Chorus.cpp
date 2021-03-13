#include "EffectBase.hpp"
#include "Oscillator.hpp"
#include "Filters.hpp"

namespace Effects
{
	class Chorus : public EffectBase
	{
	public:
		Chorus()
			: m_AmountKnob(Parameter("Amount", ParameterType::Slider)), 
			m_RateKnob(Parameter("Rate", ParameterType::Slider)),
			m_Delay1Knob(Parameter("Delay 1", ParameterType::Knob)),
			m_Delay2Knob(Parameter("Delay 2", ParameterType::Knob)),
			m_MixKnob(Parameter("Mix", ParameterType::Knob)),
			m_Highpass(Parameter("Highpass", ParameterType::Knob)),
			m_FeedbackKnob(Parameter("Feedback", ParameterType::Slider)),
			m_Delay2Type1(RadioButton("Off", 2, [&] { m_Delay2Type = 0; })),
			m_Delay2Type2(RadioButton("Fix", 2, [&] { m_Delay2Type = 1; })),
			m_Delay2Type3(RadioButton("Mod", 2, [&] { m_Delay2Type = 2; })),
			m_PolarityP(RadioButton("+", 1, [&]{ m_Polarity = 1; })),
			m_PolarityM(RadioButton("-", 1, [&]{ m_Polarity = -1; })),
			m_Controller(XYController(m_AmountKnob, m_RateKnob)),
			EffectBase("Chorus")
		{
			Height(145);
			m_AmountKnob.Range({ 0, 5 });
			m_AmountKnob.Power(2);
			m_AmountKnob.ResetValue(3);
			m_AmountKnob.ResetValue();
			m_AmountKnob.Unit(" ms");
			m_AmountKnob.Size({ 48, 18 });
			m_AmountKnob.Decimals(2);
			m_AmountKnob.Multiplier(1);
			m_AmountKnob.Vertical(false);
			m_AmountKnob.DisplayName(false);

			m_RateKnob.Range({ 0.1, 15 });
			m_RateKnob.Power(2);
			m_RateKnob.ResetValue(3);
			m_RateKnob.ResetValue();
			m_RateKnob.Unit(" Hz");
			m_RateKnob.Size({ 47, 18 });
			m_RateKnob.Decimals(2);
			m_RateKnob.Multiplier(1);
			m_RateKnob.Vertical(false);
			m_RateKnob.DisplayName(false);

			m_Highpass.Range({ 10, 22000 });
			m_Highpass.Power(3);
			m_Highpass.ResetValue(100);
			m_Highpass.ResetValue();
			m_Highpass.Unit(" Hz");
			m_Highpass.Unit(" kHz", 3);
			m_Highpass.Size({ 30, 30 });
			m_Highpass.Decimals(2);
			m_Highpass.Multiplier(0.4);

			m_Delay1Knob.Range({ 0.5, 20 });
			m_Delay1Knob.Power(1);
			m_Delay1Knob.ResetValue(3);
			m_Delay1Knob.ResetValue();
			m_Delay1Knob.Unit(" ms");
			m_Delay1Knob.Size({ 30, 30 });
			m_Delay1Knob.Decimals(2);
			m_Delay1Knob.Multiplier(0.4);

			m_Delay2Knob.Range({ 0.5, 20 });
			m_Delay2Knob.Power(1);
			m_Delay2Knob.ResetValue(3);
			m_Delay2Knob.ResetValue();
			m_Delay2Knob.Unit(" ms");
			m_Delay2Knob.Size({ 30, 30 });
			m_Delay2Knob.Decimals(2);
			m_Delay2Knob.Multiplier(0.4);

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
			m_FeedbackKnob.Size({ 48, 18 });
			m_FeedbackKnob.Decimals(1);
			m_FeedbackKnob.Multiplier(1);
			m_FeedbackKnob.Vertical(false);

			m_Delay2Type3.Size({ 40, 18 });
			m_Delay2Type2.Size({ 40, 18 });
			m_Delay2Type1.Size({ 40, 18 });
			m_Delay2Type2.Selected(true);
			m_Delay2Type = 1;

			m_PolarityP.Selected(true);
			m_PolarityP.Size({ 21, 18 });
			m_PolarityM.Size({ 21, 18 });
			m_Polarity = 1;

			m_Controller.Size({ 102, 102 });

			Div().Align(Div::Alignment::Horizontal);
			Div().Divs(4);
			Div().Dividers(true);
			Div()[0].Align(Div::Alignment::Vertical);
			Div()[0].DivSize(60);
			Div()[0].Divs(2);
			Div()[0][1] = m_Delay1Knob;
			Div()[0][0] = m_Highpass;
			Div()[1].Align(Div::Alignment::Vertical);
			Div()[1].DivSize(60);
			Div()[1].Divs(2);
			Div()[1][1] = m_Delay2Knob;
			Div()[1][0].Align(Div::Alignment::Vertical);
			Div()[1][0].DivSize(71);
			Div()[1][0].Divs(4);
			Div()[1][0][3].Align(Div::Alignment::Top);
			Div()[1][0][3] = m_Delay2Type1;
			Div()[1][0][2].Align(Div::Alignment::Top);
			Div()[1][0][2] = m_Delay2Type2;
			Div()[1][0][1].Align(Div::Alignment::Top);
			Div()[1][0][1] = m_Delay2Type3;
			Div()[1][0][0].DivSize(4);
			Div()[2].Align(Div::Alignment::Vertical);
			Div()[2].DivSize(118);
			Div()[2].Padding(4);
			Div()[2].Divs(3);
			Div()[2][0].DivSize(4);
			Div()[2][1].Align(Div::Alignment::Horizontal);
			Div()[2][1].DivSize(27);
			Div()[2][1].Divs(2);
			Div()[2][1][0] = m_AmountKnob;
			Div()[2][1][1] = m_RateKnob;
			Div()[2][2].Align(Effects::Div::Alignment::Bottom);
			Div()[2][2] = m_Controller;
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
			Div()[3][0][1] = m_MixKnob;
			Div()[3][0][2] = m_FeedbackKnob;

			m_Oscillator.wavetable = [](double p) { return std::sin(p * 3.14159265359 * 2); };
		}

		void Update() override
		{
			if (m_Delay2Type == 0)
				m_Delay2Knob.Disable();
			else
				m_Delay2Knob.Enable();

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
				m_Delay1t = ((m_Delay1 + m_Oscillator.NextSample() * m_Amount) / 1000.0) * m_SampleRate;
			
				if (m_Delay2Type == 2)
					m_Delay2t = ((m_Delay2) / 1000.0) * m_SampleRate;
				else
					m_Delay2t = ((m_Delay2 + m_Oscillator.sample * m_Amount) / 1000.0) * m_SampleRate;

				m_Delay1t = (std::max(m_Delay1t, 1)) % BUFFER_SIZE;
				m_Delay2t = (std::max(m_Delay2t, 1)) % BUFFER_SIZE;
			}

			auto& _buffer = m_Buffers[c];

			if (m_Delay2Type == 0)
			{
				int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;

				float del1s = _buffer[i1];

				float now = del1s;

				_buffer[m_Position] = sin + m_Polarity * now * m_Feedback;

				return sin * (1.0 - m_Mix) + m_Equalizers[c].Apply(now * m_Mix);
			}
			else
			{
				int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;
				int i2 = (m_Position - m_Delay2t + BUFFER_SIZE) % BUFFER_SIZE;

				float del1s = _buffer[i1];
				float del2s = _buffer[i2];
		
				float now = (del1s + del2s) / 2.0;

				_buffer[m_Position] = sin + m_Polarity * now * m_Feedback;

				return sin * (1.0 - m_Mix) + m_Equalizers[c].Apply(now * m_Mix);
			}
		}

		void UpdateParams()
		{
			m_Delay1 = m_Delay1Knob.Value();
			m_Delay2 = m_Delay2Knob.Value();
			m_Amount = m_AmountKnob.Value();
			m_Oscillator.frequency = m_RateKnob.Value();
			m_Feedback = m_FeedbackKnob.Value() * 0.01;
			m_Mix = m_MixKnob.Value() * 0.01;
			m_Parameters[0].type = FilterType::HighPass;
			m_Parameters[0].Q = 1;
			m_Parameters[0].f0 = m_Highpass.Value();
			m_Parameters[0].dbgain = 0;
			m_Parameters[0].RecalculateParameters();
		}

		operator nlohmann::json()
		{
			nlohmann::json _json = nlohmann::json::object();
			_json["type"] = "Chorus";
			_json["del1"] = m_Delay1Knob.Value();
			_json["del2"] = m_Delay2Knob.Value();
			_json["highp"] = m_Highpass.Value();
			_json["del2t"] = m_Delay2Type;
			_json["mamt"] = m_AmountKnob.Value();
			_json["mrte"] = m_RateKnob.Value();
			_json["pola"] = m_Polarity;
			_json["fdbc"] = m_FeedbackKnob.Value();
			_json["mix"] = m_MixKnob.Value();
			return _json;
		}

		void operator=(const nlohmann::json& json)
		{
			m_Delay1Knob.Value(json.at("del1").get<double>());
			m_Delay2Knob.Value(json.at("del2").get<double>());
			m_Highpass.Value(json.at("highp").get<double>());
			m_AmountKnob.Value(json.at("mamt").get<double>());
			m_RateKnob.Value(json.at("mrte").get<double>());
			m_FeedbackKnob.Value(json.at("fdbc").get<double>());
			m_MixKnob.Value(json.at("mix").get<double>());
			int type = json.at("del2t").get<int>();
			if (type == 0)
				m_Delay2Type1.Selected(true);
			else if (type == 1)
				m_Delay2Type2.Selected(true);
			else if (type == 2)
				m_Delay2Type3.Selected(true);
			int pola = json.at("pola").get<int>();
			if (pola == 0)
				m_PolarityP.Selected(true);
			else if (pola == 1)
				m_PolarityM.Selected(true);
			UpdateParams();
		}

	private:
		static inline constexpr int BUFFER_SIZE = 2048;
		std::vector<std::vector<float>> m_Buffers;
		int m_Position = 0;
		double m_Delay1 = 5;
		int m_Delay1t = 5;
		double m_Delay2 = 5;
		int m_Delay2t = 5;
		double m_Amount = 3;
		double m_Feedback = 0.3;
		double m_Mix = 0.5;
		Oscillator m_Oscillator;

		BiquadParameters m_Parameters[1];
		std::vector<ChannelEqualizer<1, BiquadFilter<>>> m_Equalizers;

		bool m_Dragging = false;
		int m_Key1, m_Key2;
		int m_Delay2Type = 0, m_Polarity = 1;
		Effects::Parameter& m_Delay1Knob, & m_Delay2Knob, & m_MixKnob, & m_Highpass;
		Effects::Parameter& m_AmountKnob, & m_RateKnob, & m_FeedbackKnob;
		Effects::RadioButton & m_Delay2Type1, & m_Delay2Type2, & m_Delay2Type3, & m_PolarityP, & m_PolarityM;
		Effects::XYController& m_Controller;
	};
}
extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Effects::Chorus();
	}
}