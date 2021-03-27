#include "ui/EffectPanel.hpp"
#include "EffectLoader.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Effect Panel -------------------------------- \\
// -------------------------------------------------------------------------- \\

EffectPanel::EffectPanel()
{
	m_Listener += [this](Event& e)
	{
		if (m_EffectsGroup)
		{
			if (e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
				e.x -= X(), e.y -= Y();
			m_EffectsGroup->AddEvent(e);
		}
	};
	Width(316);
}

void EffectPanel::Update(const Vec4<int>& viewport)
{

	if (m_EffectsGroup)
	{
		Height(m_EffectsGroup->Height() + 64);
		auto& newv = viewport.Translate({ X(), Y() });
		newv.height += 96;
		newv.y -= 96;
		m_EffectsGroup->Width(Width());
		m_EffectsGroup->Y(64);
		m_EffectsGroup->Update(newv);
	}
	auto& newvv = viewport.Translate({ X(), Y() });
	newvv.height += 96;
	newvv.y -= 96;

	if (m_EffectsGroup)
		m_Cursor = m_EffectsGroup ? m_EffectsGroup->Cursor() : m_Pressed && m_LayoutManager.Cursor() == -1 ?
		GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

	if (m_EffectsGroup)
	{
		Height(m_EffectsGroup->Height() + 64);
		m_EffectsGroup->Width(Width());
		m_EffectsGroup->Y(64);
	}

	Container::Update(newvv);
}

void EffectPanel::Render(CommandCollection& d)
{
	d.Command<Graphics::PushMatrix>();
	d.Command<Graphics::Translate>(Vec2<int>{ X(), Y() });
	Background(d);
	if (m_EffectsGroup)
		m_EffectsGroup->Render(d);
	d.Command<Graphics::PopMatrix>();
}

// -------------------------------------------------------------------------- \\
// ------------------------ Effect Scroll Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

EffectScrollPanel::EffectScrollPanel()
	: m_EffectPanel(Panel<EffectPanel>())
{
	EnableScrollbars(true, true);

	m_Listener += [this](Event::MousePressed& e)
	{
		m_MouseY = e.y;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		m_MouseY = e.y;
	};
}

void EffectScrollPanel::Update(const Vec4<int>& v)
{
	if (m_EffectPanel.EffectsGroup() && m_EffectPanel.EffectsGroup()->Dragging())
	{
		if (m_MouseY < Y() + 50)
			m_ScrollbarY->Scroll(5 * 0.02 * (((Y() + 50) - m_MouseY)));

		if (m_MouseY > Y() + Height() - 50)
			m_ScrollbarY->Scroll(-5 * 0.02 * ((m_MouseY - (Y() + Height() - 50))));
	}
	SMXRScrollPanel::Update(v);
}


// -------------------------------------------------------------------------- \\
// ------------------------- Effect Frame Panel ----------------------------- \\
// -------------------------------------------------------------------------- \\

EffectFramePanel::EffectFramePanel()
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

void EffectFramePanel::SetupMenu()
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

void EffectFramePanel::Update(const Vec4<int>& v)
{
	Panel::Update(v);
	m_Toggle->Position({ 8 + 3, Height() - 32 + 3 });
}

void EffectFramePanel::Render(CommandCollection& d)
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
	d.Command<Graphics::Line>(Vec4<float>{ X() + Width() - 42.f, Y() + Height() - 26.f, X() + Width() - 30.f, Y() + Height() - 14.f }, 3.0f);
	d.Command<Graphics::Line>(Vec4<float>{ X() + Width() - 42.f, Y() + Height() - 14.f, X() + Width() - 30.f, Y() + Height() - 26.f }, 3.0f);
	Panel::Render(d);
};

void EffectFramePanel::EffectsGroup(::EffectsGroup* e)
{
	m_Scroll.EffectsGroup(e);
	m_EffectGroup = e;
	if (e != nullptr)
		m_Toggle->Active(e->Enabled()), m_Name = e->Name() + " - Effects Chain";
}

void EffectFramePanel::UpdateEffects() 
{ 
	if (m_EffectGroup) 
		m_EffectGroup->UpdateEffects(); 
};
