#pragma once
#include "pch.hpp"
#include "ui/Slider.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Some Graphics --------------------------------- \\
// -------------------------------------------------------------------------- \\

class Themes
{
public:
	enum N
	{
		DARK, 
		LIGHT,
		BLUE,
		RED,

		ITEMS
	};

	static inline N Theme = N::DARK;
};

enum class C
{
	WindowBorder, 
	MainPanel, 

	Channel,
	ChannelSelected,

	Text,
	TextOff,
	TextSmall,

	Divider,

	Scrollbar,
	ScrollbarH,

	Menu,
	MenuB,

	Button,
	ButtonD,
	ButtonH,
	ButtonS,

	MButton,
	MButtonH,
	MButtonS,

	RButton,
	RButtonB,
	RButtonH,
	RButtonHB,
	RButtonS,
	RButtonSB,

	VSlider,

	PSlider,
	PSliderB,
	PSliderS,

	VMeter,
	VMeterFill,
	VMeterFillC1,
	VMeterFillC2,
	VMeterFillC3,
	VMeterIndB,
	VMeterIndD,

	ITEMS
};

constexpr Color THEMES[Themes::N::ITEMS][(int)C::ITEMS] = 
{
	// DARK
	{ 
		Color{ 23, 23, 23, 255 },  	 // WindowBorder, 
		Color{ 40, 40, 40, 255 },	 // MainPanel, 
									 // 
		Color{ 33, 33, 33, 255 },	 // Channel,
		Color{ 26, 26, 26, 255 },	 // ChannelSelected,
									 // 
		Color{ 255, 255, 255, 255 }, // Text,
		Color{ 200, 200, 200, 255 }, // TextOff,
		Color{ 128, 128, 128, 255 }, // TextSmall,
									 // 
		Color{ 51, 51, 51, 255 },    // Divider,
									 // 
		Color{ 55, 55, 55, 255 },    // Scrollbar,
		Color{ 75, 75, 75, 255 },    // ScrollbarH,
									 // 
		Color{ 18, 18, 18, 255 },  	 // Menu,
		Color{ 64, 64, 64, 255 }, 	 // MenuB,
									 // 
		Color{ 0, 0, 0, 0 },     	 // Button,
		Color{ 0, 0, 0, 0 },    	 // ButtonD,
		Color{ 255, 255, 255, 13 },	 // ButtonH,
		Color{ 255, 255, 255, 26 },	 // ButtonS,
									 // 
		Color{ 50, 50, 50, 255 },	 // MButton,
		Color{ 70, 70, 70, 255 },	 // MButtonH,
		Color{ 90, 90, 90, 255 },	 // MButtonS,
									 // 
		Color{ 50, 50, 50, 255 },	 // RButton,
		Color{ 0, 0, 0, 0 },		 // RButtonB,
		Color{ 70, 70, 70, 255 },	 // RButtonH,
		Color{ 30 ,30, 30, 255 },	 // RButtonHB,
		Color{ 80, 80, 80, 255 },	 // RButtonS,
		Color{ 28 ,28, 28, 255 },	 // RButtonSB,
									 // 
		Color{ 100, 100, 100, 255 }, // VSlider,
									 // 
		Color{ 15, 15, 15, 255 },	 // PSlider,
		Color{ 45, 45, 45, 255 },	 // PSliderB,
		Color{ 90, 90, 90, 255 },	 // PSliderS,
									 // 
		Color{ 0, 0, 0, 255 },		 // VMeter,
		Color{ 0, 255, 0, 255 },	 // VMeterFill,
		Color{ 255, 255, 0, 255 },	 // VMeterFillC1,
		Color{ 255, 126, 0, 255 },	 // VMeterFillC2,
		Color{ 255, 0, 0, 255 },	 // VMeterFillC3,
		Color{ 128, 128, 128, 255 }, // VMeterIndB,
		Color{ 64, 64, 64, 255 },	 // VMeterIndD,
	},
	// LIGHT
	{ 
		Color{ 220, 220, 220, 255 }, // WindowBorder, 
		Color{ 255, 255, 255, 255 }, // MainPanel, 
									 // 
		Color{ 245, 245, 245, 255 }, // Channel,
		Color{ 225, 225, 225, 255 }, // ChannelSelected,
									 // 
		Color{ 0, 0, 0, 255 },		 // Text,
		Color{ 50, 50, 50, 255 },	 // TextOff,
		Color{ 100, 100, 100, 255 }, // TextSmall,
									 // 
		Color{ 205, 205, 205, 255 }, // Divider,
									 // 
		Color{ 255, 255, 255, 255 }, // Scrollbar,
		Color{ 240, 240, 240, 255 }, // ScrollbarH,
									 // 
		Color{ 255, 255, 255, 255 }, // Menu,
		Color{ 170, 170, 170, 255 }, // MenuB,
									 // 
		Color{ 0, 0, 0, 0 },     	 // Button,
		Color{ 0, 0, 0, 0 },    	 // ButtonD,
		Color{ 0, 0, 0, 13 },		 // ButtonH,
		Color{ 0, 0, 0, 26 },		 // ButtonS,
									 // 
		Color{ 190, 190, 190, 255 }, // MButton,
		Color{ 175, 175, 175, 255 }, // MButtonH,
		Color{ 160, 160, 160, 255 }, // MButtonS,
									 // 
		Color{ 200, 200, 200, 255 }, // RButton,
		Color{ 0, 0, 0, 0 },		 // RButtonB,
		Color{ 180, 180, 180, 255 }, // RButtonH,
		Color{ 225, 225, 225, 255 }, // RButtonHB,
		Color{ 160, 160, 160, 255 }, // RButtonS,
		Color{ 220, 220, 220, 255 }, // RButtonSB,
									 // 
		Color{ 150, 150, 150, 255 }, // VSlider,
									 // 
		Color{ 245, 245, 245, 255 }, // PSlider,
		Color{ 128, 128, 128, 255 }, // PSliderB,
		Color{ 180, 180, 180, 255 }, // PSliderS,
									 // 
		Color{ 255, 255, 255, 255 }, // VMeter,
		Color{ 0, 255, 0, 255 },	 // VMeterFill,
		Color{ 255, 255, 0, 255 },	 // VMeterFillC1,
		Color{ 255, 126, 0, 255 },	 // VMeterFillC2,
		Color{ 255, 0, 0, 255 },	 // VMeterFillC3,
		Color{ 180, 180, 180, 255 }, // VMeterIndB,
		Color{ 128, 128, 128, 255 }, // VMeterIndD,
	},
	// BLUE
	{ 
		Color{  44,  61,  77, 255 }, // WindowBorder, 
		Color{  38,  52,  66, 255 }, // MainPanel, 
									 // 
		Color{  44,  61,  77, 255 }, // Channel,
		Color{  50,  69,  86, 255 }, // ChannelSelected,
									 // 
		Color{ 126, 157, 181, 255 }, // Text,
		Color{ 111, 140, 160, 255 }, // TextOff,
		Color{ 111, 140, 160, 255 }, // TextSmall,
									 // 
		Color{  44,  61,  77, 255 }, // Divider,
									 // 
		Color{  50,  69,  86, 255 }, // Scrollbar,
		Color{  95, 120, 137, 255 }, // ScrollbarH,
									 // 
		Color{  16,  33,  43, 255 }, // Menu,
		Color{  50,  69,  86, 255 }, // MenuB,
									 // 
		Color{ 0, 0, 0, 0 },     	 // Button,
		Color{ 0, 0, 0, 0 },    	 // ButtonD,
		Color{ 255, 255, 255, 13 },	 // ButtonH,
		Color{ 255, 255, 255, 26 },	 // ButtonS,
									 // 
		Color{  38,  52,  66, 255 }, // MButton,
		Color{  64,  80,  91, 255 }, // MButtonH,
		Color{  99, 125, 142, 255 }, // MButtonS,
									 // 
		Color{  95, 120, 137, 255 }, // RButton,
		Color{  0,    0,   0,   0 }, // RButtonB,
		Color{  95, 120, 137, 255 }, // RButtonH,
		Color{  50,  69,  86, 255 }, // RButtonHB,
		Color{ 111, 140, 160, 255 }, // RButtonS,
		Color{  50,  69,  86, 255 }, // RButtonSB,
									 // 
		Color{ 111, 140, 160, 255 }, // VSlider,
									 // 
		Color{  38,  52,  66, 255 }, // PSlider,
		Color{  38,  52,  66, 255 }, // PSliderB,
		Color{  99, 125, 142, 255 }, // PSliderS,
									 // 
		Color{  18,  32,  41, 255 }, // VMeter,
		Color{ 0, 255, 0, 255 },	 // VMeterFill,
		Color{ 255, 255, 0, 255 },	 // VMeterFillC1,
		Color{ 255, 126, 0, 255 },	 // VMeterFillC2,
		Color{ 255, 0, 0, 255 },	 // VMeterFillC3,
		Color{ 180, 180, 180, 255 }, // VMeterIndB,
		Color{ 128, 128, 128, 255 }, // VMeterIndD,
	},
	// RED
	{
		Color{  77,  61,  44, 255 }, // WindowBorder, 
		Color{  66,  52,  38, 255 }, // MainPanel, 
										// 
		Color{  77,  61,  44, 255 }, // Channel,
		Color{  86,  69,  50, 255 }, // ChannelSelected,
										// 
		Color{ 181, 157, 126, 255 }, // Text,
		Color{ 160, 140, 111, 255 }, // TextOff,
		Color{ 160, 140, 111, 255 }, // TextSmall,
										// 
		Color{ 116, 101,  82, 255 }, // Divider,
										// 
		Color{  43,  33,  16, 255 }, // Menu,
		Color{  86,  69,  50, 255 }, // MenuB,
										// 
		Color{   0,   0,   0, 0 },     	 // Button,
		Color{   0,   0,   0, 0 },    	 // ButtonD,
		Color{ 255, 255, 255, 13 },	 // ButtonH,
		Color{ 255, 255, 255, 26 },	 // ButtonS,
										// 
		Color{  66,  52,  38, 255 }, // MButton,
		Color{  91,  80,  64, 255 }, // MButtonH,
		Color{ 142, 125,  99, 255 }, // MButtonS,
										// 
		Color{ 137, 120,  95, 255 }, // RButton,
		Color{   0,   0,   0,   0 }, // RButtonB,
		Color{ 137, 120,  95, 255 }, // RButtonH,
		Color{  86,  69,  50, 255 }, // RButtonHB,
		Color{ 160, 140, 111, 255 }, // RButtonS,
		Color{  86,  69,  50, 255 }, // RButtonSB,
										// 
		Color{ 160, 140, 111, 255 }, // VSlider,
										// 
		Color{   66,  52, 38, 255 }, // PSlider,
		Color{   66,  52, 38, 255 }, // PSliderB,
		Color{  142, 125, 99, 255 }, // PSliderS,
										// 
		Color{  41,  32,  18, 255 }, // VMeter,
		Color{   0, 255, 0, 255 },	 // VMeterFill,
		Color{ 255, 255, 0, 255 },	 // VMeterFillC1,
		Color{ 255, 126, 0, 255 },	 // VMeterFillC2,
		Color{ 255, 0, 0, 255 },	 // VMeterFillC3,
		Color{ 180, 180, 180, 255 }, // VMeterIndB,
		Color{ 128, 128, 128, 255 }, // VMeterIndD,
	}
};

template<C T>
class Theme
{
public:
	static Color Get() { return THEMES[(int)Themes::Theme][(int)T]; };
};

class TitleText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1 = Theme<C::Channel>::Get();
		Color _c2 = Theme<C::Text>::Get();
		
		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Font>(Fonts::Gidole, 24.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class SmallText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		Color _c2 = Theme<C::TextSmall>::Get();

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class RouteButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		double _div = 3;
		Color _c1 = Theme<C::RButtonB>::Get();
		Color _c2 = Theme<C::RButton>::Get();
		if ((b.Hovering()))
		{
			_c1 = Theme<C::RButtonHB>::Get();
			_c2 = Theme<C::RButtonH>::Get();
		}
		if (b.Active())
		{
			_c1 = Theme<C::RButtonSB>::Get();
			_c2 = Theme<C::RButtonS>::Get();
			_div = 2.7;
		}

		int _w = b.Width() / _div;
		int _h = 2 * b.Height() / _div;
		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Fill>(_c2);
		d.Command<Triangle>(Vec4<int>{b.Position() + (b.Size() / 2), _w, _h }, b.Name() == "in" ? -90.0f : 90.0f);
	}
};

class MonoButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		Color _c1 = Theme<C::TextOff>::Get();
		Color _c2 = Theme<C::MButton>::Get();
		if ((b.Hovering()))
		{
			_c2 = Theme<C::MButtonH>::Get();
		}
		if (b.Active())
		{
			_c1 = Theme<C::Text>::Get();
			_c2 = Theme<C::MButtonS>::Get();
		}

		int _w = b.Width();
		int _h = b.Height();
		d.Command<Fill>(_c2);
		d.Command<Quad>(Vec4<int>{b.Position(), _w, _h });
		d.Command<Font>(Fonts::Gidole16, 11.0f);
		d.Command<Fill>(_c1);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2 + 2, b.Y() + b.Height() / 2);
	}
};

class MuteButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		Color _c1 = Theme<C::TextOff>::Get();
		Color _c2 = Theme<C::MButton>::Get();
		if ((b.Hovering()))
		{
			_c2 = Theme<C::MButtonH>::Get();
		}
		if (b.Active())
		{
			_c1 = Theme<C::Text>::Get();
			_c2 = Theme<C::MButtonS>::Get();
		}

		int _w = b.Width();
		int _h = b.Height();
		d.Command<Fill>(_c2);
		d.Command<Quad>(Vec4<int>{b.Position(), _w, _h });
		d.Command<Font>(Fonts::Gidole16, 11.0f);
		d.Command<Fill>(_c1);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2 + 2, b.Y() + b.Height() / 2);
	}
};

class VolumeSliderGraphics
{
public:

	static void Render(SliderBase<VolumeSliderGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		if (b.Vertical())
		{
			int _p = 6;
			int _h = 25;
			int _y = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start) * -(b.Height()) + b.Y() + b.Height();

			d.Command<Fill>(Theme<C::VSlider>::Get());

			int _w = b.Width() - _p * 2;
			int _he = _h - _p * 2;
			d.Command<Triangle>(Vec4<int>{b.X() + _p, _y + _p, 8, _he}, 0.0f);
			d.Command<Triangle>(Vec4<int>{b.X() - _p + b.Width(), _y + _p, 8, _he}, 180.0f);
			d.Command<Quad>(Vec4<int>{b.X() + _p, _y + _p - 1, b.Width() - _p * 2, 3});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(Theme<C::TextSmall>::Get());
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X() + _p + _w / 2, b.Y()});
		}
		else
		{
			int _p = 6;
			int _w = 25;
			int _x = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start) * -(b.Width()) + b.X() + b.Width();

			d.Command<Fill>(Theme<C::VSlider>::Get());

			int _h = b.Height() - _p * 2;
			int _we = _w - _p * 2;
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() + _p, _we, 8}, 90.0f);
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() - _p + b.Height(), _we, 8}, 270.0f);
			d.Command<Quad>(Vec4<int>{_x + _p - 1, b.Y() + _p, 3, b.Height() - _p * 2});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(Theme<C::TextSmall>::Get());
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X(), b.Y() + _p + _h / 2});
		}
	}
};

class PanSliderGraphics
{
public:

	static void Render(SliderBase<PanSliderGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;
		int _w = -(b.SliderValue() / 50.0) * (b.Width() * 0.5 - 1);

		int _h = b.Height() - _p * 2;
		int _we = _w - _p * 2;
		d.Command<Fill>(Theme<C::PSliderB>::Get());
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		d.Command<Fill>(Theme<C::PSlider>::Get());
		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		d.Command<Fill>(Theme<C::PSliderS>::Get());
		d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2, b.Y() + 1, _w, b.Height() - 2});

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.ValueText(), b.Position() + (b.Size() / 2));
	}
};

namespace SoundMixrGraphics
{
	using namespace Graphics;
	struct Menu
	{
		template<typename Type = ButtonBase>
		static void Render(Type& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? Theme<C::ButtonD>::Get() : b.Active() ? Theme<C::ButtonS>::Get() : b.Hovering() ? Theme<C::ButtonH>::Get() : Theme<C::Button>::Get();
			Color _c2 = Theme<C::Text>::Get() *(Graphics::WindowFocused() ? 1.0f : 0.8f);
			Color _c3 = Theme<C::TextSmall>::Get() *(Graphics::WindowFocused() ? 1.0f : 0.8f);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<Fill>(_c2);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);
			d.Command<Fill>(_c3);
			d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
			if (b.KeyCombo() != Key::NONE)
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
		};

		template<>
		static void Render<Button<Menu, ButtonType::List>>(Button<Menu, ButtonType::List>& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? Theme<C::ButtonD>::Get() : b.Active() ? Theme<C::ButtonS>::Get() : b.Hovering() ? Theme<C::ButtonH>::Get() : Theme<C::Button>::Get();
			Color _c2 = Theme<C::Text>::Get() *(Graphics::WindowFocused() ? 1.0f : 0.8f);
			Color _c3 = Theme<C::TextSmall>::Get() *(Graphics::WindowFocused() ? 1.0f : 0.8f);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());

			if (b.Selected())
			{
				d.Command<Fill>(Theme<C::ButtonS>::Get());
				int _p = 2;
				d.Command<Quad>(b.Position() + Vec2<int>{ _p, _p }, Vec2<int>{ b.Size().y - _p * 2, b.Size().y - _p * 2 });
			}
			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<Fill>(_c2);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);
			d.Command<Fill>(_c3);
			d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
			if (b.KeyCombo() != Key::NONE)
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
		};

		template<>
		static void Render<Button<Menu, ButtonType::Toggle>>(Button<Menu, ButtonType::Toggle>& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? Theme<C::ButtonD>::Get() : b.Hovering() ? Theme<C::ButtonH>::Get() : Theme<C::Button>::Get();
			Color _c2 = Theme<C::Text>::Get() * (Graphics::WindowFocused() ? 1.0f : 0.8f);
			Color _c3 = Theme<C::TextSmall>::Get() * (Graphics::WindowFocused() ? 1.0f : 0.8f);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());

			if (b.Active())
			{
				d.Command<Fill>(Theme<C::ButtonS>::Get());
				int _p = 2;
				d.Command<Quad>(b.Position() + Vec2<int>{ _p, _p }, Vec2<int>{ b.Size().y - _p * 2, b.Size().y - _p * 2 });
			}
			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<Fill>(_c2);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);
			d.Command<Fill>(_c3);
			d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
			if (b.KeyCombo() != Key::NONE)
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
		};

		template<>
		static void Render<Button<Menu, ButtonType::Hover>>(Button<Menu, ButtonType::Hover>& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? Theme<C::ButtonD>::Get() : b.Hovering() ? Theme<C::ButtonH>::Get() : Theme<C::Button>::Get();
			Color _c2 = Theme<C::Text>::Get() * (Graphics::WindowFocused() ? 1.0f : 0.8f);
			Color _c3 = Theme<C::TextSmall>::Get() * (Graphics::WindowFocused() ? 1.0f : 0.8f);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Font>(Fonts::Gidole14, 14);
			d.Command<Fill>(_c2);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);
			d.Command<Fill>(_c3);
			d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
			if (b.KeyCombo() != Key::NONE)
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
		};
	};

	// --------------------------------------------------------------------------
	// ------------------------------ Sub Menu ----------------------------------
	// --------------------------------------------------------------------------

	/**
	 * Button graphics for a submenu, which is a button that, when hovered over, opens a menu.
	 */
	struct SubMenu
	{
		/**
		 * Render
		 * @param b the <code>Button</code>
		 * @param d the <code>CommandCollection</code>
		 */
		template<typename Type = ButtonBase>
		static void Render(Type& b, CommandCollection& d)
		{
			Menu::Render(b, d);
			int _w = 8;
			int _padding = 4;
			d.Command<Triangle>(b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w / 2, _w, 0.0f);
		}
	};

	// --------------------------------------------------------------------------
	// ----------------------------- Title Menu ---------------------------------
	// --------------------------------------------------------------------------

	/**
	 * Button graphics used for the buttons in the title bar menu.
	 */
	struct TitleMenu
	{
		/**
		 * Render
		 * @param b the <code>Button</code>
		 * @param d the <code>CommandCollection</code>
		 */    
		static void TitleMenu::Render(ButtonBase& b, CommandCollection& d)
		 {
			 Color _c1 = b.Active() ? Theme<C::Menu>::Get() : b.Hovering() ? Theme<C::ButtonH>::Get() : Theme<C::Button>::Get();
			 Color _c2 = Theme<C::Text>::Get();

			 int _padding = 20;
			 if (b.Active())
			 {
				 d.Command<Fill>(Theme<C::MenuB>::Get());
				 d.Command<Quad>(b.X() - 1, b.Y(), b.Width() + 2, b.Height());
			 }
			 d.Command<Fill>(_c1);
			 d.Command<Quad>(b.X(), b.Y(), b.Width(), b.Height() - 1);
			 d.Command<Fill>(_c2);
			 d.Command<Font>(Fonts::Gidole14, 14.0f);
			 d.Command<TextAlign>(Align::CENTER, Align::CENTER);
			 d.Command<Text>(&b.Name(), Vec2<int>{b.X() + b.Width() / 2, b.Y() + b.Height() / 2});
		 }
	}; 

	/**
	 * Horizontal menu.
	 */
	struct Horizontal
	{
		static void Render(MenuBase& b, CommandCollection& d) 
		{
			auto& _menuItems = b.Components();

			int _x = b.X();
			int _y = b.Y();
			int _ix = b.X();
			int _mh = 0;

			for (auto& _item : _menuItems)
			{
				if (!_item->Visible())
					continue;

				_item->Position({ _x, _y });
				_x += _item->Width();
				if (_item->Height() > _mh)
					_mh = _item->Height();
			}

			b.Size({ _x - _ix, _mh });
		}
	};

	// --------------------------------------------------------------------------
	// ------------------------------ Vertical ----------------------------------
	// --------------------------------------------------------------------------

	/**
	 * Vertical menu.
	 */
	struct Vertical
	{
		static void Render(MenuBase& b, CommandCollection& d) 
		{
			auto& _menuItems = b.Components();

			int _padding = 2;
			int _x = b.X();
			int _y = b.Y() + b.Height();
			int _iy = _y;
			int _mw = 0;

			d.Command<Fill>(Theme<C::MenuB>::Get());
			d.Command<Quad>(b.X() - 1, b.Y() - 1, b.Width() + 2, b.Height() + 2);

			d.Command<Fill>(Theme<C::Menu>::Get());
			d.Command<Quad>(b.X(), b.Y(), b.Width(), b.Height());

			for (auto& _item : _menuItems)
			{
				if (!_item->Visible())
					continue;

				_y -= _item->Height();
				_item->Position({ _x + _padding, _y - _padding });
				if (_item->Width() > _mw)
					_mw = _item->Width();
			}

			b.Size({ _mw + _padding * 2, -_y + _iy + _padding * 2 });
		}
	};

	/**
	 * Simple normal scrollbar graphics.
	 */
	struct ScrollbarNormal
	{
		/**
		 * Render
		 * @param b the <code>Scrollbar</code>
		 * @param d the <code>CommandCollection</code>
		 */
		template<typename ScrollbarType>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType>& b, CommandCollection& d)
		{}

		template<>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType::Horizontal>& b, CommandCollection& d)
		{
			int _p = 6;
			int _w = std::max((b.VisibleRange() - b.Range().start) / (float)(b.Range().end - b.Range().start) * b.Width(), (float)b.MinBarSize());
			int _x = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start - b.VisibleRange()) * (b.Width() - _w) + b.X();

			if (b.Hovering() && b.Mouse() >= _x && b.Mouse() <= _x + _w)
			{
				d.Command<Fill>(Theme<C::Scrollbar>::Get());
				d.Command<Quad>(b.Position().x, b.Position().y + b.Size().height / 2 - 1, b.Size().width, 2);
				d.Command<Fill>(Theme<C::ScrollbarH>::Get());
				_p = 4;
			}
			else
				d.Command<Fill>(Theme<C::Scrollbar>::Get());

			d.Command<Quad>(_x + _p, b.Position().y + _p, _w - _p * 2, b.Size().height - _p * 2);
		}

		template<>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType::Vertical>& b, CommandCollection& d)
		{
			int _p = 6;
			int _h = std::max((b.VisibleRange() - b.Range().start) / (float)(b.Range().end - b.Range().start) * b.Height(), (float)b.MinBarSize());
			int _y = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start - b.VisibleRange()) * -(b.Height() - _h) + b.Y() + b.Height() - _h;

			if (b.Hovering() && b.Mouse() >= _y && b.Mouse() <= _y + _h)
			{
				d.Command<Fill>(Theme<C::Scrollbar>::Get());
				d.Command<Quad>(b.Position().x + b.Size().width / 2 - 1, b.Position().y, 2, b.Size().height);
				d.Command<Fill>(Theme<C::ScrollbarH>::Get());
				_p = 4;
			}
			else
				d.Command<Fill>(Theme<C::Scrollbar>::Get());

			d.Command<Quad>(b.Position().x + _p, _y + _p, b.Size().width - _p * 2, _h - _p * 2);

		}
	};
}

