#include "audio/Effects.hpp"
#include "audio/Oscillator.hpp"

class Chorus : public Effect
{
public:
	Chorus()
		: m_AmountKnob(Emplace<KnobSlider>("Amount")), 
		m_RateKnob(Emplace<KnobSlider>("Rate")),
		m_Delay1Knob(Emplace<KnobSlider>("Delay 1")),
		m_Delay2Knob(Emplace<KnobSlider>("Delay 2")),
		m_MixKnob(Emplace<KnobSlider>("Mix")),
		m_FeedbackKnob(Emplace<KnobSlider>("Feedback")),
		Effect("Chorus")
	{}

	void Init() override
	{
		Height(120);
		m_AmountKnob.Range({ 0, 5 });
		m_AmountKnob.Power(3);
		m_AmountKnob.ResetValue(3);
		m_AmountKnob.ResetValue();
		m_AmountKnob.Unit(" ms");
		m_AmountKnob.Size({ 30, 30 });
		m_AmountKnob.Decimals(2);
		m_AmountKnob.Multiplier(0.4);

		m_RateKnob.Range({ 0.1, 15 });
		m_RateKnob.Power(3);
		m_RateKnob.ResetValue(3);
		m_RateKnob.ResetValue();
		m_RateKnob.Unit(" Hz");
		m_RateKnob.Size({ 30, 30 });
		m_RateKnob.Decimals(2);
		m_RateKnob.Multiplier(0.4);

		m_Delay1Knob.Range({ 0.5, 20 });
		m_Delay1Knob.Power(3);
		m_Delay1Knob.ResetValue(3);
		m_Delay1Knob.ResetValue();
		m_Delay1Knob.Unit(" ms");
		m_Delay1Knob.Size({ 30, 30 });
		m_Delay1Knob.Decimals(2);
		m_Delay1Knob.Multiplier(0.4);

		m_Delay2Knob.Range({ 0.5, 20 });
		m_Delay2Knob.Power(3);
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

		m_FeedbackKnob.Range({ 0, 90 });
		m_FeedbackKnob.Power(1);
		m_FeedbackKnob.ResetValue(10);
		m_FeedbackKnob.ResetValue();
		m_FeedbackKnob.Unit(" %");
		m_FeedbackKnob.Size({ 30, 30 });
		m_FeedbackKnob.Decimals(1);
		m_FeedbackKnob.Multiplier(0.4);

		m_Oscillator.wavetable = [](double p) { return std::sin(p * M_PI * 2); };
	}

	void Update(const Vec4<int>& v) override 
	{
		Background(theme->Get(C::Channel));
		m_AmountKnob.Position({ 10, 21 });
		m_RateKnob.Position({ 60, 21 });
		m_Delay1Knob.Position({ 110, 21 });
		m_Delay2Knob.Position({ 160, 21 });
		m_MixKnob.Position({ 210, 21 });
		m_FeedbackKnob.Position({ 260, 21 });
		UpdateParams();
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
			auto& a = m_Buffers.emplace_back();
			while (a.size() < BUFFER_SIZE)
				a.emplace_back(0);
		}
		Effect::Channels(c);
	}

	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		if (c == 0)
		{
			m_Position = (m_Position + 1) % BUFFER_SIZE;
			m_Delay1t = ((m_Delay1 + m_Oscillator.NextSample() * m_Amount) / 1000.0) * Effect::sampleRate;
			m_Delay2t = ((m_Delay2) / 1000.0) * Effect::sampleRate;

			m_Delay1t = (std::max(m_Delay1t, 0)) % BUFFER_SIZE;
			m_Delay2t = (std::max(m_Delay2t, 0)) % BUFFER_SIZE;
		}

		auto& _buffer = m_Buffers[c];

		int i1 = (m_Position + m_Delay1t) % BUFFER_SIZE;
		int i2 = (m_Position + m_Delay2t) % BUFFER_SIZE;

		float del1s = _buffer[i1];
		float del2s = _buffer[i2];
		float now = (del1s + del2s) / 2;
		_buffer[m_Position] = sin + now * m_Feedback;

		return sin * (1.0 - m_Mix) + now * m_Mix;
	}

	void UpdateParams()
	{
		m_Delay1 = m_Delay1Knob.Value();
		m_Delay2 = m_Delay2Knob.Value();
		m_Amount = m_AmountKnob.Value();
		m_Oscillator.frequency = m_RateKnob.Value();
		m_Feedback = m_FeedbackKnob.Value() * 0.01;
		m_Mix = m_MixKnob.Value() * 0.01;
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Chorus";
		return _json;
	}

	void operator=(const json& json)
	{
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

	KnobSlider& m_AmountKnob, & m_RateKnob, & m_Delay1Knob, & m_Delay2Knob, & m_MixKnob, & m_FeedbackKnob;
};


extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Chorus();
	}
}