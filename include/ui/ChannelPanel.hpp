#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/EndpointPanel.hpp"

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
		: m_InputChannel(&c), m_Input(true),
		text(Emplace<Button<SmollText, BT::Normal>>([]() {}, c.name, Vec2<int>{100, 24}))
	{
		text.Disable();

		// Styling of panel
		Background(Color{ 33, 33, 33, 255 });
		Width(100);
	}

	ChannelPanel(StereoOutputChannel& c)
		: m_OutputChannel(&c), m_Input(false),
		text(Emplace<Button<SmollText, BT::Normal>>([]() {}, c.name, Vec2<int>{100, 24}))
	{
		text.Disable();

		// Styling of panel
		Background(Color{ 33, 33, 33, 255 });
		Width(100);
	}

	bool Close() { return m_ClosePanel; }

private:

	void Update(const Vec4<int>& viewport)
	{

		// Location of buttons
		text.Position(Vec2<int>{0, Height() - 24});
		Panel::Update(viewport);
	}

	bool m_Input;
	union
	{
		StereoInputChannel* m_InputChannel;
		StereoOutputChannel* m_OutputChannel;
	};
	Button<SmollText, BT::Normal>& text;

	bool m_ClosePanel = false;
};