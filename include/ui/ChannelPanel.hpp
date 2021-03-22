#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

class ChannelPanel : public Panel
{
public:
	template<typename T>
	ChannelPanel(T* l, bool isinput, bool special = false)
		: m_ChannelGroup(), m_IsInput(isinput), m_IsSpecial(special),
		volume(Emplace<OldVolumeSlider>()),
		routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, isinput ? "in" : "")),

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
		pan.Name("Pan");
		pan.DisplayName(false);
		
		Width(70);

		pan.Position(Vec2<int>{4, 27});
		pan.Size(Vec2<int>{62, 18});
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
		
		m_Div4 = &m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
		m_Connect = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&, l] {
				l->ShowEffectsPanel(ChannelGroup().EffectsGroup());
			}, "Show Effects");
		
		m_Div3 = &m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
		m_Connect = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&, l] {
				if (auto ss = m_SelectedSame)
				{
					ChannelGroup().ClearConnections();
					ss->ClearConnections();
					int size = ChannelGroup().ChannelAmount();
					for (int i = 0; i < size; i++)
					{
						auto b = ChannelGroup().Channels()[0];
						ss->AddChannel(b);
						ChannelGroup().RemoveChannel(b);
					}

					m_Delete = true;
				}
			}, "Combine");
		m_Connect->Visible(false);

		m_Split = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
			[&, l] {
				auto& a = m_IsSpecial ? l->EmplaceSpecialChannel(m_IsInput) : l->EmplaceChannel(m_IsInput);

				int size = ChannelGroup().ChannelAmount() / 2;
				for (int i = 0; i < size; i++)
				{
					auto b = ChannelGroup().Channels()[0];
					a.AddChannel(b);
					ChannelGroup().RemoveChannel(b);
				}

				l->SortChannels();

			}, "Split");

		m_Div2 = &m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
		m_MenuMuted = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
			[&](bool s) {
				m_ChannelGroup.Mute(s);
			}, "Mute");

		m_MenuMono = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
			[&](bool s) { 
				m_ChannelGroup.Mono(s);
			}, "Mono");

		m_Listener += [this, l](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
				RightClickMenu::Get().Open(&m_Menu);

			if (e.button == Event::MouseButton::LEFT)
			{
				if (m_Counter && !mono.Hovering() && !muted.Hovering() && !volume.Hovering() && !pan.Hovering() && (routed.Disabled() || !routed.Hovering()))
				{
					l->ShowEffectsPanel(ChannelGroup().EffectsGroup());
				}
				m_Counter = 20;
			}
		};

		m_Listener += [this](Event::MouseEntered& e)
		{
			m_Hovering = true;
		};

		m_Listener += [this](Event::MouseExited& e)
		{
			m_Hovering = false;
		};
	}

	void Select(ChannelPanel* s);
	void AddChannel(Channel* s);
	void Selected(bool v) { m_Selected = v; }
	bool Selected() { return m_Selected; }
	void Unselect() { m_HasSelect = false; routed.Disable(); m_Connect->Visible(false); }
	void Routed(bool v) { m_Routed = v; }
	bool Routed() { return m_Routed; }
	auto ChannelGroup() -> ::ChannelGroup& { return m_ChannelGroup; }
	bool IsInput() { return m_IsInput; }
	void Transparency(bool t) { m_Transparency = t; }
	bool Delete() { return m_Delete; }
	bool Hovering() { return m_Hovering; }
	bool IsSpecial() { return m_IsSpecial; }

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);

private:
	// This private thing is defined here because it needs to be initialized first
	// in the constructor, so it is defined above all other things that are
	// initialized in the constructor. Yes, that is how C++ works, learn your shit!
	::ChannelGroup m_ChannelGroup;

public:
	Button<RouteButton, ButtonType::Toggle>& routed;
	Button<MuteButton, ButtonType::Toggle>& muted;
	Button<MonoButton, ButtonType::Toggle>& mono;
	PanSlider& pan;
	OldVolumeSlider& volume;

private:
	static inline std::unordered_map<int, std::string> m_Numbers;
	static inline std::string m_NegInf = "Inf";

	ButtonBase
		*m_Connect, 
		*m_MenuTitle, 
		*m_Split, 
		*m_MenuMono, 
		*m_MenuMuted;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> 
		m_Menu;
	
	bool m_Transparency = false,
		m_Routed = false,
		m_Delete = false,
		m_Hovering = false,
		m_Selected = false,
		m_IsInput = false,
		m_IsSpecial = false,
		m_HasSelect = false;

	int m_Counter = 0;

	MenuDivider
		*m_Div1, 
		*m_Div2, 
		*m_Div3,
		*m_Div4;

	::ChannelGroup
		*m_SelectedChannels,
		*m_SelectedSame;

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;
};