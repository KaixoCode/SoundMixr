#pragma once
#include "pch.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

static inline float db2lin(float db) { // dB to linear
	return powf(10.0f, 0.05f * db);
}

static inline float lin2db(float lin) { // linear to dB
	return 20.0f * log10f(lin);
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

	virtual float NextSample(float) = 0;

	const std::string m_Name = "Dynamics";
};

class Dynamics : public Effect
{
public:


	Dynamics()
		: m_Knob(Emplace<VolumeSliderG<KnobSliderGraphics>>()),
		m_Knob2(Emplace<KnobSlider>()),
		m_Knob3(Emplace<KnobSlider>()),
		m_Knob4(Emplace<VolumeSliderG<KnobSliderGraphics>>()),
		m_Knob5(Emplace<KnobSlider>()),
		m_Slider(Emplace<DynamicsSlider>()),
		Effect("Dynamics")
	{

		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Dynamics").Disable();
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Remove");

		Height(180);
		m_Knob.Size({ 50, 50 });
		m_Knob.SliderMult(0.4);
		
		m_Knob2.SliderRange({ 100, 0 });
		m_Knob2.SliderMult(0.4);
		m_Knob2.Unit(" ms");
		m_Knob2.Size({ 50, 50 });

		m_Knob3.SliderRange({ 100, 0 });
		m_Knob3.SliderMult(0.4);
		m_Knob3.Unit(" ms");
		m_Knob3.Size({ 50, 50 });

		m_Knob4.Size({ 50, 50 });
		m_Knob4.SliderMult(0.4);

		m_Knob5.SliderRange({ 100, 0 });
		m_Knob5.SliderMult(0.4);
		m_Knob5.Unit(" %");
		m_Knob5.Size({ 50, 50 });


		Background(Theme<C::Channel>::Get());

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};
	}

	void Update(const Vec4<int>& v) override
	{
		m_Knob.Position({ 65, Height() - 100 });
		m_Knob2.Position({ 5, Height() - 100 });
		m_Knob3.Position({ 5, Height() - 170 });
		m_Knob4.Position({ Width() - 55, Height() - 100 });
		m_Knob5.Position({ Width() - 55, Height() - 170 });
		m_Slider.Position({ 120, 10 });
		m_Slider.Size({ (Width() - 60) - 120, Height() - 45 });
		Background(Theme<C::Channel>::Get());
		Effect::Update(v);

		compressRatio = m_Slider.ratio1 > 0 ? m_Slider.ratio1 + 1 : -1.0 / (m_Slider.ratio1 - 1.0);
		expanderRatio = m_Slider.ratio2 > 0 ? m_Slider.ratio2 + 1 : -1.0 / (m_Slider.ratio2 - 1.0);
		compressThreshhold = m_Slider.threshhold1;
		expanderThreshhold = m_Slider.threshhold2;
		double newval = 10 - m_Knob2.SliderValue() * 10;
		if (newval != attms)
		{
			attms = newval;
			attcoef = exp(-1000.0 / (attms * sampleRate));
		}		
		newval = 100 - m_Knob3.SliderValue() * 100;
		if (newval != relms)
		{
			relms = newval;
			relcoef = exp(-1000.0 / (relms * sampleRate));
		}

		pregain = m_Knob.Decibels();
		postgain = m_Knob4.Decibels();
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
		d.Command<Text>(&m_Knob2Name, m_Knob2.Position() + Vec2<int>{ m_Knob2.Width() / 2, m_Knob2.Height() + 5 });
		d.Command<Text>(&m_Knob3Name, m_Knob3.Position() + Vec2<int>{ m_Knob3.Width() / 2, m_Knob3.Height() + 5 });
		d.Command<Text>(&m_Knob4Name, m_Knob4.Position() + Vec2<int>{ m_Knob4.Width() / 2, m_Knob4.Height() + 5 });
		d.Command<Text>(&m_Knob5Name, m_Knob5.Position() + Vec2<int>{ m_Knob5.Width() / 2, m_Knob5.Height() + 5 });
		d.Command<TextAlign>(Align::RIGHT, Align::BOTTOM);
		d.Command<Text>(&m_Slider.m_TH2Str, Vec2<int>{115, 10});
		d.Command<Text>(&m_Slider.m_TH1Str, Vec2<int>{115, 25});
		d.Command<Text>(&m_Slider.m_RT2Str, Vec2<int>{115, 40});
		d.Command<Text>(&m_Slider.m_RT1Str, Vec2<int>{115, 55});
		d.Command<PopMatrix>();
	}

	VolumeSliderG<KnobSliderGraphics>& m_Knob;
	KnobSlider& m_Knob2;
	KnobSlider& m_Knob3;
	VolumeSliderG<KnobSliderGraphics>& m_Knob4;
	KnobSlider& m_Knob5;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	DynamicsSlider& m_Slider;
	std::string
		m_Knob1Name = "PreGain",
		m_Knob2Name = "Attack",
		m_Knob3Name = "Release",
		m_Knob4Name = "PostGain",
		m_Knob5Name = "Mix";


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

	/*
	
	PreGain

	Attack Time
	Release Time

	Expander Threshhold
	Expander Ratio

	Compressor Threshhold
	Compressor Ratio

	PostGain

	Mix
	
	*/

	float NextSample(float s) override
	{
		float _absSample = std::fabs(s);

		// convert key to dB
		_absSample += DC_OFFSET;	// add DC offset to avoid log( 0 )
		double _absSampledB = pregain + lin2db(_absSample); // convert linear -> dB

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
		_gr = db2lin(_gr); // convert dB -> linear

		// output gain expander
		s = s * _gr;

		// Absolute of new sample
		_absSample = std::fabs(s);

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
		_gr = db2lin(_gr + postgain); // convert dB -> linear

		// output gain
		s = s * _gr;

		return s;
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

	float NextSample(float a)
	{
		float out = a;
		for (auto& c : Components())
		{
			auto a = dynamic_cast<Effect*>(c.get());
			if (a)
			{
				out = a->NextSample(out);
			}
		}
		return out;
	}

	template<typename T>
	T& Emplace()
	{
		return Container::Emplace<T>();
	}
};
