#pragma once
#include "pch.hpp"
#include "ui/Slider.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Some Graphics --------------------------------- \\
// -------------------------------------------------------------------------- \\

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
		Color _c2{ 200, 200, 200, 255 };

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
		Color _c1{ 0, 0, 0, 0 };
		Color _c2{ 50, 50, 50, 255 };
		if ((b.Hovering()))
		{
			_c1 = Color{ 30 ,30, 30, 255 };
			_c2 = Color{ 70, 70, 70, 255 };
		}
		if (b.Active())
		{
			_c1 = Color{ 28 ,28, 28, 255 };
			_c2 = Color{ 80, 80, 80, 255 };
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
		Color _c1{ 128, 128, 128, 255 };
		Color _c2{ 50, 50, 50, 255 };
		if ((b.Hovering()))
		{
			_c2 = Color{ 70, 70, 70, 255 };
		}
		if (b.Active())
		{
			_c1 = Color{ 255, 255, 255, 255 };
			_c2 = Color{ 90, 90, 90, 255 };
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
		Color _c1{ 128, 128, 128, 255 };
		Color _c2{ 50, 50, 50, 255 };
		if ((b.Hovering()))
		{
			_c2 = Color{ 70, 70, 70, 255 };
		}
		if (b.Active())
		{
			_c1 = Color{ 255, 255, 255, 255 };
			_c2 = Color{ 90, 90, 90, 255 };
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

		d.Command<Font>(Fonts::Gidole16, 16.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + 6, b.Y() + b.Height() / 2);
		int _w = 16;
		_padding = 4;
		if (!b.Disabled())
			d.Command<Triangle>(b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w / 2, _w / 2, -90.0f);
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

			d.Command<Fill>(Color{ 100, 100, 100, 255 });

			int _w = b.Width() - _p * 2;
			int _he = _h - _p * 2;
			d.Command<Triangle>(Vec4<int>{b.X() + _p, _y + _p, 8, _he}, 0.0f);
			d.Command<Triangle>(Vec4<int>{b.X() - _p + b.Width(), _y + _p, 8, _he}, 180.0f);
			d.Command<Quad>(Vec4<int>{b.X() + _p, _y + _p - 1, b.Width() - _p * 2, 3});

			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<Fill>(Color{ 200, 200, 200, 255 });
			d.Command<TextAlign>(Align::CENTER, Align::TOP);
			d.Command<Text>(&b.ValueText(), Vec2<int>{b.X() + _p + _w / 2, b.Y()});
		}
		else
		{
			int _p = 6;
			int _w = 25;
			int _x = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start) * -(b.Width()) + b.X() + b.Width();

			d.Command<Fill>(Color{ 100, 100, 100, 255 });

			int _h = b.Height() - _p * 2;
			int _we = _w - _p * 2;
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() + _p, _we, 8}, 90.0f);
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() - _p + b.Height(), _we, 8}, 270.0f);
			d.Command<Quad>(Vec4<int>{_x + _p - 1, b.Y() + _p, 3, b.Height() - _p * 2});

			d.Command<Font>(Fonts::Gidole14, 14.0f);
			d.Command<Fill>(Color{ 200, 200, 200, 255 });
			d.Command<TextAlign>(Align::CENTER, Align::TOP);
			d.Command<Text>(&b.ValueText(), Vec2<int>{b.X(), b.Y() + _p + _h / 2});
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
		d.Command<Fill>(Color{ 45, 45, 45, 255 });
		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		d.Command<Fill>(Color{ 15, 15, 15, 255 });
		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		d.Command<Fill>(Color{ 90, 90, 90, 255 });
		d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2, b.Y() + 1, _w, b.Height() - 2});

		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Color{ 200, 200, 200, 255 });
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.ValueText(), b.Position() + (b.Size() / 2));
	}
};