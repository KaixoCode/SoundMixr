#pragma once
#include "pch.hpp"
#include "audio/Effect.hpp"
#include "ui/Graphics.hpp"

template <typename t> void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
{
	if (oldIndex > newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

/**
 * Panel that is stored inside of each channel, will be given to EffectPanel to
 * display it in the east panel of the AudioController.
 */
class EffectChain : public SMXRScrollPanel
{
public:

	/**
	 * Constructor.
	 * All layout and event handling.
	 */
	EffectChain()
	{
		// The panel in this scrollpanel will contain 2 panels, one will contain all effects
		// and the other is some padding at the bottom.
		Panel<::Panel>();
		Panel().Layout<Layout::Stack>(0);
		Panel().AutoResize(true, true);
		Panel().Width(316);
		
		// This is the effect panel which will contain all the effects. It has a stack layout
		// because the effects will be displayed vertically.
		m_EffectPanel = &Panel().Emplace<::Panel>();
		m_EffectPanel->Layout<Layout::Stack>(8);
		m_EffectPanel->AutoResize(true, true);

		// Padding panel.
		Panel().Emplace<::Panel>().Height(128);

		// Enable scrollbars for both axis to not have autoresizing of components.
		// because effects are always 300 pixels wide.
		EnableScrollbars(true, true);

		// Setup the rightclick menu.
		GenerateMenu();

		m_Listener += [this](Event::MousePressed& e) 
		{
			// Used for dragging around effects.
			m_MouseY = e.y;

			// Rightclick menu, either the normal one for the EffectChain, or a special
			// one if hovering over an effect.
			if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
			{
				if (!m_EffectPanel->HoveringComponent())
				{
					RightClickMenu::Get().Open(&m_Menu);

					return;
				}
				
				// Get the hovering effect
				Effect* effect = (Effect*) m_EffectPanel->HoveringComponent();

				// Generate the rightclick menu from the effect.
				effect->GetMenu(m_EffectMenu);

				// Add a remove button.
				m_EffectMenu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
					[&, effect] {

						// Lock, to prevent concurrency issues
						m_Lock.lock();

						// Erase the effect from the effect panel
						auto it = std::remove_if(m_EffectPanel->Components().begin(), m_EffectPanel->Components().end(), 
							[effect](std::unique_ptr<Component>& c) { return c.get() == effect; });
						m_EffectPanel->Erase(it);
						m_Lock.unlock();
					}, "Remove");
				RightClickMenu::Get().Open(&m_EffectMenu);
			}
		};
		
		m_Listener += [this](Event::MouseDragged& e)
		{
			// Used for dragging around effects.
			m_MouseY = e.y;
		};

		// Listeners used for dragging effects to a new position.
		m_EffectPanel->Listener() += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT && m_EffectPanel->HoveringComponent() && ((Effect*)m_EffectPanel->HoveringComponent())->HoveringDrag())
				m_DraggingComponent = m_EffectPanel->HoveringComponent(),
				m_InsertIndex = constrain(GetIndex(e.y), 0, m_EffectPanel->Components().size() - 1);;
		};

		m_EffectPanel->Listener() += [this](Event::MouseDragged& e)
		{
			if (m_DraggingComponent)
				m_InsertIndex = constrain(GetIndex(e.y), 0, m_EffectPanel->Components().size() - 1);
		};

		m_EffectPanel->Listener() += [this](Event::MouseReleased& e)
		{
			if (m_DraggingComponent)
			{
				int _myIndex = 0;
				int _index = 0;
				for (auto& i : m_EffectPanel->Components())
				{
					if (i.get() == m_DraggingComponent)
					{
						_myIndex = _index;
						break;
					}
					_index++;
				}
				move(m_EffectPanel->Components(), _index, m_InsertIndex);
				m_DraggingComponent = nullptr;
				m_InsertIndex = -1;
			}
		};
	}

	/**
	 * Setup the rightclick menu.
	 */
	void GenerateMenu()
	{
		m_Menu.Clear();
		m_Menu.ButtonSize({ 160, 20 });
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Effect Panel").Disable();
		m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { Visible(false); }, "Hide Effects Panel");
		m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this]
			{
				RightClickMenu::Get().Close();
				std::string path = FileDialog::SaveFile();
				if (path.empty())
					return;
				try
				{
					std::ofstream _of{ path };
					nlohmann::json _json = *this;
					_of << _json;
					_of.close();
				}
				catch (...)
				{
					LOG("Failed to save effect chain");
				}
			}, "Save Effect Chain...");
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this]
			{
				RightClickMenu::Get().Close();
				std::string path = FileDialog::OpenFile();
				if (path.empty())
					return;

				try
				{
					std::ifstream _in{ path };
					nlohmann::json _json;
					_json << _in;
					_in.close();
					Clear();
					this->operator=(_json);
				}
				catch (...)
				{
					LOG("Failed to load effect chain");
				}
			}, "Load Effect Chain...");
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this]
			{
				Clear();
			}, "Remove All Effects");
		m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
		for (auto& i : EffectLoader::Effects())
		{
			m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&]
				{
					auto a = i.second->CreateInstance();
					if (a != nullptr)
						AddEffect(a);
				}, "+ " + i.first);
		}
	}

	/**
	 * Override because we need to lock before clearing.
	 */
	void Clear() override
	{
		// Make sure shit is locked
		m_Lock.lock();
		SMXRScrollPanel::Clear();
		m_Lock.unlock();
	}

	/**
	 * Add an effect to this chain.
	 * @param effect effectbase
	 */
	auto AddEffect(Effects::EffectBase* effect) -> Effect& 
	{
		// Lock to not cause concurrency issues.
		m_Lock.lock();

		// Set amount of channels.
		effect->Channels(m_Lines);

		// Add effect
		auto& e = m_EffectPanel->Emplace<Effect>(effect);
		
		m_Lock.unlock();

		return e;
	}

	/**
	 * Set the amount of lines in this chain. Resize all buffers etc.
	 * @param c count
	 */
	void Lines(int c)
	{
		for (auto& i : m_EffectPanel->Components())
			((Effect*)i.get())->Lines(c);

		m_Lines = c;
	}

	/**
	 * Set if this effect chain should be bypassed.
	 * @param v bypass
	 */
	void Bypass(bool v) 
	{
		for (auto& i : m_EffectPanel->Components())
			((Effect*)i.get())->Bypass(v);

		m_Bypassed = v; 
	}

	/**
	 * Returns true if this effect chain is bypassed.
	 * @return true if bypassed
	 */
	bool Bypass() const { return m_Bypassed; }

	/**
	 * Generate the next sample given the input sample and the line number.
	 * @param s input sample
	 * @param c line number
	 */
	double NextSample(double s, int c)
	{
		double out = s;

		// Lock, to make sure deleting of effects doesn't give concurrency issues.
		m_Lock.lock();

		// Apply all the effects
		for (auto& i : m_EffectPanel->Components())
			out = ((Effect*)i.get())->NextSample(out, c);

		m_Lock.unlock();
		return out;
	}

	/**
	 * Update the effects, happens separately because this needs to always happens, not
	 * only when the effect is visible and within the viewport.
	 */
	void UpdateEffects()
	{
		for (auto& i : m_EffectPanel->Components())
			((Effect*)i.get())->UpdateEffect();
	}

	/**
	 * Override the cursor, effectpanel will determine cursor in most cases. If over HoveringDrag arae
	 * the cursor will be the resize all cursor, otherwise it will be the cursor chosen by the hovering
	 * component. If no hovering it will be the normal cursor.
	 */
	int Cursor() const override
	{
		return (m_EffectPanel->HoveringComponent() && ((Effect*)m_EffectPanel->HoveringComponent())->HoveringDrag())
			|| m_DraggingComponent ? GLFW_RESIZE_ALL_CURSOR :
			m_EffectPanel->HoveringComponent() ? m_EffectPanel->HoveringComponent()->Cursor() : GLFW_CURSOR_NORMAL;
	}

	void Update(const Vec4<int>& v) override
	{
		// Auto scroll up or down when dragging.
		if (m_DraggingComponent)
		{
			if (m_MouseY < Y() + 50)
				m_ScrollbarY->Scroll(5 * 0.02 * (((Y() + 50) - m_MouseY)));

			if (m_MouseY > Y() + Height() - 50)
				m_ScrollbarY->Scroll(-5 * 0.02 * ((m_MouseY - (Y() + Height() - 50))));
		}
		SMXRScrollPanel::Update(v);
	}

	void Render(CommandCollection& d)
	{
		SMXRScrollPanel::Render(d);
		
		using namespace Graphics;

		if (!m_DraggingComponent)
			return;

		// Display the dragging line when dragging effect to new position.
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Clip>(Vec4<int>{ 0, 0, Width(), Height() });
		d.Command<Translate>(Panel().Position());
		d.Command<Translate>(m_EffectPanel->Position());
		int _index = 0;
		bool _past = false;
		for (auto& _c : m_EffectPanel->Components())
		{
			if (m_InsertIndex == _index)
			{
				d.Command<Fill>(ThemeT::Get().effect_drag_divider);
				d.Command<Quad>(Vec4<int>{_c->X(), _c->Y() + (_past ? -5 : _c->Height() + 3), _c->Width(), 2});
				break;
			}

			if (_c.get() == m_DraggingComponent)
				_past = true;
			
			_index++;
		}
		d.Command<PopClip>();
		d.Command<PopMatrix>();
	}

	virtual operator nlohmann::json()
	{
		nlohmann::json _json = nlohmann::json::object();
		_json["bypassed"] = m_Bypassed;
		_json["effects"] = nlohmann::json::array();
		for (auto& i : m_EffectPanel->Components())
		{
			nlohmann::json _j = *(Effect*)i.get();
			_json["effects"].push_back(_j);
		}
		return _json;
	}

	virtual void operator=(const nlohmann::json& json)
	{
		try
		{
			m_Bypassed = json.at("bypassed").get<bool>();

			for (auto effect : json.at("effects"))
			{
				try
				{
					auto& type = effect.at("type").get<std::string>();

					auto& _it = EffectLoader::Effects().find(type);
					if (_it != EffectLoader::Effects().end())
					{
						Effects::EffectBase* e = (*_it).second->CreateInstance();
						if (e != nullptr)
						{
							auto& a = AddEffect(e);
							a = effect;
							a.Bypass(m_Bypassed);
						}
					}
				}
				catch (...)
				{
				}
			}
		}
		catch (...)
		{
		}
	}

private:
	int m_InsertIndex = -1,
		m_Lines = 0;

	double m_MouseY = 0;

	bool m_Bypassed = false;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> 
		m_Menu,
		m_EffectMenu;

	::Panel* m_EffectPanel;
	
	Component* m_DraggingComponent = nullptr;
	
	// Lock is used to make sure any mutations to the effect chain 
	// are done concurrently safe.
	mutable std::mutex m_Lock;
	
	/**
	 * Converts y-coord to index in the effect panel.
	 * @param y y-coord
	 */
	int GetIndex(int y) const
	{
		int index = 0;
		int _ri = 0;
		for (auto& i : m_EffectPanel->Components())
		{
			if (y < i->Y() + i->Height() / 2)
				_ri = index + 1;

			index++;
		}

		if (m_DraggingComponent && y < m_DraggingComponent->Y() + m_DraggingComponent->Height() / 2)
			_ri -= 1;

		return _ri;
	}
};