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
		// Forward all events to the effect chain if it is not nullptr.
		m_Listener += [this](Event& e)
		{
			if (m_EffectChain)
			{
				if (e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased && e.type != Event::Type::KeyTyped)
					e.x -= X(), e.y -= Y();
				m_EffectChain->AddEvent(e);
			}
		};

		// Static width, because effects are always 300 pixels wide.
		Width(332);
	}

	void Update(const Vec4<int>& viewport) override
	{
		// Update and set size of effectchain if not nullptr.
		if (m_EffectChain)
		{
			auto& newv = viewport.Translate({ X(), Y() });
			m_EffectChain->Size(Size());
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

		d.Command<Graphics::PopMatrix>();
	}

	/**
	 * Set the effect chain for this effect panel.
	 * @param e effect chain
	 */
	void EffectChain(EffectChain* e) { m_EffectChain = e; }

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

private:
	::EffectChain* m_EffectChain = nullptr;
};