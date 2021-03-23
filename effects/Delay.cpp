#pragma once
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Delay : public Effect
{
public:
	Delay()
		: m_DelayKnob(Emplace<KnobSlider>("Delay")),
		m_FeedbackKnob(Emplace<KnobSlider>("Feedback")),
		m_MixKnob(Emplace<KnobSlider>("Mix")),
		Effect("Delay")
	{}

	void Init() override
	{
		Height(140);
		m_DelayKnob.Range({ 0.1, 20000 });
		m_DelayKnob.Power(5);
		m_DelayKnob.ResetValue(600);
		m_DelayKnob.ResetValue();
		m_DelayKnob.Unit(" ms");
		m_DelayKnob.Unit(" s", 3);
		m_DelayKnob.Size({ 30, 30 });
		m_DelayKnob.Decimals(2);
		m_DelayKnob.Multiplier(0.4);

		m_FeedbackKnob.Range({ 0, 100 });
		m_FeedbackKnob.ResetValue(30);
		m_FeedbackKnob.ResetValue();
		m_FeedbackKnob.Unit("%");
		m_FeedbackKnob.Decimals(1);
		m_FeedbackKnob.Size({ 30, 30 });
		m_FeedbackKnob.Multiplier(0.4);

		m_MixKnob.Range({ 0, 100 });
		m_MixKnob.ResetValue(50);
		m_MixKnob.ResetValue();
		m_MixKnob.Unit("%");
		m_MixKnob.Decimals(1);
		m_MixKnob.Size({ 30, 30 });
		m_MixKnob.Multiplier(0.4);
	}

	void Update(const Vec4<int>& v) override 
	{
		m_DelayKnob.Position({ 10, 21 });
		m_FeedbackKnob.Position({ 65, 21 });
		m_MixKnob.Position({ 120, 21 });
		UpdateParams();
		Background(theme->Get(C::Channel));
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
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}

		Effect::Channels(c);
	}
	
	std::vector<std::vector<float>> m_Buffers;
	int m_ReadPos = 0, m_WritePos = 0, m_DelaySamples = 48000;
	float m_Feedback = 0.3, m_Mix = 0.5;
	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		if (c == 0)
		{
			m_WritePos = (m_WritePos + 1) % BUFFER_SIZE;
			m_ReadPos = (m_WritePos - m_DelaySamples + BUFFER_SIZE) % BUFFER_SIZE;
		}
		auto& buffer = m_Buffers[c];
		buffer[m_WritePos] = sin;
		
		float r = buffer[m_ReadPos];
		buffer[m_WritePos] += r * m_Feedback;

		return sin * (1.0 - m_Mix) + m_Mix * r;
	}

	void UpdateParams()
	{
		m_Mix = m_MixKnob.Value() * 0.01;
		m_Feedback = m_FeedbackKnob.Value() * 0.01;
		m_DelaySamples = constrain(m_DelayKnob.Value() * 0.001 * Effect::sampleRate, 0, 20 * 480000 - 2);
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Delay";
		return _json;
	}

	void operator=(const json& json)
	{
		UpdateParams();
	}

private:
	static inline constexpr int BUFFER_SIZE = 48000 * 20;

	KnobSlider
		& m_DelayKnob, &m_MixKnob, &m_FeedbackKnob;
};

extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Delay();
	}
}