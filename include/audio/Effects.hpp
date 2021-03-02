#pragma once
#include "pch.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

static inline float db2lin(float db) { // dB to linear
	return std::powf(10.0f, 0.05f * db);
}

static inline float lin2db(float lin) { // linear to dB
	return 20.0f * std::log10f(lin);
}

static inline void myabs(float& f)
{
	if (f < 0)
		f = -f;
}

class Effect : public Panel
{
public:
	Effect(const std::string& name)
		: m_Name(name)
	{}

	void Render(CommandCollection& d)
	{
		using namespace Graphics;
		Panel::Render(d);
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Fill>(Theme<C::ChannelSelected>::Get());
		d.Command<Quad>(Vec4<int>{ 0, Height() - 25, Width(), 25 });
		d.Command<Font>(Fonts::Gidole16, 16.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::LEFT, Align::TOP);
		d.Command<Text>(&m_Name, Vec2<int>{ 5, Height() - 2});
		d.Command<PopMatrix>();
	}

	virtual float NextSample(float, int) = 0;

	virtual void Channels(int c)
	{
		m_Channels = c;
	}

protected:
	int m_Channels;

	const std::string m_Name = "Dynamics";
};

class Dynamics : public Effect
{
public:

	Dynamics()
		: m_Knob(Emplace<KnobSlider>()),
		m_Knob2(Emplace<KnobSlider>()),
		m_Knob3(Emplace<KnobSlider>()),
		m_Knob4(Emplace<KnobSlider>()),
		m_Knob5(Emplace<KnobSlider>()),
		m_Slider(Emplace<DynamicsSlider>()),
		Effect("Dynamics")
	{

		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Dynamics").Disable();
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Remove");

		Height(200);
		m_Knob.SliderRange({ 24, -24 });
		m_Knob.ResetValue(0);
		m_Knob.ResetValue();
		m_Knob.Unit("dB");
		m_Knob.Size({ 30, 30 });
		m_Knob.SliderMult(0.4);
		
		m_Knob2.SliderRange({ 30, .1f });
		m_Knob2.SliderMult(0.4);
		m_Knob2.ResetValue(3);
		m_Knob2.ResetValue();
		m_Knob2.Unit(" ms");
		m_Knob2.Size({ 30, 30 });

		m_Knob3.SliderRange({ 300, 1 });
		m_Knob3.ResetValue(30);
		m_Knob3.ResetValue();
		m_Knob3.SliderMult(0.4);
		m_Knob3.Unit(" ms");
		m_Knob3.Size({ 30, 30 });

		m_Knob4.SliderRange({ 24, -24 });
		m_Knob4.Unit("dB");
		m_Knob4.ResetValue(0);
		m_Knob4.ResetValue();
		m_Knob4.Size({ 30, 30 });
		m_Knob4.SliderMult(0.4);

		m_Knob5.SliderRange({ 100, 0 });
		m_Knob5.SliderMult(0.4);
		m_Knob5.ResetValue();
		m_Knob5.Unit(" %");
		m_Knob5.Size({ 30, 30 });


		Background(Theme<C::Channel>::Get());

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};
	}

	void Update(const Vec4<int>& v) override
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

		compressRatio = m_Slider.ratio1 >= 0 ? m_Slider.ratio1 / 32.0 + 1 : (-1.0 / (m_Slider.ratio1 - 1.0));
		expanderRatio = m_Slider.ratio2 >= 0 ? m_Slider.ratio2 + 1 : (-1.0 / (m_Slider.ratio2 / 8.0 - 1.0));
		compressThreshhold = m_Slider.threshhold1;
		expanderThreshhold = m_Slider.threshhold2;
		double newval = 30 - m_Knob2.SliderValue() * 29.9;
		if (newval != attms)
		{
			attms = newval;
			attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
		}		
		newval = 300 - m_Knob3.SliderValue() * 299;
		if (newval != relms)
		{
			relms = newval;
			relcoef = std::exp(-1.0 / ((relms / 1000.0)* sampleRate));
		}

		pregain = db2lin(- m_Knob.SliderValue() * 48 + 24);
		postgain = db2lin(- m_Knob4.SliderValue() * 48 + 24);
		mix = 1.0 - m_Knob5.SliderValue();
	}

	void Render(CommandCollection& d) override
	{
		Effect::Render(d);
		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
		d.Command<Text>(&m_Knob1Name, m_Knob.Position() + Vec2<int>{ m_Knob.Width() / 2, m_Knob.Height() + 5 });
		d.Command<Text>(&m_Knob4Name, m_Knob4.Position() + Vec2<int>{ m_Knob4.Width() / 2, m_Knob4.Height() + 5 });
		d.Command<Fill>(Theme<C::Divider>::Get());
		d.Command<Quad>(Vec4<int>{(m_Knob4.X() + m_Knob4.Width() + m_Knob2.X()) / 2, 10, 1, 50});
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<Text>(&m_Knob2Name, m_Knob2.Position() + Vec2<int>{ m_Knob2.Width() / 2, m_Knob2.Height() + 5 });
		d.Command<Text>(&m_Knob3Name, m_Knob3.Position() + Vec2<int>{ m_Knob3.Width() / 2, m_Knob3.Height() + 5 });
		d.Command<Fill>(Theme<C::Divider>::Get());
		d.Command<Quad>(Vec4<int>{(m_Knob3.X() + m_Knob3.Width() + m_Knob5.X()) / 2, 10, 1, 50});
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<Text>(&m_Knob5Name, m_Knob5.Position() + Vec2<int>{ m_Knob5.Width() / 2, m_Knob5.Height() + 5 });
		d.Command<TextAlign>(Align::LEFT, Align::BOTTOM);
		d.Command<Text>(&m_Slider.m_TH2Str, Vec2<int>{10, 77});
		d.Command<Text>(&m_Slider.m_RT2Str, Vec2<int>{70, 77});
		d.Command<Text>(&m_Slider.m_RT1Str, Vec2<int>{Width() - 110, 77});
		d.Command<TextAlign>(Align::RIGHT, Align::BOTTOM);
		d.Command<Text>(&m_Slider.m_TH1Str, Vec2<int>{Width() - 10, 77});

		int _channels = m_Channels;
		int _x = 12;
		int _rw = Width() - 24;
		int _p = 10;
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

	void Channels(int c) override
	{
		m_Channels = c;
		while (m_Levels.size() < m_Channels)
			m_Levels.push_back(0);

		while (m_Peaks.size() < m_Channels)
			m_Peaks.push_back(0);
	}

private:
	KnobSlider& m_Knob;
	KnobSlider& m_Knob2;
	KnobSlider& m_Knob3;
	KnobSlider& m_Knob4;
	KnobSlider& m_Knob5;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	DynamicsSlider& m_Slider;
	std::string
		m_Knob1Name = "PreGain",
		m_Knob2Name = "Attack",
		m_Knob3Name = "Release",
		m_Knob4Name = "PostGain",
		m_Knob5Name = "Mix";

	std::vector<float> m_Levels;
	std::vector<float> m_Peaks;

public:
	static inline const double DC_OFFSET = 1.0E-25;

	double expanderThreshhold = -50;
	double compressThreshhold = -40;
	double expanderRatio = 8.0 / 1.0;
	double compressRatio = 1.0 / 8.0;

	double expanderEnv = DC_OFFSET;
	double compressEnv = DC_OFFSET;

	double sampleRate = 480000;
	double attms = 1;
	double relms = 100;
	double attcoef = exp(-1000.0 / (attms * sampleRate));
	double relcoef = exp(-1000.0 / (relms * sampleRate));

	double pregain = 0;
	double postgain = 0;
	double mix = 0;

	double expanderMult = 1;
	double compressMult = 1;

	int counter = 0;
	double biggest;
	double r = 0.9;
	float NextSample(float sin, int c) override
	{
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

};


class EffectsGroup : public Panel
{
public:
	
	EffectsGroup()
	{
		Layout<Layout::Stack>(8);
		AutoResize(false, true);
	}

	float NextSample(float a, int ch)
	{
		float out = a;
		for (auto& c : Components())
		{
			auto a = dynamic_cast<Effect*>(c.get());
			if (a)
				out = a->NextSample(out, ch);
		}
		return out;
	}

	void Channels(int channels)
	{
		for (auto& c : Components())
		{
			auto a = dynamic_cast<Effect*>(c.get());
			if (a)
				a->Channels(channels);
		}
		m_Channels = channels;
	}

	template<typename T>
	T& Emplace()
	{
		auto& t = Container::Emplace<T>();
		t.Channels(m_Channels);
		return t;
	}

private:
	int m_Channels = 0;
};
