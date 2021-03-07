#include "audio/Dynamics.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

Dynamics::Dynamics()
	: m_PreGain(Emplace<KnobSlider>("PreGain")),
	m_Attack(Emplace<KnobSlider>("Attack")),
	m_Release(Emplace<KnobSlider>("Release")),
	m_PostGain(Emplace<KnobSlider>("PostGain")),
	m_Mix(Emplace<KnobSlider>("Mix")),
	m_Slider(Emplace<DynamicsSlider>()),
	Effect("Dynamics")
{
	Height(200);
	m_PreGain.Range({ -24, 24 });
	m_PreGain.ResetValue(0);
	m_PreGain.ResetValue();
	m_PreGain.Unit("dB");
	m_PreGain.Size({ 30, 30 });
	m_PreGain.Multiplier(0.4);
	m_PreGain.Decimals(2);

	m_Attack.Range({ .1f, 30 });
	m_Attack.Power(2);
	m_Attack.Multiplier(0.4);
	m_Attack.ResetValue(3);
	m_Attack.ResetValue();
	m_Attack.Unit(" ms");
	m_Attack.Size({ 30, 30 });

	m_Release.Range({ 1, 300 });
	m_Release.Power(2);
	m_Release.ResetValue(30);
	m_Release.ResetValue();
	m_Release.Multiplier(0.4);
	m_Release.Unit(" ms");
	m_Release.Size({ 30, 30 });

	m_PostGain.Range({ -24, 24 });
	m_PostGain.Unit("dB");
	m_PostGain.ResetValue(0);
	m_PostGain.ResetValue();
	m_PostGain.Size({ 30, 30 });
	m_PostGain.Multiplier(0.4);
	m_PostGain.Decimals(2);

	m_Mix.Range({ 0, 100 });
	m_Mix.Multiplier(0.4);
	m_Mix.ResetValue(100);
	m_Mix.ResetValue();
	m_Mix.Unit(" %");
	m_Mix.Size({ 30, 30 });
	m_Mix.Decimals(0);

	UpdateParams();

	Background(Theme<C::Channel>::Get());
}

void Dynamics::Update(const Vec4<int>& v)
{
	m_PreGain.Position({ Width() - 280, 20 });
	m_PostGain.Position({ Width() - 225, 20 });
	m_Attack.Position({ Width() - 162, 20 });
	m_Release.Position({ Width() - 107, 20 });
	m_Mix.Position({ Width() - 45, 20 });
	m_Slider.Position({ 12, 93 });
	m_Slider.Size({ (Width() - 24), Height() - 123 });
	Background(Theme<C::Channel>::Get());
	Effect::Update(v);
	UpdateParams();
}

void Dynamics::Render(CommandCollection& d)
{
	Effect::Render(d);
	if (m_Small)
		return;

	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	d.Command<Fill>(Theme<C::Divider>::Get());
	d.Command<Quad>(Vec4<int>{(m_PostGain.X() + m_PostGain.Width() + m_Attack.X()) / 2, 10, 1, 50});
	d.Command<Quad>(Vec4<int>{(m_Release.X() + m_Release.Width() + m_Mix.X()) / 2, 10, 1, 50});
	d.Command<Fill>(Theme<C::TextSmall>::Get());
	d.Command<Font>(Fonts::Gidole14, 14.0f);
	d.Command<TextAlign>(Align::LEFT, Align::BOTTOM);
	d.Command<Text>(&m_Slider.m_TH2Str, Vec2<int>{10, 77});
	d.Command<Text>(&m_Slider.m_RT2Str, Vec2<int>{70, 77});
	d.Command<Text>(&m_Slider.m_RT1Str, Vec2<int>{Width() - 110, 77});
	d.Command<TextAlign>(Align::RIGHT, Align::BOTTOM);
	d.Command<Text>(&m_Slider.m_TH1Str, Vec2<int>{Width() - 10, 77});

	int _channels = m_Channels;
	int _x = 12;
	int _rw = Width() - 24;
	int _p = 16;
	int _h = ((Height() - 123 - 22 - _p * 2) / _channels) - (_channels > 4 ? 1 : 2);
	int _y = 94 + _p;

	// Draw all audio meters
	for (int i = 0; i < _channels; i++)
	{
		_y = 94 + _p + i * (_h + (_channels > 4 ? 1 : 2));
		float _level = std::powf(m_Levels[i], 0.25);

		int _w = (std::min(_level, 1.0f) / 1.0f) * (_rw);
		d.Command<Graphics::Fill>(Theme<C::VMeterFill>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
	}

	d.Command<PopMatrix>();
}

void Dynamics::Channels(int c)
{
	while (m_Levels.size() < c)
		m_Levels.push_back(0);

	while (m_Peaks.size() < c)
		m_Peaks.push_back(0);

	m_Channels = c;
}

float Dynamics::NextSample(float sin, int c) 
{
	if (!m_Enabled)
		return sin;

	if (c == 0)
	{
		if (counter > 512)
			counter = 0;

		counter++;
	}

	float out = m_Compressor.NextSample(sin, c);

	float abs = out;
	myabs(abs);
	auto& l = m_Peaks[c];
	if (abs > l)
		l = abs;

	if (counter > 512)
		m_Levels[c] = m_Levels[c] * r + m_Peaks[c] * (1.0 - r),
		m_Peaks[c] = 0;

	return out;
}