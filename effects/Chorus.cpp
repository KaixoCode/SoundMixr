#include "audio/Effects.hpp"
#include "audio/Oscillator.hpp"

class Chorus : public Effect
{
public:
	Chorus()
		: m_AmountKnob(Emplace<NormalSlider>("Amount")), 
		m_RateKnob(Emplace<NormalSlider>("Rate")),
		m_Delay1Knob(Emplace<KnobSlider>("Delay 1")),
		m_Delay2Knob(Emplace<KnobSlider>("Delay 2")),
		m_MixKnob(Emplace<KnobSlider>("Mix")),
		m_Highpass(Emplace<KnobSlider>("Highpass")),
		m_FeedbackKnob(Emplace<NormalSlider>("Feedback")),
		m_Key1(ButtonType::List::NewKey()),
		m_Delay2Type1(Emplace<Button<RadioButton, ButtonType::List>>([&] { m_Delay2Type = 0; }, "Off", m_Key1)),
		m_Delay2Type2(Emplace<Button<RadioButton, ButtonType::List>>([&] { m_Delay2Type = 1; }, "Fix", m_Key1)),
		m_Delay2Type3(Emplace<Button<RadioButton, ButtonType::List>>([&] { m_Delay2Type = 2; }, "Mod", m_Key1)),
		m_Key2(ButtonType::List::NewKey()),
		m_PolarityP(Emplace<Button<RadioButton, ButtonType::List>>([&] { m_Polarity = 1; }, "+", m_Key2)),
		m_PolarityM(Emplace<Button<RadioButton, ButtonType::List>>([&] { m_Polarity = -1; }, "-", m_Key2)),
		Effect("Chorus")
	{}

	void Init() override
	{
		Height(170);
		m_AmountKnob.Range({ 0, 5 });
		m_AmountKnob.Power(3);
		m_AmountKnob.ResetValue(3);
		m_AmountKnob.ResetValue();
		m_AmountKnob.Unit(" ms");
		m_AmountKnob.Size({ 48, 18 });
		m_AmountKnob.Decimals(2);
		m_AmountKnob.Multiplier(1);
		m_AmountKnob.Vertical(false);
		m_AmountKnob.DisplayName(false);

		m_RateKnob.Range({ 0.1, 15 });
		m_RateKnob.Power(3);
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

		m_FeedbackKnob.Range({ 0, 90 });
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


		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT && e.x > 125 && e.x < 226 && e.y > 34 && e.y < 135)
				m_Dragging = true;
		};

		m_Listener += [this](Event::MouseDragged& e)
		{
			if (m_Dragging)
			{
				double _x = constrain((e.x - 125 - 8) / (101 - 8 * 2.0), 0, 1);
				double _y = constrain((e.y - 34 - 8) / (101 - 8 * 2.0), 0, 1);
				m_AmountKnob.NormalizedValue(_x);
				m_RateKnob.NormalizedValue(_y);
			}
		};

		m_Listener += [this](Event::MouseReleased& e)
		{
			m_Dragging = false;
		};

		m_Oscillator.wavetable = [](double p) { return std::sin(p * M_PI * 2); };
	}

	void Update(const Vec4<int>& v) override 
	{
		Background(theme->Get(C::Channel));
		m_Highpass.Position({ 16, 21 });
		m_Delay1Knob.Position({ 16, 89 });
		m_Delay2Knob.Position({ 73, 89 });
		m_Delay2Type3.Position({ 68, 10 });
		m_Delay2Type2.Position({ 68, 32 });
		m_Delay2Type1.Position({ 68, 54 });
		m_AmountKnob.Position({ 125, 10 });
		m_RateKnob.Position({ 179, 10 });
		m_MixKnob.Position({ 252, 21 });
		m_FeedbackKnob.Position({ 243, 74 });
		m_PolarityP.Position({ 243, 117 });
		m_PolarityM.Position({ 270, 117 });

		if (m_Delay2Type == 0)
			m_Delay2Knob.Disable();
		else
			m_Delay2Knob.Enable();

		UpdateParams();
		Effect::Update(v);
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
		d.Command<Quad>(Vec4<int>{ m_Delay2Knob.X() - 14, 10, 1, Height() - 45});
		d.Command<Quad>(Vec4<int>{ m_Delay2Knob.X() + m_Delay2Knob.Width() + 13, 10, 1, Height() - 45});
		d.Command<Quad>(Vec4<int>{ m_RateKnob.X() + m_RateKnob.Width() + 8, 10, 1, Height() - 45});

		d.Command<Fill>(theme->Get(C::Dynamics));
		d.Command<Quad>(Vec4<int>{ 125, 34, 101, 101 });

		int _p = 8;
		int _x = m_AmountKnob.NormalizedValue() * (101 - 2 * _p) + 125 + _p;
		int _y = m_RateKnob.NormalizedValue() * (101 - 2 * _p) + 34 + _p;
		d.Command<Fill>(theme->Get(C::VSlider));
		d.Command<Graphics::Ellipse>(Vec4<int>{ _x, _y, _p * 2, _p * 2 });

		d.Command<PopMatrix>();
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
			
			if (m_Delay2Type == 2)
				m_Delay2t = ((m_Delay2) / 1000.0) * Effect::sampleRate;
			else
				m_Delay2t = ((m_Delay2 + m_Oscillator.sample * m_Amount) / 1000.0) * Effect::sampleRate;

			m_Delay1t = (std::max(m_Delay1t, 0)) % BUFFER_SIZE;
			m_Delay2t = (std::max(m_Delay2t, 0)) % BUFFER_SIZE;
		}

		auto& _buffer = m_Buffers[c];

		if (m_Delay2Type == 0)
		{
			int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;

			float del1s = _buffer[i1];

			float now = del1s;

			_buffer[m_Position] = sin + now * m_Feedback;

			return sin * (1.0 - m_Mix) + m_Polarity * now * m_Mix;
		}
		else
		{
			int i1 = (m_Position - m_Delay1t + BUFFER_SIZE) % BUFFER_SIZE;
			int i2 = (m_Position - m_Delay2t + BUFFER_SIZE) % BUFFER_SIZE;

			float del1s = _buffer[i1];
			float del2s = _buffer[i2];
		
			float now = (del1s + del2s) / 2;

			_buffer[m_Position] = sin + now * m_Feedback;

			return sin * (1.0 - m_Mix) + m_Polarity * now * m_Mix;
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

	bool m_Dragging = false;
	int m_Key1, m_Key2;
	int m_Delay2Type = 0, m_Polarity = 1;
	KnobSlider& m_Delay1Knob, & m_Delay2Knob, & m_MixKnob, & m_Highpass;
	NormalSlider& m_AmountKnob, & m_RateKnob, & m_FeedbackKnob;
	Button<RadioButton, ButtonType::List> & m_Delay2Type1, & m_Delay2Type2, & m_Delay2Type3, & m_PolarityP, & m_PolarityM;
};


extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Chorus();
	}
}