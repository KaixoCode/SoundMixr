#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename This, typename Other = std::conditional<std::is_same_v<This, InputChannelGroup>, OutputChannelGroup, InputChannelGroup>::type>
class ChannelPanel : public Panel
{
public:
	static constexpr bool Input = std::is_same_v<This, ::InputChannelGroup>;

	template<typename T>
	ChannelPanel(T* l)
		: m_ChannelGroup(),
		volume(Emplace<VolumeSlider>()),
		routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, Input ? "in" : "")),

		muted(Emplace<Button<MuteButton, ButtonType::Toggle>>(
			[&](bool s) { 
				m_ChannelGroup.Mute(s); 
			}, "MUTE")),

		mono(Emplace<Button<MonoButton, ButtonType::Toggle>>(
			[&](bool s) { 
				m_ChannelGroup.Mono(s); 
			}, "MONO")),

		pan(Emplace<PanSlider>())
	{
		Width(70);

		pan.Position(Vec2<int>{4, 25});
		pan.Size(Vec2<int>{62, 19});
		volume.Vertical(true);
		volume.Position(Vec2<int>{0, 95});
		volume.Size(Vec2<int>{50, Height() - 135});
		muted.Position(Vec2<int>{5, 50});
		muted.Size({ 27, 25 });
		mono.Position(Vec2<int>{38, 50});
		mono.Size({ 27, 25 });
		routed.Position(Vec2<int>{0, 0});
		routed.Size({ 70, 25 });
		routed.Disable();

		// Init the rightclick menu:
		m_Menu.ButtonSize({ 180, 20 });
		m_MenuTitle = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(m_ChannelGroup.Name());
		m_MenuTitle->Disable();
		m_Div3 = &m_Menu.Emplace<MenuAccessories::Divider>(180, 1, 2, 2);
		m_Connect = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&, l] {
				if constexpr (std::is_same_v<This, InputChannelGroup>) ChannelGroup().Clear();
				if constexpr (std::is_same_v<This, OutputChannelGroup>) ChannelGroup().Clear();
				if constexpr (std::is_same_v<This, InputChannelGroup>) m_SelectedSame->Clear();
				if constexpr (std::is_same_v<This, OutputChannelGroup>) m_SelectedSame->Clear();

				for (auto& c : ChannelGroup().Channels())
					m_SelectedSame->AddChannel(c);

				m_Delete = true;
			}, "Combine");
		m_Connect->Visible(false);

		m_Split = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&, l] {
				auto& a = l->EmplaceChannel<ChannelPanel<This>>();

				int size = ChannelGroup().Size() / 2;
				for (int i = 0; i < size; i++)
				{
					auto b = ChannelGroup().Channels()[0];
					a.AddChannel(b);
					ChannelGroup().RemoveChannel(b);
				}

				l->SortChannels();

			}, "Split");

		m_Div1 = &m_Menu.Emplace<MenuAccessories::Divider>(180, 1, 2, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&] { 
				volume.SliderValue(1); 
			}, "Reset Volume");

		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&] { 
				pan.SliderValue(0); 
			}, "Reset Pan");

		m_Div2 = &m_Menu.Emplace<MenuAccessories::Divider>(180, 1, 2, 2);
		m_MenuMuted = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
			[&](bool s) {
				m_ChannelGroup.Mute(s); 
			}, "Mute");

		m_MenuMono = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
			[&](bool s) { 
				m_ChannelGroup.Mono(s);
			}, "Mono");

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};
	}

	void Select(Other* s)
	{
		m_SelectedChannels = s;
		m_HasSelect = true;

		if constexpr (Input)
		{
			m_Routed = m_ChannelGroup.Connected(s);
			routed.Enable();
		}
		else
		{
			m_Routed = s->Connected(&m_ChannelGroup);
			routed.Enable();
		}

		m_Connect->Visible(false);
	}

	void Select(This* s)
	{
		m_HasSelect = false;
		if constexpr (Input)
		{
			m_Routed = false;
			routed.Disable();
		}
		else
		{
			m_Routed = false;
			routed.Disable();
		}

		if (s != &m_ChannelGroup)
		{
			m_Connect->Name(std::string("Combine with ") + s->Name());
			m_Connect->Visible(true);
		}
		else
		{
			m_Connect->Visible(false);
		}
		m_SelectedSame = s;
	}

	template<typename T>
	void AddChannel(T* s)
	{
		m_ChannelGroup.AddChannel(s);
		m_MenuTitle->Name(m_ChannelGroup.Name());
	}

	void Selected(bool v) { m_Selected = v; }
	bool Selected() { return m_Selected; }
	void Unselect() { m_HasSelect = false; routed.Disable(); m_Connect->Visible(false); }
	void Routed(bool v) { m_Routed = v; }
	bool Routed() { return m_Routed; }
	auto ChannelGroup()  -> This& { return m_ChannelGroup; }
	void Transparency(bool t) { m_Transparency = t; }
	bool Delete() { return m_Delete; }

private:
	// This private thing is defined here because it needs to be initialized first
	// in the constructor, so it is defined above all other things that are
	// initialized in the constructor. Yes, that is how C++ works, learn your shit!
	This m_ChannelGroup;

public:
	//Button<SmallText, ButtonType::Normal>& text;
	Button<RouteButton, ButtonType::Toggle>& routed;
	Button<MuteButton, ButtonType::Toggle>& muted;
	Button<MonoButton, ButtonType::Toggle>& mono;
	PanSlider& pan;
	VolumeSlider& volume;

private:
	ButtonBase* m_Connect, *m_MenuTitle, *m_Split, *m_MenuMono, *m_MenuMuted;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	bool m_Transparency = false;
	bool m_Routed = false;
	bool m_Delete = false;

	MenuAccessories::Divider* m_Div1, * m_Div2, *m_Div3;

	bool m_HasSelect = false;
	Other* m_SelectedChannels;
	This* m_SelectedSame;

	bool m_Selected = false;

	std::unordered_map<int, std::string> m_Numbers;
	std::string m_NegInf = "Inf";

	void Update(const Vec4<int>& viewport)
	{
		if (m_HasSelect)
			if constexpr (!Input)
				if (m_Routed)
					m_SelectedChannels->Connect(&m_ChannelGroup);
				else
					m_SelectedChannels->Disconnect(&m_ChannelGroup);
			else if constexpr (Input)
				if (m_Routed)
					m_ChannelGroup.Connect(m_SelectedChannels);
				else
					m_ChannelGroup.Disconnect(m_SelectedChannels);

		Color c;
		if (Selected())
			c = Theme<C::ChannelSelected>::Get();
		else
			c = Theme<C::Channel>::Get();
		c.a = m_Transparency ? 245.0f : 255.0f;
		Background(c);

		m_Div1->Color(Theme<C::Divider>::Get());
		m_Div2->Color(Theme<C::Divider>::Get());
		m_Div3->Color(Theme<C::Divider>::Get());
		m_Div3->Visible(m_Connect->Visible() || m_Split->Visible());
		m_Split->Visible(ChannelGroup().Channels().size() > 1);

		volume.Size(Vec2<int>{50, Height() - 135});

		m_MenuMono->Active(m_ChannelGroup.Mono());
		m_MenuMuted->Active(m_ChannelGroup.Muted());
		mono.Active(m_ChannelGroup.Mono());
		muted.Active(m_ChannelGroup.Muted());

		m_ChannelGroup.Volume(volume.SliderValue());
		m_ChannelGroup.Pan(pan.SliderValue());
		Panel::Update(viewport);
	}

	void Render(CommandCollection& d)
	{
		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		Background(d);

		int _channels = m_ChannelGroup.Size();;
		int _y = 100;
		int _rh = Height() - 35 - _y;
		int _0db = ((1.0 / 1.412536) * (_rh)) + _y;
		int _3db = ((std::powf(std::powf(10, 3 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		int _6db = ((std::powf(std::powf(10, 6 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		int _w = (32.0 / _channels) - (_channels > 4 ? 1 : 2);
		int _x = 10;

		// Draw all audio meters
		for (int i = 0; i < _channels; i++)
		{
			_x = 10 + i * (_w + (_channels > 4 ? 1 : 2));
			float _level = std::powf(m_ChannelGroup.Channels()[i]->Peak(), 0.25);

			int _h = (std::min(_level, 1.412536f) / 1.412536) * (_rh);
			d.Command<Graphics::Fill>(Theme<C::VMeter>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
			d.Command<Graphics::Fill>(Theme<C::Channel>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
			d.Command<Graphics::Fill>(Theme<C::VMeterFill>::Get());
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
			if (_level > 1.0)
			{
				d.Command<Graphics::Fill>(Theme<C::VMeterFillC1>::Get());
				d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, _h - _0db + _y});
			}
			if (_level > std::powf(std::powf(10, 3 / 20.0), 0.25))
			{
				d.Command<Graphics::Fill>(Theme<C::VMeterFillC2>::Get());
				d.Command<Graphics::Quad>(Vec4<int>{_x, _3db, _w, _h - _3db + _y});
			}
			if (_level > std::powf(std::powf(10, 6 / 20.0), 0.25))
			{
				d.Command<Graphics::Fill>(Theme<C::VMeterFillC3>::Get());
				d.Command<Graphics::Quad>(Vec4<int>{_x, _6db, _w, _h - _6db + _y});
			}
		}
		
		// Volume level
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::CENTER, Align::TOP);
		d.Command<Text>(&volume.ValueText(), Vec2<int>{volume.X() + 6 + (volume.Width() - 6 * 2) / 2, volume.Y()});
			
		// Channel name
		d.Command<Font>(Fonts::Gidole16, 16.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::CENTER, Align::TOP);
		d.Command<Text>(&m_ChannelGroup.Name(), Vec2<int>{ Width() / 2, Height() - 4 });

		// db numbers besides volume meter
		int _d = 3;
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
		for (int i = 12; i > -120; i -= _d)
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;

			if (_b)
				d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
			else
				d.Command<Graphics::Fill>(Theme<C::VMeterIndD>::Get());

			int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
			d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
			if (_b)
			{
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i, std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(Theme<C::TextSmall>::Get());
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(Theme<C::VMeterIndB>::Get());
		d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});
		d.Command<Graphics::Fill>(Theme<C::TextSmall>::Get());
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y});
		
		Container::Render(d);
		d.Command<PopMatrix>();
	}
};

using InputChannelPanel = ChannelPanel<::InputChannelGroup>;
using OutputChannelPanel = ChannelPanel<::OutputChannelGroup>;
