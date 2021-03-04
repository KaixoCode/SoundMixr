#include "ui/EffectPanel.hpp"

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
}

void EffectPanel::Update(const Vec4<int>& viewport)
{
	if (m_EffectsGroup)
		m_Cursor = m_EffectsGroup ? m_EffectsGroup->Cursor() : m_Pressed && m_LayoutManager.Cursor() == -1 ?
		GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

	auto& newv = viewport.Overlap({ X(), Y(), Width(), Height() }).Translate({ X(), Y() });

	if (m_EffectsGroup)
	{
		Height(m_EffectsGroup->Height() + 8);
		m_EffectsGroup->Width(Width());
		m_EffectsGroup->Y(8);
		m_EffectsGroup->Update(newv);
	}

	Container::Update(newv);
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
	EnableScrollbars(false, true);
	m_Menu.ButtonSize({ 160, 20 });
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Effect Panel").Disable();
	m_Div = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 4);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { Visible(false); }, "Hide Effects Panel");
	m_Div2 = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 4);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&]
		{
			m_EffectPanel.AddEffect<Dynamics>();
		}, "+ Dynamics");
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&]
		{
			m_EffectPanel.AddEffect<Equalizer<>>();
		}, "+ Equalizer");
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT && !(m_EffectPanel.EffectsGroup() && m_EffectPanel.EffectsGroup()->Hovering()))
			RightClickMenu::Get().Open(&m_Menu);
	};
}

void EffectScrollPanel::Render(CommandCollection& d)
{
	m_Div->Color(Theme<C::Divider>::Get());
	m_Div2->Color(Theme<C::Divider>::Get());

	d.Command<Graphics::Fill>(Theme<C::Channel>::Get());
	d.Command<Graphics::Quad>(Vec4<int>{X() - 8, Y(), 8, Height()});
	d.Command<Graphics::Fill>(Theme<C::TextOff>::Get());
	d.Command<Graphics::Quad>(Vec4<int>{X() - 6, Y() + Height() / 2 - 8, 1, 16});
	d.Command<Graphics::Quad>(Vec4<int>{X() - 4, Y() + Height() / 2 - 8, 1, 16});
	d.Command<Graphics::Quad>(Vec4<int>{X() - 2, Y() + Height() / 2 - 8, 1, 16});

	SMXRScrollPanel::Render(d);
}
