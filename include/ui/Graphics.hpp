#pragma once
#include "pch.hpp"

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
		d.Command<Font>(Fonts::Gidole, 24);
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

		d.Command<Font>(Fonts::Gidole14, 14);
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
		d.Command<Font>(Fonts::Gidole16, 11);
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
		d.Command<Font>(Fonts::Gidole16, 11);
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