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
	Width(314);
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
	SetupMenu();
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT && !(m_EffectPanel.EffectsGroup() && m_EffectPanel.EffectsGroup()->Hovering()))
			RightClickMenu::Get().Open(&m_Menu);
	};

	m_Listener += [this](Event::MousePressed& e)
	{
		m_MouseY = e.y;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		m_MouseY = e.y;
	};
}

void EffectScrollPanel::SetupMenu()
{
	m_Menu.Clear();
	m_Menu.ButtonSize({ 160, 20 });
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Effect Panel").Disable();
	m_Div = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 2);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { Visible(false); }, "Hide Effects Panel");
	m_Div2 = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 2);

	for (auto& i : EffectLoader::Effects())
	{
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&]
			{
				m_EffectPanel.AddEffect(i.second->CreateInstance());
			}, "+ " + i.first);
	}
}

void EffectScrollPanel::Render(CommandCollection& d)
{
	m_Div->Color(theme->Get(C::Divider));
	m_Div2->Color(theme->Get(C::Divider));

	d.Command<Graphics::Fill>(theme->Get(C::Channel));
	d.Command<Graphics::Quad>(Vec4<int>{X() - 8, Y(), 8, Height()});

	SMXRScrollPanel::Render(d);
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
