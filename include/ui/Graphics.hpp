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
		Color _c2{ 255, 255, 255, 255 };

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