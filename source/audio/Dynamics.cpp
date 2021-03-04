#include "audio/Dynamics.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Dynamics Effect -------------------------------- \\
// -------------------------------------------------------------------------- \\

Dynamics::Dynamics()
	: m_Knob(Emplace<KnobSlider>("PreGain")),
	m_Knob2(Emplace<KnobSlider>("Attack")),
	m_Knob3(Emplace<KnobSlider>("Release")),
	m_Knob4(Emplace<KnobSlider>("PostGain")),
	m_Knob5(Emplace<KnobSlider>("Mix")),
	m_Slider(Emplace<DynamicsSlider>()),
	Effect("Dynamics")
{
	Height(200);
	m_Knob.Range({ -24, 24 });
	m_Knob.ResetValue(0);
	m_Knob.ResetValue();
	m_Knob.Unit("dB");
	m_Knob.Size({ 30, 30 });
	m_Knob.Multiplier(0.5);
	m_Knob.Decimals(2);

	m_Knob2.Range({ .1f, 30 });
	m_Knob2.Power(2);
	m_Knob2.Multiplier(0.5);
	m_Knob2.ResetValue(3);
	m_Knob2.ResetValue();
	m_Knob2.Unit(" ms");
	m_Knob2.Size({ 30, 30 });

	m_Knob3.Range({ 1, 300 });
	m_Knob3.Power(2);
	m_Knob3.ResetValue(30);
	m_Knob3.ResetValue();
	m_Knob3.Multiplier(0.5);
	m_Knob3.Unit(" ms");
	m_Knob3.Size({ 30, 30 });

	m_Knob4.Range({ -24, 24 });
	m_Knob4.Unit("dB");
	m_Knob4.ResetValue(0);
	m_Knob4.ResetValue();
	m_Knob4.Size({ 30, 30 });
	m_Knob4.Multiplier(0.5);
	m_Knob4.Decimals(2);

	m_Knob5.Range({ 0, 100 });
	m_Knob5.Multiplier(0.5);
	m_Knob5.ResetValue(100);
	m_Knob5.ResetValue();
	m_Knob5.Unit(" %");
	m_Knob5.Size({ 30, 30 });
	m_Knob5.Decimals(0);

	UpdateParams();

	Background(Theme<C::Channel>::Get());
}

void Dynamics::Update(const Vec4<int>& v)
{
	m_Knob.Position({ Width() - 280, 20 });
	m_Knob4.Position({ Width() - 225, 20 });
	m_Knob2.Position({ Width() - 155, 20 });
	m_Knob3.Position({ Width() - 105, 20 });
	m_Knob5.Position({ Width() - 45, 20 });
	m_Slider.Position({ 12, 93 });
	m_Slider.Size({ (Width() - 24), Height() - 123 });
	Background(Theme<C::Channel>::Get());
	Effect::Update(v);
	UpdateParams();
}

void Dynamics::Render(CommandCollection& d)
{
	Effect::Render(d);
	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	d.Command<Fill>(Theme<C::Divider>::Get());
	d.Command<Quad>(Vec4<int>{(m_Knob4.X() + m_Knob4.Width() + m_Knob2.X()) / 2, 10, 1, 50});
	d.Command<Quad>(Vec4<int>{(m_Knob3.X() + m_Knob3.Width() + m_Knob5.X()) / 2, 10, 1, 50});
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
	if (m_Channels <= c)
		return 0;

	float out = 0;
	if (c != 0)
	{
		float abs = sin;
		myabs(abs);
		if (biggest < abs)
			biggest = abs;
		out = sin * pregain * compressMult * expanderMult * postgain;
	}
	else
	{
		if (counter > 512)
			counter = 0;

		counter++;

		double s = biggest;
		biggest = 0;
		s *= pregain;
		float _absSample = s;
		myabs(_absSample);

		// convert key to dB
		_absSample += DC_OFFSET;	// add DC offset to avoid log( 0 )
		double _absSampledB = lin2db(_absSample); // convert linear -> dB

		// threshold
		double _overdB = _absSampledB - expanderThreshhold;
		if (_overdB > 0.0)
			_overdB = 0.0;

		// attack/release
		_overdB += DC_OFFSET; // add DC offset to avoid denormal	
		if (_overdB > expanderEnv)
			expanderEnv = _overdB + attcoef * (expanderEnv - _overdB);
		else
			expanderEnv = _overdB + relcoef * (expanderEnv - _overdB);

		_overdB = expanderEnv - DC_OFFSET; // subtract DC offset

			// transfer function
		double _gr = _overdB * (expanderRatio - 1.0) * mix;
		expanderMult = db2lin(_gr); // convert dB -> linear

		// output gain expander
		s *= expanderMult;

		// Absolute of new sample
		_absSample = s;
		myabs(_absSample);

		// convert key to dB
		_absSample += DC_OFFSET;	// add DC offset to avoid log( 0 )
		_absSampledB = lin2db(_absSample); // convert linear -> dB

		// threshold
		_overdB = _absSampledB - compressThreshhold;
		if (_overdB < 0.0)
			_overdB = 0.0;

		// attack/release
		_overdB += DC_OFFSET; // add DC offset to avoid denormal	
		if (_overdB > compressEnv)
			compressEnv = _overdB + attcoef * (compressEnv - _overdB);
		else
			compressEnv = _overdB + relcoef * (compressEnv - _overdB);
		_overdB = compressEnv - DC_OFFSET;// subtract DC offset

		// transfer function
		_gr = _overdB * (compressRatio - 1.0) * mix;
		compressMult = db2lin(_gr); // convert dB -> linear

		// output gain
		s;
		out = sin * pregain * compressMult * expanderMult * postgain;
	}

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