#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"

class TitleText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{35, 35, 35, 255};
		Color _c2{255, 255, 255, 255};

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Font>(Fonts::Gidole, 24);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width()/2, b.Y() + b.Height() / 2);
	}
};

class CloseButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 8;
		Color _c1{ 35, 35, 35, 255 };
		Color _c2{ 255, 255, 255, 255 };
		if (!b.Disabled() && (b.Hovering()))
			_c1 = Color{ 30, 30, 30, 255 };
		else if (!b.Disabled() && b.Active())
			_c1 = Color{ 25, 25, 25, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Fill>(Color{ 255, 0, 0, 255 });
		d.Command<Quad>(Vec4<int>{b.X() + _padding / 2, b.Y() + b.Height() / 2, b.Width() - _padding, 2}, 45.0f);
		d.Command<Quad>(Vec4<int>{b.X() + _padding / 2, b.Y() + b.Height() / 2, b.Width() - _padding, 2}, 90.0f+45.0f);
	}
};

class NiceButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{ 35, 35, 35, 255 };
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
		d.Command<Triangle>(b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w / 2, _w / 2, -90.0f);
	}
};

namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::RIGHT>>;
using MenuButton = Button<BG::Menu, BT::Normal>;

class EndpointPanel : public Panel
{
public:
	EndpointPanel(bool in)
		: Panel(), m_Audio(in),
		m_DeviceButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_DeviceMenu);}, "Pick Device", Vec2<int>{200, 24})),
		m_SRButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_SRMenu);}, "Rate", Vec2<int>{80, 24})),
		m_BSButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_BSMenu);}, "Buffersize", Vec2<int>{100, 24})),
		m_Close(Emplace<Button<CloseButton, BT::Normal>>([&] { m_ClosePanel = true; LOG("CLOSE"); }, "", Vec2<int>{24, 24}))
	{
		// Styling of panel
		Background(Color{ 35, 35, 35, 255 });
		Height(48);

		// Location of buttons
		m_DeviceButton.Position(Vec2<int>{20, Height() - 24});
		m_Close.Position(Vec2<int>{220, Height() - 24});
		m_SRButton.Position(Vec2<int>{20, Height() - 48});
		m_SRButton.Disable();
		m_BSButton.Position(Vec2<int>{100, Height() - 48});
		m_BSButton.Disable();

		// Add all available devices to the menu in other thread
		auto& _devices = m_Audio.Devices();
		int listid = BT::List::NewKey();
		for (auto& _d : _devices)
		{
			// Only inputs or outputs depending on type
			if (in ? _d.inputChannels == 0 : _d.outputChannels == 0)
				continue;

			std::string name = _d.name;
			int i = name.find_first_of('(');
			name.resize(i, ' ');
			m_DeviceMenu.Emplace<Button<BG::Menu, BT::List>>([=] { m_DeviceButton.Name(name); SetDevice(_d); }, name, Vec2<int>{220, 20}, listid);
		}
	}

	void SetDevice(const Device& d)
	{
		int error = (m_Audio.SetDevice(d));

		m_SRButton.Enable();
		m_BSButton.Enable();

		id = std::to_string(m_Audio.Device().id);
		m_SetSamplerate = true;
	}

	bool Close() { return m_ClosePanel; }

private:
	void Render(CommandCollection& d) override
	{
		Panel::Render(d);
		if (m_Audio.GetAudio().isStreamOpen())
		{
			m_SRButton.Name(std::to_string(m_Audio.GetAudio().getStreamSampleRate()));
			m_BSButton.Name(std::to_string(m_Audio.Buffersize()));
		}
		d.Command<Graphics::Fill>(Color{ 255, 255, 255, 255 });
		d.Command<Graphics::TextAlign>(Align::LEFT, Align::TOP);
		if (!id.empty())
		{
			d.Command<Graphics::Font>(Graphics::Fonts::Gidole, 24);
			d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Graphics::Text>(&id, X() + 5, Y() + Height() / 2 - 2);
		}

		if (m_SetSamplerate)
		{
			m_SRMenu.Clear();
			m_BSMenu.Clear();
			int listid = BT::List::NewKey();
			for (auto& _sr : m_Audio.Device().sampleRates)
				m_SRMenu.Emplace<Button<BG::Menu, BT::List>>([=] { m_SRButton.Name(std::to_string(_sr)); m_Audio.SetSamplerate(_sr); }, std::to_string(_sr), Vec2<int>{80, 20}, listid);
			
			static int bs[]{128, 256, 512, 1024, 2048};
			listid = BT::List::NewKey();
			for (int i = 0; i < 5; i++)
				m_BSMenu.Emplace<Button<BG::Menu, BT::List>>([=] { m_BSButton.Name(std::to_string(bs[i])); m_Audio.SetBuffersize(bs[i]); }, std::to_string(bs[i]), Vec2<int>{80, 20}, listid);

			m_SetSamplerate = false;
		}
	}

private:	   
	Button<NiceButton, BT::Normal>& m_DeviceButton, &m_SRButton, &m_BSButton;
	Button<CloseButton, BT::Normal>& m_Close;
	Menu<MenuGraphics::Vertical, MenuType::Normal> m_DeviceMenu, m_SRMenu, m_BSMenu;
	Audio m_Audio;
	std::string id;

	bool m_SetSamplerate = false, m_ClosePanel = false;
};