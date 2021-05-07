#include "audio/EffectPanel.hpp"

EffectPanel::EffectPanel()
{
	// Toggle button, will bypass the effect chain if it's not nullptr.
	m_BypassChain = &Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(
		[this](bool a) {
			if (m_EffectChain)
				m_EffectChain->Bypass(!a);
		}
	, "");
	m_BypassChain->Size({ 18, 18 });
	m_BypassChain->Visible(true);

	// Forward all events to the effect chain if it is not nullptr.
	m_Listener += [this](Event& e)
	{
		if (m_EffectChain)
		{
			Event copy = e;
			if (e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased && e.type != Event::Type::KeyTyped)
				copy.x -= X(), copy.y -= Y();
			m_EffectChain->AddEvent(copy);
		}
	};

	// Eventlistener for cross to close effect panel.
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT && e.x > Width() - 46 && e.x < Width() - 16 && e.y > Height() - 32 && e.y < Height() - 8)
			Visible(false);
	};

	// Static width, because effects are always 300 pixels wide.
	Width(340);
	MinWidth(340);
}

void EffectPanel::EffectChain(::EffectChain* e)
{
	// If not nullptr, set toggle button to the bypass.
	if (e)
	{
		m_BypassChain->Active(!e->Bypass());
		e->Visible(true);
		Visible(true);
	}
	else
	{
		Visible(false);
	}
	m_EffectChain = e;
}

void EffectPanel::Update(const Vec4<int>& viewport)
{
	// Set position of toggle button.
	int x = m_ShowSidebar ? 8 : 0;
	m_BypassChain->Position({ x + 8 + 3, Height() - 32 + 3 });

	// Update and set size of effectchain if not nullptr.
	if (m_EffectChain)
	{
		if (!m_EffectChain->Visible())
			Visible(false);

		auto newv = viewport;
		newv.Translate({ X(), Y() });
		m_EffectChain->Size(Size() - Vec2<int>{ x, 32 });
		m_EffectChain->X(x);
		m_EffectChain->Update(newv);
	}

	// Cursor from effect chain if not nullptr, otherwise layoutmanager.
	m_Cursor = m_EffectChain ? m_EffectChain->Cursor() : m_Pressed && m_LayoutManager.Cursor() == -1 ?
		GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

	Container::Update(viewport);
}

void EffectPanel::Render(CommandCollection& d)
{
	d.Command<Graphics::PushMatrix>();
	d.Command<Graphics::Translate>(Vec2<int>{ X(), Y() });

	// Make sure background color matches theme.
	Background(ThemeT::Get().window_panel);
	Background(d);

	// Border on the left of the panel.
	int x = 8;
	if (m_ShowSidebar)
	{
		d.Command<Graphics::Fill>(ThemeT::Get().window_frame);
		d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, 8, Height() });
		x = 16;
	}

	// Render effect chain if not nullptr.
	if (m_EffectChain)
		m_EffectChain->Render(d);

	// Display the top bar of the effect panel
	d.Command<Graphics::Fill>(ThemeT::Get().effect_title_bar);
	d.Command<Graphics::Quad>(Vec4<int>{ x, Height() - 32, Width() - 24 - x, 24 });
	d.Command<Graphics::Fill>(ThemeT::Get().effect_title_text);
	d.Command<Graphics::Font>(Graphics::Fonts::Gidole16, 16.0f);
	d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
	d.Command<Graphics::Text>(&m_Name, Vec2<int>{ 30 + x, Height() - 20 });
	d.Command<Graphics::Fill>(ThemeT::Get().effect_minimize_button);
	d.Command<Graphics::Line>(Vec4<float>{ Width() - 42.f, Height() - 26.f, Width() - 30.f, Height() - 14.f }, 3.0f);
	d.Command<Graphics::Line>(Vec4<float>{ Width() - 42.f, Height() - 14.f, Width() - 30.f, Height() - 26.f }, 3.0f);
	m_BypassChain->Render(d);
	d.Command<Graphics::PopMatrix>();
}