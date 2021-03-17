#pragma once
#include "pch.hpp"
#include "ui/Parameter.hpp"

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

	KnobSlider,
	KnobSliderB,
	KnobSliderV,
	KnobSliderS,

	Slider,
	SliderB,
	SliderV,

	ToggleButton,
	ToggleButtonB,
	ToggleButtonV,
	ToggleButtonH,
	ToggleButtonText,

	Dynamics,
	DynamicsB,
	DynamicsL,

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
		Color{ 29, 29, 29, 255 },	 // ChannelSelected,
									 // 
		Color{ 255, 255, 255, 255 }, // Text,
		Color{ 128, 128, 128, 255 }, // TextOff,
		Color{ 200, 200, 200, 255 }, // TextSmall,
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
		Color{ 0, 0, 0, 0 },	     // RButtonHB,
		Color{ 100, 100, 100, 255 },	 // RButtonS,
		Color{ 0, 0, 0, 0 },	     // RButtonSB,
									 // 
		Color{ 100, 100, 100, 255 }, // VSlider,
									 // 
		Color{ 29, 29, 29, 255 },	 // PSlider,
		Color{ 23, 23, 23, 255 },	 // PSliderB,
		Color{ 65, 65, 65, 255 },	 // PSliderS,
									 // 
		Color{ 15, 15, 15, 255 },    // VMeter,
		Color{ 0, 255, 0, 255 },	 // VMeterFill,
		Color{ 255, 255, 0, 255 },	 // VMeterFillC1,
		Color{ 255, 126, 0, 255 },	 // VMeterFillC2,
		Color{ 255, 0, 0, 255 },	 // VMeterFillC3,
		Color{ 128, 128, 128, 255 }, // VMeterIndB,
		Color{ 64, 64, 64, 255 },	 // VMeterIndD,
									 // 
		Color{ 33, 33, 33, 255 },	 // KnobSlider,
		Color{ 45, 45, 45, 255 },	 // KnobSliderB,
		Color{ 120, 120, 120, 255 }, // KnobSliderV,
		Color{ 85, 85, 85, 255 },    // KnobSliderS,
									 // 
		Color{ 29, 29, 29, 255 },	 // Slider,
		Color{ 23, 23, 23, 255 },    // SliderB,
		Color{ 65, 65, 65, 255 },    // SliderV,
									 // 
		Color{ 29, 29, 29, 255 },	 // ToggleButton,
		Color{ 23, 23, 23, 255 },    // ToggleButtonB,
		Color{ 65, 65, 65, 255 },    // ToggleButtonV,
		Color{ 40, 40, 40, 255 },    // ToggleButtonH,
		Color{ 255, 255, 255, 255 }, // ToggleButtonText,
									 // 
		Color{ 15, 15, 15, 255 },	 // Dynamics,
		Color{ 100, 100, 100, 50 },	 // DynamicsB,
		Color{ 100, 100, 100, 100 }, // DynamicsL,
									 // 
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
									 // 
		Color{ 245, 245, 245, 255 }, // KnobSlider,
		Color{ 225, 225, 225, 255 }, // KnobSliderB,
		Color{ 180, 180, 180, 255 }, // KnobSliderV,
		Color{ 195, 195, 195, 255 }, // KnobSliderS,
									 // 
		Color{ 245, 245, 245, 255 }, // Slider,
		Color{ 225, 225, 225, 255 }, // SliderB,
		Color{ 180, 180, 180, 255 }, // SliderV,
									 // 
		Color{ 245, 245, 245, 255 }, // ToggleButton,
		Color{ 225, 225, 225, 255 }, // ToggleButtonB,
		Color{ 180, 180, 180, 255 }, // ToggleButtonV,
		Color{ 180, 180, 180, 255 }, // ToggleButtonH,
		Color{ 180, 180, 180, 255 }, // ToggleButtonText,
									 // 
		Color{ 255, 255, 255, 255 }, // Dynamics,
		Color{ 0, 0, 0, 20 },		 // DynamicsB,
		Color{ 0, 0, 0, 20 },		 // DynamicsL,
									 // 
	},
	// BLUE
	{ 
		Color{  44,  61,  77, 255 }, // WindowBorder, 
		Color{  38,  52,  66, 255 }, // MainPanel, 
									 // 
		Color{  44,  61,  77, 255 }, // Channel,
		Color{  50,  69,  86, 255 }, // ChannelSelected,
									 // 
		Color{ 190, 206, 218, 255 }, // Text,
		Color{ 109, 135, 156, 255 }, // TextOff,
		Color{ 158, 181, 199, 255 }, // TextSmall,
									 // 
		Color{  50,  69,  86, 255 }, // Divider,
									 // 
		Color{  50,  69,  86, 255 }, // Scrollbar,
		Color{  95, 120, 137, 255 }, // ScrollbarH,
									 // 
		Color{  16,  33,  43, 255 }, // Menu,
		Color{  50,  69,  86, 255 }, // MenuB,
									 // 
		Color{ 0, 0, 0, 0 },     	 // Button,
		Color{ 0, 0, 0, 0 },    	 // ButtonD,
		Color{ 190, 206, 218, 13 },	 // ButtonH,
		Color{ 190, 206, 218, 26 },	 // ButtonS,
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
									 // 
		Color{  44,  61,  77, 255 }, // KnobSlider,
		Color{  50,  69,  86, 255 }, // KnobSliderB,
		Color{ 126, 157, 181, 255 }, // KnobSliderV,
		Color{  90, 116, 132, 255 }, // KnobSliderS,
									 // 
		Color{  38,  52,  66, 255 }, // Slider,
		Color{  44,  61,  77, 255 }, // SliderB,
		Color{  99, 125, 142, 255 }, // SliderV,
									 // 
		Color{  38,  52,  66, 255 }, // ToggleButton,
		Color{  44,  61,  77, 255 }, // ToggleButtonB,
		Color{  99, 125, 142, 255 }, // ToggleButtonV,
		Color{  50,  69,  86, 255 }, // ToggleButtonH,
		Color{ 190, 206, 218, 255 }, // ToggleButtonText,
									 // 
		Color{   18,  32,  41, 255 },// Dynamics,
		Color{  200, 230, 255, 50 }, // DynamicsB,
		Color{  200, 230, 255, 100 },// DynamicsL,
									 // 
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

class Theme
{
public:
	Themes::N theme;

	Color Get(C c) 
	{ 
		return THEMES[theme][(int)c];
	};
};

extern Theme* theme;

struct NOTHING
{
	static void Render(ButtonBase&, CommandCollection&) {}
};

class TitleText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1 = theme->Get(C::Channel);
		Color _c2 = theme->Get(C::Text);
		
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
		Color _c2 = theme->Get(C::TextSmall);

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

template<Align Xa = Align::CENTER, Align Ya = Align::CENTER>
class TextComponent : public Component
{
public:
	TextComponent(const std::string& t, Graphics::Fonts f = Graphics::Fonts::Gidole14, float fsize = 14)
		: m_Text(t), m_Font(f), m_FontSize(fsize)
	{}

	void Render(CommandCollection& d) override
	{
		using namespace Graphics;
		Color _c2 = theme->Get(C::TextSmall);

		d.Command<Font>(m_Font, m_FontSize);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Xa, Ya);
		d.Command<Text>(&m_Text, X() + Width() / 2, Y() + Height() / 2);
		Component::Render(d);
	}

private:
	std::string m_Text;
	float m_FontSize;
	Graphics::Fonts m_Font;
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
		Color _c1 = theme->Get(C::RButtonB);
		Color _c2 = theme->Get(C::RButton);
		if ((b.Hovering()))
		{
			_c1 = theme->Get(C::RButtonHB);
			_c2 = theme->Get(C::RButtonH);
		}
		if (b.Active())
		{
			_c1 = theme->Get(C::RButtonSB);
			_c2 = theme->Get(C::RButtonS);
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
		Color _c1 = theme->Get(C::TextOff);
		Color _c2 = theme->Get(C::MButton);
		if ((b.Hovering()))
		{
			_c2 = theme->Get(C::MButtonH);
		}
		if (b.Active())
		{
			_c1 = theme->Get(C::Text);
			_c2 = theme->Get(C::MButtonS);
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
		Color _c1 = theme->Get(C::TextOff);
		Color _c2 = theme->Get(C::MButton);
		if ((b.Hovering()))
		{
			_c2 = theme->Get(C::MButtonH);
		}
		if (b.Active())
		{
			_c1 = theme->Get(C::Text);
			_c2 = theme->Get(C::MButtonS);
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

	static void Render(Parameter<VolumeSliderGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		if (b.Vertical())
		{
			int _p = 6;
			int _h = 25;
			int _y = b.NormalizedValue() * b.Height() + b.Y();

			d.Command<Fill>(theme->Get(C::VSlider));

			int _w = b.Width() - _p * 2;
			int _he = _h - _p * 2;
			d.Command<Triangle>(Vec4<int>{b.X() + _p, _y + _p, 8, _he}, 0.0f);
			d.Command<Triangle>(Vec4<int>{b.X() - _p + b.Width(), _y + _p, 8, _he}, 180.0f);
			d.Command<Quad>(Vec4<int>{b.X() + _p, _y + _p - 1, b.Width() - _p * 2, 3});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(theme->Get(C::TextSmall));
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X() + _p + _w / 2, b.Y()});
		}
		else
		{
			int _p = 6;
			int _w = 25;
			int _x = b.NormalizedValue() * (b.Width()) + b.X();

			d.Command<Fill>(theme->Get(C::VSlider));

			int _h = b.Height() - _p * 2;
			int _we = _w - _p * 2;
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() + _p, _we, 8}, 90.0f);
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() - _p + b.Height(), _we, 8}, 270.0f);
			d.Command<Quad>(Vec4<int>{_x + _p - 1, b.Y() + _p, 3, b.Height() - _p * 2});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(theme->Get(C::TextSmall));
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X(), b.Y() + _p + _h / 2});
		}
	}
};

class PanSliderGraphics
{
public:

	static void Render(Parameter<PanSliderGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;
		int _w = (b.NormalizedValue() - 0.5) * (b.Width() - 1.0);

		int _h = b.Height() - _p * 2;
		int _we = _w - _p * 2;
		d.Command<Fill>(theme->Get(C::PSliderB));
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		d.Command<Fill>(theme->Get(C::PSlider));
		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		d.Command<Fill>(theme->Get(C::PSliderS));
		d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2, b.Y() + 1, _w, b.Height() - 2});

		d.Command<Font>(Fonts::Gidole14, 14.0f);		
		if (b.Disabled())
			d.Command<Fill>(theme->Get(C::TextOff));
		else
			d.Command<Fill>(theme->Get(C::TextSmall));
		if (b.DisplayValue())
		{
			d.Command<TextAlign>(Align::CENTER, Align::CENTER);
			d.Command<Text>(&b.ValueText(), b.Position() + (b.Size() / 2));
		}
		if (b.DisplayName())
		{
			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + 5 });
		}
	}
};


class SliderGraphics
{
public:

	static void Render(Parameter<SliderGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Vertical())
		{
			int _p = 6;
			int _h = b.NormalizedValue() * (b.Height() - 2);

			d.Command<Fill>(theme->Get(C::SliderB));
			d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
			d.Command<Fill>(theme->Get(C::Slider));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
			d.Command<Fill>(theme->Get(C::SliderV));
			d.Command<Quad>(Vec4<int>{b.X() + 1, b.Y() + 1, b.Width() - 2, _h});
		}
		else
		{
			int _p = 6;
			int _w = b.NormalizedValue() * (b.Width() - 2);

			d.Command<Fill>(theme->Get(C::SliderB));
			d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
			d.Command<Fill>(theme->Get(C::Slider));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
			d.Command<Fill>(theme->Get(C::SliderV));
			d.Command<Quad>(Vec4<int>{b.X() + 1, b.Y() + 1, _w, b.Height() - 2});
		}

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		if (b.Disabled())
			d.Command<Fill>(theme->Get(C::TextOff));
		else
			d.Command<Fill>(theme->Get(C::TextSmall));
		if (b.DisplayValue())
		{
			d.Command<TextAlign>(Align::CENTER, Align::CENTER);
			d.Command<Text>(&b.ValueText(), Vec2<int>{ b.X() + (b.Width() / 2), b.Y() + b.Height() / 2});
		}
		if (b.DisplayName())
		{
			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + 5 });
		}
	}

};


class ToggleButtonGraphics
{
public:

	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		d.Command<Fill>(theme->Get(C::ToggleButtonB));
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		if (b.Active())
			d.Command<Fill>(theme->Get(C::ToggleButtonV));
		else
			d.Command<Fill>(theme->Get(C::ToggleButton));

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (!b.Active() && b.Hovering())
		{
			d.Command<Fill>(theme->Get(C::ToggleButtonH));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		}

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		if (b.Active())
			d.Command<Fill>(theme->Get(C::ToggleButtonText));
		else 
			d.Command<Fill>(theme->Get(C::TextOff));
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() / 2 });
		
	}

};


class RadioButtonGraphics
{
public:

	static void Render(Button<RadioButtonGraphics, ButtonType::List>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		d.Command<Fill>(theme->Get(C::ToggleButtonB));
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		if (b.Selected())
			d.Command<Fill>(theme->Get(C::ToggleButtonV));
		else
			d.Command<Fill>(theme->Get(C::ToggleButton));

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (!b.Selected() && (b.Hovering() || b.Active()))
		{
			d.Command<Fill>(theme->Get(C::ToggleButtonH));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		}

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		if (b.Selected())
			d.Command<Fill>(theme->Get(C::ToggleButtonText));
		else
			d.Command<Fill>(theme->Get(C::TextOff));
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() / 2 });

	}

};

class NormalButtonGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		d.Command<Fill>(theme->Get(C::ToggleButtonB));
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		if (b.Active())
			d.Command<Fill>(theme->Get(C::ToggleButtonV));
		else
			d.Command<Fill>(theme->Get(C::ToggleButton));

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (!b.Active() && b.Hovering())
		{
			d.Command<Fill>(theme->Get(C::ToggleButtonH));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		}

		d.Command<Font>(Fonts::Gidole14, 14.0f);

		d.Command<Fill>(theme->Get(C::TextSmall));		

		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), Vec2<int>{ b.X() + b.Width() / 2, b.Y() + b.Height() / 2 });
	}

};

class DropdownButton
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		d.Command<Fill>(theme->Get(C::ToggleButtonB));
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		if (b.Active())
			d.Command<Fill>(theme->Get(C::ToggleButtonV));
		else
			d.Command<Fill>(theme->Get(C::ToggleButton));

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (!b.Active() && b.Hovering())
		{
			d.Command<Fill>(theme->Get(C::ToggleButtonH));
			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		}

		d.Command<Font>(Fonts::Gidole14, 14.0f);

		if ((int)b.Value() != 0)
			d.Command<Fill>(theme->Get(C::TextSmall));
		else
			d.Command<Fill>(theme->Get(C::TextOff));


		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), Vec2<int>{ b.X() + b.Width() / 2, b.Y() + b.Height() / 2 });
	}

};



class KnobGraphics
{
public:

	static void Render(Parameter<KnobGraphics>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;


		bool _double = b.Range().x < 0 && b.Range().y > 0;

		double _v = 1.0 - b.NormalizedValue();
		double _a = _v * M_PI * 1.5 + M_PI * 0.25 - M_PI / 2.0;

		int _w = b.NormalizedValue() * (b.Width() * 0.5 - 1);

		int _h = b.Height() - _p * 2;
		int _we = _w - _p * 2;
		d.Command<Fill>(theme->Get(C::KnobSliderB));
		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size()});
		d.Command<Fill>(theme->Get(C::KnobSliderS));
		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size()}, Vec2<double>{_double ? _a > M_PI / 2 ? _a : M_PI / 2 : -M_PI * 0.75f, _double ? _a > M_PI / 2 ? M_PI / 2 : _a : _a});
		d.Command<Fill>(theme->Get(C::KnobSlider));
		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size() - 4});
		d.Command<Fill>(theme->Get(C::KnobSlider));

		int _x1 = std::cos(M_PI * 0.25 - M_PI / 2.0) * b.Width() / 2;
		int _x2 = std::cos(M_PI * 1.75 - M_PI / 2.0) * b.Width() / 2;

		d.Command<Graphics::Quad>(Vec4<int>{ b.X() + b.Width() / 2 + _x2, b.Y(), _x1 - _x2, b.Height() / 4});
		d.Command<Fill>(theme->Get(C::KnobSliderV));
		double _x = std::cos(_a) * b.Width() / 2.0;
		double _y = std::sin(_a) * b.Height() / 2.0;
		//d.Command<Graphics::Ellipse>(Vec4<int>{ (int)(b.X() + b.Width() / 2.0 + _x), (int)(b.Y() + b.Height() / 2.0 + _y + 1), b.Width() / 4, b.Height() / 4 }, _v * 360);
		d.Command<Graphics::Quad>(Vec4<int>{ (int)(b.X() + b.Width() / 4.0 + (_x + 1) / 2.0), (int)(b.Y() + b.Height() / 2.0 + (_y) / 2.0), b.Width() / 2, 2 }, 360 * _a / (2 * M_PI));


		d.Command<Font>(Fonts::Gidole14, 14.0f);
		if (b.Disabled())
			d.Command<Fill>(theme->Get(C::TextOff));
		else
			d.Command<Fill>(theme->Get(C::TextSmall));

		if (b.DisplayValue())
		{
			d.Command<TextAlign>(Align::CENTER, Align::TOP);
			d.Command<Text>(&b.ValueText(), Vec2<int>{ b.X() + (b.Width() / 2), b.Y() + 3});
		}		
		if (b.DisplayName())
		{
			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + 5 });
		}
	}
};
namespace SoundMixrGraphics
{
	using namespace Graphics;
	
	template<Align Hori = Align::CENTER, Align Vert = Align::CENTER>
	struct Normal
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? theme->Get(C::ButtonD) : b.Active() ? theme->Get(C::ButtonS) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Fill>(theme->Get(C::Text));
			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<TextAlign>(Hori, Vert);
			if (Hori == Align::CENTER)
				d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
			else if (Hori == Align::RIGHT)
				d.Command<Text>(&b.Name(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			else if (Hori == Align::LEFT)
				d.Command<Text>(&b.Name(), b.X() + 6, b.Y() + b.Height() / 2);
		}
	};

	struct Menu
	{
		template<typename This = ButtonBase>
		static void Render(This& b, CommandCollection& d)
		{
			int _padding = 20;
			Color _c1 = b.Disabled() ? theme->Get(C::ButtonD) : b.Active() ? theme->Get(C::ButtonS) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);
			Color _c2 = theme->Get(C::Text);
			Color _c3 = theme->Get(C::TextOff);

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
			Color _c1 = b.Disabled() ? theme->Get(C::ButtonD) : b.Active() ? theme->Get(C::ButtonS) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);
			Color _c2 = theme->Get(C::Text);
			Color _c3 = theme->Get(C::TextOff);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());

			if (b.Selected())
			{
				d.Command<Fill>(theme->Get(C::ButtonS));
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
			Color _c1 = b.Disabled() ? theme->Get(C::ButtonD) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);
			Color _c2 = theme->Get(C::Text);
			Color _c3 = theme->Get(C::TextOff);

			d.Command<Fill>(_c1);
			d.Command<Quad>(b.Position(), b.Size());

			if (b.Active())
			{
				d.Command<Fill>(theme->Get(C::ButtonS));
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
			Color _c1 = b.Disabled() ? theme->Get(C::ButtonD) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);
			Color _c2 = theme->Get(C::Text);
			Color _c3 = theme->Get(C::TextOff);

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
		template<typename This = ButtonBase>
		static void Render(This& b, CommandCollection& d)
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
			 Color _c1 = b.Active() ? theme->Get(C::Menu) : b.Hovering() ? theme->Get(C::ButtonH) : theme->Get(C::Button);
			 Color _c2 = theme->Get(C::Text);

			 int _padding = 20;
			 if (b.Active())
			 {
				 d.Command<Fill>(theme->Get(C::MenuB));
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

			d.Command<Fill>(theme->Get(C::MenuB));
			d.Command<Quad>(b.X() - 1, b.Y() - 1, b.Width() + 2, b.Height() + 2);

			d.Command<Fill>(theme->Get(C::Menu));
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
				d.Command<Fill>(theme->Get(C::Scrollbar));
				d.Command<Quad>(b.Position().x, b.Position().y + b.Size().height / 2 - 1, b.Size().width, 2);
				d.Command<Fill>(theme->Get(C::ScrollbarH));
				_p = 4;
			}
			else
				d.Command<Fill>(theme->Get(C::Scrollbar));

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
				d.Command<Fill>(theme->Get(C::Scrollbar));
				d.Command<Quad>(b.Position().x + b.Size().width / 2 - 1, b.Position().y, 2, b.Size().height);
				d.Command<Fill>(theme->Get(C::ScrollbarH));
				_p = 4;
			}
			else
				d.Command<Fill>(theme->Get(C::Scrollbar));

			d.Command<Quad>(b.Position().x + _p, _y + _p, b.Size().width - _p * 2, _h - _p * 2);

		}
	};
}

class SMXRScrollPanel : public ScrollPanel
{
public:

	Vec2<bool> ScrollbarNotNecessary()
	{
		return { m_ScrollbarX->NotNecessary(), m_ScrollbarY->NotNecessary() };
	}

	// Overwrite these methods from ScrollPanel for custom ScrollbarGraphics	
	::Panel& Panel() const { return *m_Panel; }
	template<typename T, typename ...Args>
	T& Panel(Args&&... args)
	{
		if (m_Panel != nullptr)
			return dynamic_cast<T&>(*m_Panel);

		auto& _t = Emplace<T>(std::forward<Args>(args)...);
		m_Panel = &_t;
		m_ScrollbarX = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Horizontal>>();
		m_ScrollbarY = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Vertical>>();
		return _t;
	}
};