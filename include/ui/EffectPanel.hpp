#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "audio/Effects.hpp"

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectPanel : public Panel
{
public:
	EffectPanel();

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectsGroup = e; }
	auto EffectsGroup() -> ::EffectsGroup* { return m_EffectsGroup; }

private:
	::EffectsGroup* m_EffectsGroup = nullptr;
};

// -------------------------------------------------------------------------- \\
// ------------------------ Effect Scroll Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectScrollPanel : public SMXRScrollPanel
{
public:
	EffectScrollPanel();

	void Update(const Vec4<int>& v) override;

	void EffectsGroup(EffectsGroup* e) { m_EffectPanel.EffectsGroup(e); }

private:
	EffectPanel& m_EffectPanel;
	double m_MouseY = 0;
};

// -------------------------------------------------------------------------- \\
// ------------------------- Effect Frame Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectFramePanel : public Panel
{
public:
	EffectFramePanel()
		: m_Scroll(Emplace<EffectScrollPanel>(Layout::Hint::Center))
	{
		SetupMenu();
		Layout<Layout::Border>(0, 8, false, false, false, false);
		auto& a = Emplace<Panel>(Layout::Hint::North);
		a.Height(24);
		m_Toggle = &Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(
			[this](bool a) { 
				if (m_EffectGroup) 
					a ? m_EffectGroup->Enable() : m_EffectGroup->Disable(); 
			}
		, "");
		m_Toggle->Size({ 18, 18 });
		m_Toggle->Visible(true);

		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT && e.x > Width() - 46 && e.x < Width() - 16 && e.y > Height() - 32 && e.y < Height() - 8)
				Visible(false);

			if (e.button == Event::MouseButton::RIGHT && !(m_EffectGroup && m_EffectGroup->Hovering()))
				RightClickMenu::Get().Open(&m_Menu);
		};
	}
	void SetupMenu() 
	{
		m_Menu.Clear();
		m_Menu.ButtonSize({ 160, 20 });
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Effect Panel").Disable();
		m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { Visible(false); }, "Hide Effects Panel");
		m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);

		for (auto& i : EffectLoader::Effects())
		{
			m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&]
				{
					if (m_EffectGroup)
					{
						auto a = i.second->CreateInstance();
						if (a != nullptr)
							m_EffectGroup->Add(a);
					}
				}, "+ " + i.first);
		}
	};

	void Update(const Vec4<int>& v)
	{
		Panel::Update(v);
		m_Toggle->Position({ 8 + 3, Height() - 32 + 3 });
	}

	void Render(CommandCollection& d) override 
	{
		d.Command<Graphics::Fill>(ThemeT::Get().window_frame);
		d.Command<Graphics::Quad>(Vec4<int>{X() - 8, Y(), 8, Height()});
		d.Command<Graphics::Fill>(ThemeT::Get().effect_title_bar);
		d.Command<Graphics::Quad>(Vec4<int>{X() + 8, Y() + Height() - 32, Width() - 32, 24 });
		d.Command<Graphics::Fill>(ThemeT::Get().effect_title_text);
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole16, 16.0f);
		d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Graphics::Text>(&m_Name, Vec2<int>{ X() + 38, Y() + Height() - 20 });

		d.Command<Graphics::Fill>(ThemeT::Get().effect_minimize_button);
		d.Command<Graphics::Line>(Vec4<int>{ X() + Width() - 42, Y() + Height() - 26, X() + Width() - 30, Y() + Height() - 14 }, 3.0f);
		d.Command<Graphics::Line>(Vec4<int>{ X() + Width() - 42, Y() + Height() - 14, X() + Width() - 30, Y() + Height() - 26}, 3.0f);
		Panel::Render(d);
	};

	void EffectsGroup(EffectsGroup* e) 
	{ 
		m_Scroll.EffectsGroup(e); 
		m_EffectGroup = e; 
		if (e != nullptr)
			m_Toggle->Active(e->Enabled()), m_Name = e->Name() + " - Effects Chain";
	}

	void UpdateEffects() { if (m_EffectGroup) m_EffectGroup->UpdateEffects(); };

private:
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	::ButtonBase* m_Toggle;
	::EffectsGroup* m_EffectGroup;
	std::string m_Name = "";
	EffectScrollPanel& m_Scroll;
};