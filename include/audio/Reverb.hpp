#pragma once
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

class Reverb : public Effect
{
public:
	Reverb()
		: m_DecayKnob(Emplace<KnobSlider>("Decay")),
		m_FeedbackKnob(Emplace<KnobSlider>("Feedback")),
		m_MixKnob(Emplace<KnobSlider>("Mix")),
		Effect("Reverb")
	{
		Height(140);
		m_DecayKnob.Range({ 0.1, 20000 });
		m_DecayKnob.Power(5);
		m_DecayKnob.ResetValue(600);
		m_DecayKnob.ResetValue();
		m_DecayKnob.Unit(" ms");
		m_DecayKnob.Unit(" s", 3);
		m_DecayKnob.Size({ 30, 30 });
		m_DecayKnob.Decimals(2);
		m_DecayKnob.Multiplier(0.4);

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
		m_DecayKnob.Position({ 10, 21 });
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
		m_Comb1Y.reserve(c);
		while (m_Comb1Y.size() < c)
		{
			auto& v = m_Comb1Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_Comb2Y.reserve(c);
		while (m_Comb2Y.size() < c)
		{
			auto& v = m_Comb2Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_Comb3Y.reserve(c);
		while (m_Comb3Y.size() < c)
		{
			auto& v = m_Comb3Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_Comb4Y.reserve(c);
		while (m_Comb4Y.size() < c)
		{
			auto& v = m_Comb4Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_APass1Y.reserve(c);
		while (m_APass1Y.size() < c)
		{
			auto& v = m_APass1Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_APass1X.reserve(c);
		while (m_APass1X.size() < c)
		{
			auto& v = m_APass1X.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_APass2Y.reserve(c);
		while (m_APass2Y.size() < c)
		{
			auto& v = m_APass2Y.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}
		m_APass2X.reserve(c);
		while (m_APass2X.size() < c)
		{
			auto& v = m_APass2X.emplace_back();
			for (int i = 0; i < BUFFER_SIZE; i++)
				v.push_back(0);
		}

		Effect::Channels(c);
	}
	
	std::vector<std::vector<float>> m_Comb1Y;
	std::vector<std::vector<float>> m_Comb2Y;
	std::vector<std::vector<float>> m_Comb3Y;
	std::vector<std::vector<float>> m_Comb4Y;
	std::vector<std::vector<float>> m_APass1Y;
	std::vector<std::vector<float>> m_APass2Y;
	std::vector<std::vector<float>> m_APass1X;
	std::vector<std::vector<float>> m_APass2X;
	int m_ReadPos = 0, n = 0, m_DelaySamples = 1;
	float m_Feedback = 0.3, m_Mix = 0.5;
	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		if (c == 0)
		{
			n = (n + 1) % BUFFER_SIZE;
			//m_ReadPos = (m_WritePos - m_DelaySamples + BUFFER_SIZE) % BUFFER_SIZE;
		}
		auto& comb1y = m_Comb1Y[c];
		auto& comb2y = m_Comb1Y[c];
		auto& comb3y = m_Comb2Y[c];
		auto& comb4y = m_Comb3Y[c];
		auto& apas1x = m_APass1X[c];
		auto& apas1y = m_APass1Y[c];
		auto& apas2x = m_APass2X[c];
		auto& apas2y = m_APass2Y[c];

		int nm = (int)(n - 1 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm1 = (int)(n - m_DelaySamples * 0.799 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm2 = (int)(n - m_DelaySamples * 0.999 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm3 = (int)(n - m_DelaySamples * 0.399 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm4 = (int)(n - m_DelaySamples * 0.201 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm5 = (int)(n - m_DelaySamples * 0.021 + BUFFER_SIZE) % BUFFER_SIZE;
		int nm6 = (int)(n - m_DelaySamples * 0.017 + BUFFER_SIZE) % BUFFER_SIZE;

		float in = sin + apas2y[nm] * m_Feedback;

		comb1y[n] = in + -0.742 * comb1y[nm1];
		comb2y[n] = in + -0.733 * comb2y[nm1];
		comb3y[n] = in + -0.715 * comb3y[nm1];
		comb4y[n] = in + -0.697 * comb4y[nm1];

		float g = 0.7;
		apas1x[n] = (comb1y[n] + comb2y[n] + comb3y[n] + comb4y[n]) / 4.0f;
		apas1y[n] = (-g * apas1x[n]) + apas1x[nm5] + (g * apas1y[nm5]);
		apas2x[n] = apas1y[n];
		apas2y[n] = (-g * apas2x[n]) + apas2x[nm6] + (g * apas2y[nm6]);


		return sin * (1.0 - m_Mix) + m_Mix * apas2y[n];
	}

	void UpdateParams()
	{
		m_Mix = m_MixKnob.Value() * 0.01;
		m_Feedback = m_FeedbackKnob.Value() * 0.01;
		m_DelaySamples = constrain(m_DecayKnob.Value() * 0.001 * Effect::sampleRate, 0, BUFFER_SIZE - 1);
	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Reverb";
		return _json;
	}

	void operator=(const json& json)
	{
		UpdateParams();
	}

private:
	static inline constexpr int BUFFER_SIZE = 48000;

	KnobSlider
		& m_DecayKnob, &m_MixKnob, &m_FeedbackKnob;
};
