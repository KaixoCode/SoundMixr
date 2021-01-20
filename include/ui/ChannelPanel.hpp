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

class NiceButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1{ 35, 35, 35, 255 };
		Color _c2{ 255, 255, 255, 255 };
		if (b.Hovering() || b.Active())
			_c1 = Color{ 40, 40, 40, 255 };

		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());

		d.Command<Font>(Fonts::Gidole14, 14);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + 6, b.Y() + b.Height() / 2);
	}
};

namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::RIGHT>>;
using MenuButton = Button<BG::Menu, BT::Normal>;

class ChannelPanel : public Panel
{
public:

	ChannelPanel(bool in)
		: Panel(), m_Audio(in),
		m_DeviceButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_DeviceMenu);}, "Pick Device", Vec2<int>{200, 20})),
		m_SRButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_SRMenu);}, "Samplerate", Vec2<int>{200, 20}))
	{
		LOG(in);
		Background(Color{ 35, 35, 35, 255 });
		m_DeviceButton.Position(Vec2<int>{30, Height() - 20});
		m_SRButton.Position(Vec2<int>{230, Height() - 20});
		auto& _devices = m_Audio.Devices();
		for (auto& _d : _devices)
			m_DeviceMenu.Emplace<Button<BG::Menu, BT::List>>([=] { m_DeviceButton.Name(_d.name); SetDevice(_d); }, _d.name, Vec2<int>{220, 20}, 69);
	}

	void SetDevice(const Device& d, int sr = -1)
	{
		int error = (m_Audio.SetDevice(d, sr));
		LOG(error);
		if (error)
			m_Audio.SetDevice(d, -1);

		id = std::to_string(m_Audio.Device().id);
		m_SetSamplerate = true;
	}

private:
	void Render(CommandCollection& d) override
	{
		Panel::Render(d);
		if (m_Audio.GetAudio().isStreamOpen())
			m_SRButton.Name(std::to_string(m_Audio.GetAudio().getStreamSampleRate()));
		d.Command<Graphics::Fill>(Color{ 255, 255, 255, 255 });
		d.Command<Graphics::TextAlign>(Align::LEFT, Align::TOP);
		if (!id.empty())
		{
			d.Command<Graphics::Font>(Graphics::Fonts::Gidole, 24);
			d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Graphics::Text>(&id, X() + 10, Y() + Height() / 2);
		}

		if (m_SetSamplerate)
		{
			m_SRMenu.Clear();
			for (auto& _sr : m_Audio.Device().sampleRates)
				m_SRMenu.Emplace<Button<BG::Menu, BT::List>>([=] { m_SRButton.Name(std::to_string(_sr)); SetDevice(m_Audio.Device(), _sr); }, std::to_string(_sr), Vec2<int>{220, 20}, 43);
			m_SetSamplerate = false;
		}
	}

private:	   
	Button<NiceButton, BT::Normal>& m_DeviceButton, &m_SRButton;
	Menu<MenuGraphics::Vertical, MenuType::Normal> m_DeviceMenu, m_SRMenu;
	Audio m_Audio;
	std::string id;

	bool m_SetSamplerate = false;
};