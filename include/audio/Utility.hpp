#pragma once
#include "audio/Effects.hpp"
#include "ui/Dropdown.hpp"

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
		m_InvertPhase(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "Phase")),
		m_Mute(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "Mute")),
		m_EnableEq(Emplace<Button<ToggleButton, ButtonType::Toggle>>([](bool s) {}, "EQ")),
		Effect("Utility")
	{
		Height(170);

		m_Release.Size({ 30, 30 });
		m_Release.Range({ 1, 300 });
		m_Release.ResetValue(30);
		m_Release.ResetValue();
		m_Release.Multiplier(0.4);
		m_Release.Unit(" ms");

		m_Low.Size({ 30, 30 });
		m_Low.Range({ -24, 24 });
		m_Low.Power(1);
		m_Low.ResetValue(0);
		m_Low.ResetValue();
		m_Low.Unit("dB");
		m_Low.Multiplier(0.4);

		m_Mid.Size({ 30, 30 });
		m_Mid.Range({ -24, 24 });
		m_Mid.Power(1);
		m_Mid.ResetValue(0);
		m_Mid.ResetValue();
		m_Mid.Unit("dB");
		m_Mid.Multiplier(0.4);

		m_High.Size({ 30, 30 });
		m_High.Range({ -24, 24 });
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

		m_Mute.Size({ 45, 20 });
		m_InvertPhase.Size({ 45, 20 });
		m_EnableEq.Size({ 70, 20 });

		m_LowFreq.Size({ 58, 20 });
		m_LowFreq.Range({ 10, 4000 });
		m_LowFreq.Power(3);
		m_LowFreq.ResetValue(600);
		m_LowFreq.ResetValue();
		m_LowFreq.Unit("Hz");
		m_LowFreq.Unit("kHz", 3);
		m_LowFreq.Decimals(2);
		m_LowFreq.Multiplier(1);
		m_LowFreq.Vertical(false);
		m_LowFreq.DisplayName(false);
		
		m_HighFreq.Size({ 58, 20 });
		m_HighFreq.Range({ 4000, 22000 });
		m_HighFreq.Power(3);
		m_HighFreq.ResetValue(7000);
		m_HighFreq.ResetValue();
		m_HighFreq.Unit("Hz");
		m_HighFreq.Unit("kHz", 3);
		m_HighFreq.Decimals(2);
		m_HighFreq.Multiplier(1);
		m_HighFreq.Vertical(false);
		m_HighFreq.DisplayName(false);

		m_Limiter.Size({ 30, 115 });
	}

	void Update(const Vec4<int>& v) override
	{
		m_EnableEq.Position({ 40, 113 });
		m_Low.Position({ 15, 55 });
		m_LowFreq.Position({ 10, 10 });
		m_Mid.Position({ 57, 55 });
		m_High.Position({ 99, 55 });
		m_HighFreq.Position({ 76, 10 });

		m_Pan.Position({ 160, 85 });
		m_InvertPhase.Position({ 152, 38 });
		m_Mute.Position({ 152, 10 });

		m_Release.Position({ 220, 20 });
		m_Gain.Position({ 220, 85 });
		m_Limiter.Position({ 265, 10 });

		Background(Theme<C::Channel>::Get());
		Effect::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		Effect::Render(d);
		d.Command<Graphics::PushMatrix>();
		d.Command<Graphics::Translate>(Position());
		d.Command<Graphics::Fill>(Theme<C::Divider>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{(m_HighFreq.X() + m_HighFreq.Width() + m_InvertPhase.X()) / 2, 10, 1, Height() - 45});
		d.Command<Graphics::Quad>(Vec4<int>{m_InvertPhase.X() +m_InvertPhase.Width() + 8, 10, 1, Height() - 45});
		d.Command<Graphics::PopMatrix>();
	}

	void Channels(int c) override
	{
		Effect::Channels(c);
	}


	int m_Freq = 128;
	double m_Mix = 0.5;
	float NextSample(float sin, int c) override
	{
		return sin;
	}

	void UpdateParams()
	{

	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Formant";
		return _json;
	}

	void operator=(const json& json)
	{
		double p1 = json["shft"].get<double>();
		UpdateParams();
	}

private:
	KnobSlider& m_Low, & m_Mid, & m_High, & m_Gain, & m_Release;
	NormalSlider& m_LowFreq, & m_HighFreq;
	PanKnob & m_Pan;
	VolumeSlider& m_Limiter;
	Button<ToggleButton, ButtonType::Toggle> &m_InvertPhase, &m_Mute, &m_EnableEq;
};
