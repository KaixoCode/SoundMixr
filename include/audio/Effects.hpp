#pragma once
#include "pch.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

template<typename T>
bool is_infinite(const T& value)
{
	// Since we're a template, it's wise to use std::numeric_limits<T>
	//
	// Note: std::numeric_limits<T>::min() behaves like DBL_MIN, and is the smallest absolute value possible.
	//

	T max_value = std::numeric_limits<T>::max();
	T min_value = -max_value;

	return !(min_value <= value && value <= max_value);
}

template<typename T>
bool is_nan(const T& value)
{
	// True if NAN
	return value != value;
}

template<typename T>
bool is_valid(const T& value)
{
	return !is_infinite(value) && !is_nan(value);
}


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
	Effect(int channels, const std::string& name)
		: m_Name(name), m_Channels(0)
	{
		m_Menu.ButtonSize({ 160, 20 });
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, name).Disable();
		m_Div = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 4);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Remove");
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};

		m_Listener += [this](Event::MouseEntered& e)
		{
			m_Hovering = true;
		};
	
		m_Listener += [this](Event::MouseExited& e)
		{
			m_Hovering = false;
		};
	}

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

		m_Div->Color(Theme<C::Divider>::Get());
	}

	virtual float NextSample(float, int) = 0;
	virtual void Channels(int c) { m_Channels = c; LOG("WOOF"); }

	bool Hovering() { return m_Hovering; }

protected:
	int m_Channels;
	bool m_Hovering = false;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div;
	const std::string m_Name = "";
};

class Dynamics : public Effect
{
public:

	Dynamics(int channels)
		: m_Knob(Emplace<KnobSlider>()),
		m_Knob2(Emplace<KnobSlider>()),
		m_Knob3(Emplace<KnobSlider>()),
		m_Knob4(Emplace<KnobSlider>()),
		m_Knob5(Emplace<KnobSlider>()),
		m_Slider(Emplace<DynamicsSlider>()),
		Effect(channels, "Dynamics")
	{

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

		compressRatio = m_Slider.ratio1 >= 0 ? m_Slider.ratio1 / 32.0 + 1 : (-1.0 / (m_Slider.ratio1 - 1.0));
		expanderRatio = m_Slider.ratio2 >= 0 ? m_Slider.ratio2 + 1 : (-1.0 / (m_Slider.ratio2 / 8.0 - 1.0));
		compressThreshhold = m_Slider.threshhold1;
		expanderThreshhold = m_Slider.threshhold2;
		attms = 30 - m_Knob2.SliderValue() * 29.9;;
		attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
		relms = 300 - m_Knob3.SliderValue() * 299;
		relcoef = std::exp(-1.0 / ((relms / 1000.0) * sampleRate));
		pregain = db2lin(-m_Knob.SliderValue() * 48 + 24);
		postgain = db2lin(-m_Knob4.SliderValue() * 48 + 24);
		mix = 1.0 - m_Knob5.SliderValue();

		Background(Theme<C::Channel>::Get());
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
		while (m_Levels.size() < c)
			m_Levels.push_back(0);

		while (m_Peaks.size() < c)
			m_Peaks.push_back(0);
		m_Channels = c;
	}

private:
	KnobSlider& m_Knob;
	KnobSlider& m_Knob2;
	KnobSlider& m_Knob3;
	KnobSlider& m_Knob4;
	KnobSlider& m_Knob5;
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
	double attcoef = std::exp(-1.0 / ((attms / 1000.0) * sampleRate));
	double relcoef = std::exp(-1.0 / ((relms / 1000.0) * sampleRate));

	double pregain = 0;
	double postgain = 0;
	double mix = 0;

	double expanderMult = 1;
	double compressMult = 1;

	int counter = 0;
	double biggest = 0.0;
	double r = 0.9;
	float NextSample(float sin, int c) override
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

};


class EffectsGroup : public Panel
{
public:
	
	EffectsGroup()
	{
		Layout<Layout::Stack>(8);
		AutoResize(false, true);
		m_Listener.Clear();
		m_Listener += [this](Event& e)
		{
			// Make event coords relative to this panel
			if (e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
				e.x -= X(), e.y -= Y();

			m_LayoutManager.AddEvent(e);

			if (e.type == Event::Type::MousePressed)
				m_Pressed = true;

			if (e.type == Event::Type::MouseReleased)
				m_Pressed = false;

			Event _copy = e;
			Event _copy2 = e;

			if (e.type == Event::Type::KeyPressed)
			{
				if (!e.repeat)
					for (auto& _c : m_Effects)
						_c->AddEvent(_copy);
				else
					if (m_Focussed)
						m_Focussed->AddEvent(_copy);

				return;
			}

			// If there was an unfocus event, unfocus the focussed component
			if (m_Focussed && e.type == Event::Type::Unfocused)
			{
				m_Focussed->AddEvent(_copy);
				return;
			}

			if (e.type == Event::Type::MouseEntered)
			{
				if (m_Hovering)
					m_Hovering->AddEvent(_copy);

				return;
			}

			// If the mouse moved or pressed, update the hovering and focussed
			if (e.type == Event::Type::MouseDragged || e.type == Event::Type::MouseMoved || e.type == Event::Type::MousePressed)
				this->Determine(e);

			// Send events to hovering and focussed if it's not the same component.
			if (m_Hovering)
				m_Hovering->AddEvent(_copy);

			if (m_Hovering != m_Focussed && m_Focussed)
				m_Focussed->AddEvent(_copy2);

			// If the mouse is released update the hovering and focussed after sending the events.
			if (e.type == Event::Type::MouseReleased)
				this->Determine(e);
		};

	}

	float NextSample(float a, int ch)
	{
		float out = a;
		int c = m_EffectCount;
		for (int i = 0; i < c; i++)
			out = m_Effects[i]->NextSample(out, ch);
		
		return out;
	}

	void Channels(int channels)
	{
		for (int i = 0; i < m_EffectCount; i++)
			m_Effects[i]->Channels(channels);
		
		m_Channels = channels;
	}

	bool Hovering()
	{
		for (int i = 0; i < m_EffectCount; i++)
			if (m_Effects[i]->Hovering())
				return true;
		return false;
	}

	template<typename T>
	T& Emplace()
	{
		int p = m_EffectCount;
		m_EffectCount = 0;
		auto& _t = m_Effects.emplace_back(std::make_unique<T>(m_Channels));
		_t->Channels(m_Channels);
		m_EffectCount = p + 1;
		return *dynamic_cast<T*>(_t.get());;
	}

	void Update(const Vec4<int>& viewport)
	{		
		m_LayoutManager.Update({ 0, 0, Width(), Height() }, m_Effects); // Also set the cursor
		m_Cursor = m_Pressed && m_LayoutManager.Cursor() == -1 ? GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

		if (m_AutoResizeX)
			Width(m_LayoutManager.BiggestCoords().x);

		if (m_AutoResizeY)
			Height(m_LayoutManager.BiggestCoords().y);

		Component::Update(viewport.Overlap({ X(), Y(), Width(), Height() }).Translate({ X(), Y() }));

		// Only update if the viewport has actual size.
		if (m_Viewport.width == 0 || m_Viewport.height == 0)
			return;

		// Update all the components that lie within the viewport and are visible.
		for (auto& _c : m_Effects)
			if (_c->Visible() &&
				_c->X() + _c->Width() >= m_Viewport.x && _c->Y() + _c->Height() >= m_Viewport.y &&
				_c->X() <= m_Viewport.x + m_Viewport.width && _c->Y() <= m_Viewport.y + m_Viewport.height)
				_c->Update(viewport);
			
	}

	void Render(CommandCollection& d)
	{
		using namespace Graphics;
		d.Command<PushMatrix>();

		d.Command<Translate>(Vec2<int>{ X(), Y() });
		Background(d);

		Component::Render(d);

		// Only render if the viewport has actual size.
		if (m_Viewport.width == 0 || m_Viewport.height == 0)
			return;

		// Render all the components that lie within the viewport and are visible.
		for (auto& _c : m_Effects)
			if (_c->Visible() &&
				_c->X() + _c->Width() >= m_Viewport.x && _c->Y() + _c->Height() >= m_Viewport.y &&
				_c->X() <= m_Viewport.x + m_Viewport.width && _c->Y() <= m_Viewport.y + m_Viewport.height)
				_c->Render(d);
			

		d.Command<PopMatrix>();
	}
private:
	std::vector<std::unique_ptr<Effect>> m_Effects;
	int m_Channels = 0, m_EffectCount = 0;


	void Determine(Event& e)
	{
		// Determine the next hovering
		Component* _nextHover = 0;
		for (auto& _c : m_Effects)
			if (_c->Visible())
				if (_c->WithinBounds({ e.x, e.y }))
					_nextHover = _c.get();

		// If it is different, send MouseEntered and MouseExited events.
		if (_nextHover != m_Hovering)
		{
			if (m_Hovering)
			{
				Event _e{ Event::Type::MouseExited, e.x, e.y };
				m_Hovering->AddEvent(_e);
			}

			if (_nextHover != nullptr)
			{
				Event _e{ Event::Type::MouseEntered, e.x, e.y };
				_nextHover->AddEvent(_e);
			}
			m_Hovering = _nextHover;
		}

		// If MousePressed and Hovering is not focussed, send focus events.
		if (e.type == Event::Type::MousePressed && m_Hovering != m_Focussed)
		{
			if (m_Focussed != nullptr)
			{
				Event _e{ Event::Type::Unfocused };
				m_Focussed->AddEvent(_e);
			}

			m_Focussed = m_Hovering;
			if (m_Focussed != nullptr)
			{
				Event _e{ Event::Type::Focused };
				m_Focussed->AddEvent(_e);
			}
		}
	}
};
