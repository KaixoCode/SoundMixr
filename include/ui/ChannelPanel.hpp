#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/EndpointPanel.hpp"

using namespace Graphics;

class Slider : public Scrollbar<ScrollbarGraphics::Normal, ScrollbarType::Vertical>
{
public:
	using Parent = Scrollbar<ScrollbarGraphics::Normal, ScrollbarType::Vertical>;

	Slider()
	{
		value.reserve(10);
		Range(Vec2<int>{0, 1000000});
		VisibleRange(10);
		SliderValue(1);

		m_Listener += [this](Event::MouseClicked& e)
		{
			if (m_Counter > 0)
			{
				SliderValue(1);
			}

			if (e.button == Event::MouseButton::LEFT)
			{
				m_Counter = 20;
			}
		};
	}

	int MinBarSize() const override { return Width(); }

	double SliderValue()
	{
		double norm = 1.0 - (Value() / R);
		return std::pow(norm * H, 4);
	}

	void SliderValue(double v)
	{
		double norm = std::powf(v, 0.25) / H;
		int n = static_cast<int>((1.0 - norm) * R);
		Value(n);
	}

	double Decibels()
	{
		return 20 * std::log10(SliderValue());
	}

	void Update(const Vec4<int>& v)
	{
		if (m_Counter > 0)
			m_Counter--;

		char s[10];
		if (Decibels() < - 100)
			value = "-inf";
		else {
			std::sprintf(s, "%.1f", Decibels());
			value = s;
		}
		value += "dB";

		Parent::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		int _p = 6;
		int _h = max((VisibleRange() - Range().start) / (float)(Range().end - Range().start) * Height(), (float)MinBarSize());
		int _y = (Value() - Range().start) / (float)(Range().end - Range().start - VisibleRange()) * -(Height() - _h) + Y() + Height() - _h;

		d.Command<Fill>(Color{ 50, 50, 50, 255 });
		d.Command<Quad>(Position().x + _p * 2, _p, Size().width - _p * 4, _y);

		if (Hovering() && Mouse() >= _y && Mouse() <= _y + _h)
		{
			d.Command<Fill>(Color{ 75, 75, 75, 255 });
			_p = 4;
		}
		else
			d.Command<Fill>(Color{ 55, 55, 55, 255 });

		int _w = Size().width - _p * 2;
		int _he = _h - _p * 2;
		d.Command<Quad>(Position().x + _p, _y + _p, _w, _he);
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Color{ 255, 255, 255, 255 });
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&value, Vec2<int>{Position().x + _p + _w / 2, _y + _p + _he/2});
	};

private:
	const double R = 1000000.0;
	const double H = 1.412536;

	std::string value = "";

	int m_Counter = 0;
};


// Button visuals
class TitleText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{ 33, 33, 33, 255 };
		Color _c2{ 255, 255, 255, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Font>(Fonts::Gidole, 24);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

// Button visuals
class SmollText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{ 33, 33, 33, 255 };
		Color _c2{ 255, 255, 255, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Font>(Fonts::Gidole14, 14);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class CloseButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 8;
		Color _c1{ 33, 33, 33, 255 };
		Color _c2{ 255, 255, 255, 255 };
		if (!b.Disabled() && (b.Hovering()))
			_c1 = Color{ 30, 30, 30, 255 };
		else if (!b.Disabled() && b.Active())
			_c1 = Color{ 25, 25, 25, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Fill>(Color{ 255, 0, 0, 255 });
		d.Command<Quad>(Vec4<int>{b.X() + _padding / 2, b.Y() + b.Height() / 2, b.Width() - _padding, 2}, 45.0f);
		d.Command<Quad>(Vec4<int>{b.X() + _padding / 2, b.Y() + b.Height() / 2, b.Width() - _padding, 2}, 90.0f + 45.0f);
	}
};

class NiceButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{ 33, 33, 33, 255 };
		Color _c2{ 255, 255, 255, 255 };
		if (!b.Disabled() && (b.Hovering() || b.Active()))
			_c1 = Color{ 30, 30, 30, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());

		d.Command<Font>(Fonts::Gidole16, 16);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + 6, b.Y() + b.Height() / 2);
		int _w = 16;
		_padding = 4;
		if (!b.Disabled())
			d.Command<Triangle>(b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w / 2, _w / 2, -90.0f);
	}
};


namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::RIGHT>>;
using MenuButton = Button<BG::Menu, BT::Normal>;

class ChannelPanel : public Panel
{
public:

	ChannelPanel(StereoInputChannel& c)
		: inputChannel(&c), input(true),
		text(Emplace<Button<SmollText, BT::Normal>>([]() {}, c.name, Vec2<int>{100, 24})),
		m_VolumeSlider(Emplace<Slider>()),
		routed(Emplace<Button<ButtonGraphics::Menu, BT::Toggle>>(&m_Routed, "", Vec2<int>{25, 25}))
	{
		Init();
	}

	ChannelPanel(StereoOutputChannel& c)
		: outputChannel(&c), input(false),
		text(Emplace<Button<SmollText, BT::Normal>>([]() {}, c.name, Vec2<int>{100, 24})),
		m_VolumeSlider(Emplace<Slider>()),
		routed(Emplace<Button<ButtonGraphics::Menu, BT::Toggle>>(&m_Routed, "", Vec2<int>{25, 25}))
	{
		Init();
	}

	void Select(StereoInputChannel* s)
	{
		select = true;
		if (input)
		{
			m_Routed = false;
			routed.Disable();
		}
		else 
		{
			m_Routed = s->connected.find(outputChannel->left) != s->connected.end();
			routed.Enable();
		}
		m_SelectedInput = true;
		m_SelectedInputChannel = s;
	}

	void Select(StereoOutputChannel* s)
	{
		select = true;

		if (input)
		{
			m_Routed = inputChannel->connected.find(s->left) != inputChannel->connected.end();
			routed.Enable();
		}
		else
		{
			m_Routed = false;
			routed.Disable();
		}
		m_SelectedInput = false;
		m_SelectedOutputChannel = s;
	}

	void Selected(bool v) { m_Selected = v; }
	bool Selected() { return m_Selected; }

	void Routed(bool v) { m_Routed = v; }
	bool Routed() { return m_Routed; }

	Button<SmollText, BT::Normal>& text;
	Button<ButtonGraphics::Menu, BT::Toggle>& routed;

	bool input;

	union
	{
		StereoInputChannel* inputChannel;
		StereoOutputChannel* outputChannel;
	};

private:

	void Init()
	{
		m_VolumeSlider.Position(Vec2<int>{0, 0});
		text.Disable();

		// Styling of panel
		Width(100);

	}

	void Update(const Vec4<int>& viewport)
	{
		if (select)
			if (m_SelectedInput && !input)
				if (m_Routed)
					if (m_SelectedInputChannel->connected.find(outputChannel->left) == m_SelectedInputChannel->connected.end())
						m_SelectedInputChannel->connected.emplace(outputChannel->left, outputChannel);
					else;
				else
					if (m_SelectedInputChannel->connected.find(outputChannel->left) != m_SelectedInputChannel->connected.end())
						m_SelectedInputChannel->connected.erase(outputChannel->left);
					else;
			else if (!m_SelectedInput && input)
				if (m_Routed)
					if (inputChannel->connected.find(m_SelectedOutputChannel->left) == inputChannel->connected.end())
						inputChannel->connected.emplace(m_SelectedOutputChannel->left, m_SelectedOutputChannel);
					else;
				else
					if (inputChannel->connected.find(m_SelectedOutputChannel->left) != inputChannel->connected.end())
						inputChannel->connected.erase(m_SelectedOutputChannel->left);
					else;

		if (Selected())
			Background(Color{ 26, 26, 26, 255 });
		else
			Background(Color{ 33, 33, 33, 255 });

		// Location of buttons
		text.Position(Vec2<int>{0, Height() - 24});
		routed.Position(Vec2<int>{50, Height() - 50});
		m_VolumeSlider.Size(Vec2<int>{50, Height() - 24});

		if (input)
			inputChannel->volume = m_VolumeSlider.SliderValue();
		else
			outputChannel->volume = m_VolumeSlider.SliderValue();

		Panel::Update(viewport);
	}

	bool select = false;
	bool m_SelectedInput;
	union
	{
		StereoInputChannel* m_SelectedInputChannel;
		StereoOutputChannel* m_SelectedOutputChannel;
	};

	Slider& m_VolumeSlider;

	bool m_Selected = false,
		m_Routed = false;
};