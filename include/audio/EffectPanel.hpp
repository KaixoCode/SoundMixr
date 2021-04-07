#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "audio/EffectChain.hpp"

/**
 * This is the panel that contains a pointer to the EffectChain of the selected channel
 * and displays it/forwards events etc.
 */
class EffectPanel : public Panel
{
public:

	/**
	 * Constructor.
	 */
	EffectPanel()
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

		// Static width, because effects are always 300 pixels wide.
		Width(340);
		MinWidth(340);
	}

	/**
	 * Set the name to display at the top of the effect panel.
	 * @param name name
	 */
	void Name(const std::string& name) { m_Name = name; }

	/**
	 * Get the name that is being displayed at the top of this effect panel.
	 * @return name
	 */
	auto Name() -> std::string& { return m_Name; }

	/**
	 * Set the effect chain for this effect panel.
	 * @param e effect chain
	 */
	void EffectChain(EffectChain* e) 
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

	/**
	 * Get the effect chain currently in this effect panel.
	 * @return effect chain
	 */
	auto EffectChain() -> ::EffectChain* { return m_EffectChain; }

	/**
	 * Override the focused to automatically focus the effect 
	 * chain as well if not nullptr.
	 * @param v focused
	 */
	virtual void Focused(bool v) override { Panel::Focused(v); if (m_EffectChain) m_EffectChain->Focused(v); };
	virtual bool Focused() const override { return m_Focused; }

	/**
	 * Override the hovering to automatically hvoer the effect
	 * chain as well if not nullptr.
	 * @param v hovering
	 */
	virtual void Hovering(bool v) override { Panel::Hovering(v); if (m_EffectChain) m_EffectChain->Hovering(v); };
	virtual bool Hovering() const override { return m_Hovering; }

	void Update(const Vec4<int>& viewport) override
	{
		// Set position of toggle button.
		m_BypassChain->Position({ 16 + 3, Height() - 32 + 3 });

		// Update and set size of effectchain if not nullptr.
		if (m_EffectChain)
		{
			if (!m_EffectChain->Visible())
				Visible(false);

			auto newv = viewport;
			newv.Translate({ X(), Y() });
			m_EffectChain->Size(Size() - Vec2<int>{ 8, 32 });
			m_EffectChain->X(8);
			m_EffectChain->Update(newv);
		}

		// Cursor from effect chain if not nullptr, otherwise layoutmanager.
		m_Cursor = m_EffectChain ? m_EffectChain->Cursor() : m_Pressed && m_LayoutManager.Cursor() == -1 ?
			GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

		Container::Update(viewport);
	}

	void Render(CommandCollection& d) override
	{
		d.Command<Graphics::PushMatrix>();
		d.Command<Graphics::Translate>(Vec2<int>{ X(), Y() });

		// Make sure background color matches theme.
		Background(ThemeT::Get().window_panel);
		Background(d);

		// Border on the left of the panel.
		d.Command<Graphics::Fill>(ThemeT::Get().window_frame);
		d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, 8, Height() });

		// Render effect chain if not nullptr.
		if (m_EffectChain)
			m_EffectChain->Render(d);

		// Display the top bar of the effect panel
		d.Command<Graphics::Fill>(ThemeT::Get().window_frame);
		d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, 8, Height()});
		d.Command<Graphics::Fill>(ThemeT::Get().effect_title_bar);
		d.Command<Graphics::Quad>(Vec4<int>{ 16, Height() - 32, Width() - 40, 24 });
		d.Command<Graphics::Fill>(ThemeT::Get().effect_title_text);
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole16, 16.0f);
		d.Command<Graphics::TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Graphics::Text>(&m_Name, Vec2<int>{ 46, Height() - 20 });
		d.Command<Graphics::Fill>(ThemeT::Get().effect_minimize_button);
		d.Command<Graphics::Line>(Vec4<float>{ Width() - 42.f, Height() - 26.f, Width() - 30.f, Height() - 14.f }, 3.0f);
		d.Command<Graphics::Line>(Vec4<float>{ Width() - 42.f, Height() - 14.f, Width() - 30.f, Height() - 26.f }, 3.0f);
		m_BypassChain->Render(d);
		d.Command<Graphics::PopMatrix>();
	}

private:
	::EffectChain* m_EffectChain = nullptr;
	::ButtonBase* m_BypassChain;
	std::string m_Name;
};