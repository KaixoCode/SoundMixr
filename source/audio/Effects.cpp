#include "audio/Effects.hpp"
#include "EffectLoader.hpp"

template <typename t> void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
{
	if (oldIndex > newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

// -------------------------------------------------------------------------- \\
// ------------------------------ Effect ------------------------------------ \\
// -------------------------------------------------------------------------- \\

Effect::Effect(EffectBase* effect)
	: m_Effect(effect), m_Channels(0)
{
	Init();
	m_RealHeight = -1;
	m_MinimB = &Emplace<Button<NOTHING, ButtonType::Toggle>>([&](bool s) { m_Minim->Active(s); }, "");
	m_Enable = &Emplace<Button<ToggleButtonG, ButtonType::Toggle>>(&m_Enabled, "");
	m_Enable->Size({ 18, 18 });
	m_MinimB->Size({ 25, 25 });

	m_Menu.ButtonSize({ 160, 20 });
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Effect->Name()).Disable();
	m_Div = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 2);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Enabled, "Enable");
	m_Minim = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>([&](bool s)
		{
			if (s)
			{
				m_RealHeight = Height();
				Height(25);
				m_Small = true;
			}
			else
			{
				if (m_RealHeight == -1)
					m_RealHeight = Height();

				Height(m_RealHeight);
				m_Small = false;
			}
		}, "Minimize");
	m_Div2 = &m_Menu.Emplace<MenuAccessories::Divider>(160, 1, 0, 2);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { m_Delete = true; }, "Remove");
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT)
			RightClickMenu::Get().Open(&m_Menu);
	};

	m_Listener += [this](Event::MouseEntered& e)
	{
		m_Hovering = true;
	};

	m_Listener += [this](Event::MouseExited& e)
	{
		m_Hovering = false;
	};

	m_Listener += [this](Event::MouseMoved& e)
	{
		m_HoveringDrag = false;
		if (e.x > 25 && e.y > Height() - 25 && e.x < Width() - 25)
			m_HoveringDrag = true;
	};

	m_Listener += [this](Event& e)
	{
		if (e.x - X() >= 25 && e.x - X() <= Width() - 25 && m_Small && e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
			e.y = Y() + 1;
	};
}

void Effect::Update(const Vec4<int>& v) 
{
	m_Size.height = m_Small ? 25 : m_Effect->Height() + 25;
	m_Enable->Position({ 3, Height() - 22 });
	m_MinimB->Position({ Width() - 25, Height() - 25 });
	m_Effect->Update();
	Background(theme->Get(C::Channel));
	Panel::Update(v);
}

void Effect::Render(CommandCollection& d)
{
	using namespace Graphics;
	if (!m_Small)
		Panel::Render(d);

	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	d.Command<Fill>(theme->Get(C::ChannelSelected));
	d.Command<Quad>(Vec4<int>{ 0, Height() - 25, Width(), 25 });
	d.Command<Font>(Fonts::Gidole16, 16.0f);

	if (m_Enabled)
		d.Command<Fill>(theme->Get(C::TextSmall));
	else
		d.Command<Fill>(theme->Get(C::TextOff));

	d.Command<TextAlign>(Align::LEFT, Align::TOP);
	d.Command<Text>(&m_Effect->Name(), Vec2<int>{ 30, Height() - 2});
	
	d.Command<Fill>(theme->Get(C::TextOff));
	if (!m_Small)
		d.Command<Quad>(Vec4<int>{Width() - 17, Height() - 13, 10, 2});
	else
		d.Command<Triangle>(Vec4<int>{Width() - 12, Height() - 12, 10, 10}, 270.0f);

	m_Enable->Render(d);
	m_MinimB->Render(d);

	if (!m_Enabled)
	{
		d.Command<Fill>(Color{ 0, 0, 0, 50 });
		d.Command<Quad>(Vec4<int>{ 0, 0, Width(), Height() - 25 });
	}

	if (!m_Small)
	{
		d.Command<Fill>(theme->Get(C::Divider));
		for (auto& i : m_Dividers)
			d.Command<Quad>(i);
	}

	d.Command<PopMatrix>();

	m_Div->Color(theme->Get(C::Divider));
	m_Div2->Color(theme->Get(C::Divider));
}

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Group --------------------------------- \\
// -------------------------------------------------------------------------- \\

EffectsGroup::EffectsGroup()
{
	Layout<Layout::Stack>(8);
	AutoResize(false, true);
	m_Listener.Clear();
	m_Listener += [this](Event& e)
	{
		// Make event coords relative to this panel
		if (e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
			e.x -= X(), e.y -= Y();

		m_LayoutManager.AddEvent(e);

		if (e.type == Event::Type::MousePressed)
			m_Pressed = true;

		if (e.type == Event::Type::MouseReleased)
			m_Pressed = false;

		Event _copy = e;
		Event _copy2 = e;

		if (e.type == Event::Type::KeyPressed || e.type == Event::Type::KeyReleased)
		{
			for (auto& _c : m_Effects)
				_c->AddEvent(_copy);

			return;
		}

		// If there was an unfocus event, unfocus the focussed component
		if (m_Focussed && e.type == Event::Type::Unfocused)
		{
			m_Focussed->AddEvent(_copy);
			return;
		}

		if (e.type == Event::Type::MouseEntered)
		{
			if (m_Hovering)
				m_Hovering->AddEvent(_copy);

			return;
		}

		// If the mouse moved or pressed, update the hovering and focussed
		if (e.type == Event::Type::MouseDragged || e.type == Event::Type::MouseMoved || e.type == Event::Type::MousePressed)
			this->Determine(e);

		// Send events to hovering and focussed if it's not the same component.
		if (m_Hovering)
			m_Hovering->AddEvent(_copy);

		if (m_Hovering != m_Focussed && m_Focussed)
			m_Focussed->AddEvent(_copy2);

		// If the mouse is released update the hovering and focussed after sending the events.
		if (e.type == Event::Type::MouseReleased)
			this->Determine(e);
	};

	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT && m_Hovering && m_Hovering->HoveringDrag())
			m_Dragging = m_Hovering,
			m_InsertIndex = constrain(GetIndex(e.y), 0, m_EffectCount - 1);;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		if (m_Dragging)
			m_InsertIndex = constrain(GetIndex(e.y), 0, m_EffectCount - 1);
	};

	m_Listener += [this](Event::MouseReleased& e)
	{
		if (m_Dragging)
		{
			int _myIndex = 0;
			int _index = 0;
			for (auto& i : m_Effects)
			{
				if (i.get() == m_Dragging)
				{
					_myIndex = _index;
					break;
				}
				_index++;
			}
			move(m_Effects, _index, m_InsertIndex);
			m_Dragging = nullptr;
			m_InsertIndex = -1;
		}
	};

}

EffectsGroup::~EffectsGroup()
{
	m_Dead = true;
	m_Mutex.lock();
	m_Mutex.unlock();
}

EffectsGroup::operator json()
{
	json _json = json::array();
	for (auto& i : m_Effects)
		_json.push_back(*i);
	return _json;
}

void EffectsGroup::operator=(const json& json)
{
	for (auto effect : json)
	{
		auto& type = effect.at("type").get<std::string>();

		auto& _it = EffectLoader::Effects().find(type);
		if (_it != EffectLoader::Effects().end())
		{
			EffectBase* inst = (*_it).second->CreateInstance();
			auto& a = Add(inst);
			a = effect;
		}
	}
}

float EffectsGroup::NextSample(float a, int ch)
{
	Lock();
	float out = a;
	for (int i = 0; i < m_EffectCount; i++)
		out = m_Effects[i]->NextSample(out, ch);
	Unlock();
	return out;
}

void EffectsGroup::Channels(int channels)
{
	for (int i = 0; i < m_EffectCount; i++)
		m_Effects[i]->Channels(channels);

	m_Channels = channels;
}

bool EffectsGroup::Hovering()
{
	for (int i = 0; i < m_EffectCount; i++)
		if (m_Effects[i]->Hovering())
			return true;
	return false;
}

void EffectsGroup::Update(const Vec4<int>& viewport)
{

	for (int i = m_EffectCount - 1; i >= 0; i--)
	{
		if (m_Effects[i]->Delete())
		{
			Lock();
			m_EffectCount--;
			m_Effects.erase(m_Effects.begin() + i);
			m_Hovering = nullptr;
			m_Focussed = nullptr;
			Unlock();
		}
	}

	m_LayoutManager.UpdateLayoutStack({ 0, 0, Width(), Height() }, m_Effects); // Also set the cursor
	m_Cursor = 
		(m_Hovering && m_Hovering->HoveringDrag()) || m_Dragging ? GLFW_RESIZE_ALL_CURSOR : 
		m_Hovering ? m_Hovering->Cursor() :
		m_Pressed && m_LayoutManager.Cursor() == -1 ? GLFW_CURSOR_NORMAL : m_LayoutManager.Cursor();

	if (m_AutoResizeX)
		Width(m_LayoutManager.BiggestCoords().x);

	if (m_AutoResizeY)
		Height(m_LayoutManager.BiggestCoords().y);

	Component::Update(viewport.Overlap({ X(), Y(), Width(), Height() }).Translate({ X(), Y() }));

	// Only update if the viewport has actual size.
	if (m_Viewport.width == 0 || m_Viewport.height == 0)
		return;

	// Update all the components that lie within the viewport and are visible.
	for (auto& _c : m_Effects)
		_c->Update(viewport);
}

void EffectsGroup::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();

	d.Command<Translate>(Vec2<int>{ X(), Y() });
	Background(d);

	Component::Render(d);

	// Only render if the viewport has actual size.
	if (m_Viewport.width == 0 || m_Viewport.height == 0)
		return;

	// Render all the components that lie within the viewport and are visible.
	int _index = 0;
	bool _past = false;
	for (auto& _c : m_Effects)
	{
		if (m_InsertIndex == _index)
		{
			d.Command<Fill>(theme->Get(C::TextOff));
			d.Command<Quad>(Vec4<int>{_c->X(), _c->Y() + (_past ? -5 : _c->Height() + 3), _c->Width(), 2});
		}
	
		if (_c.get() == m_Dragging)
			_past = true;
		
		if (_c->Visible() &&
			_c->X() + _c->Width() >= m_Viewport.x && _c->Y() + _c->Height() >= m_Viewport.y &&
			_c->X() <= m_Viewport.x + m_Viewport.width && _c->Y() <= m_Viewport.y + m_Viewport.height)
			_c->Render(d);
		
		_index++;
	}

	d.Command<PopMatrix>();
}

void EffectsGroup::Determine(Event& e)
{
	// Determine the next hovering
	Effect* _nextHover = 0;
	for (auto& _c : m_Effects)
		if (_c->Visible())
			if (_c->WithinBounds({ e.x, e.y }))
				_nextHover = _c.get();

	// If it is different, send MouseEntered and MouseExited events.
	if (_nextHover != m_Hovering)
	{
		if (m_Hovering)
		{
			Event _e{ Event::Type::MouseExited, e.x, e.y };
			m_Hovering->AddEvent(_e);
		}

		if (_nextHover != nullptr)
		{
			Event _e{ Event::Type::MouseEntered, e.x, e.y };
			_nextHover->AddEvent(_e);
		}
		m_Hovering = _nextHover;
	}

	// If MousePressed and Hovering is not focussed, send focus events.
	if (e.type == Event::Type::MousePressed && m_Hovering != m_Focussed)
	{
		if (m_Focussed != nullptr)
		{
			Event _e{ Event::Type::Unfocused };
			m_Focussed->AddEvent(_e);
		}

		m_Focussed = m_Hovering;
		if (m_Focussed != nullptr)
		{
			Event _e{ Event::Type::Focused };
			m_Focussed->AddEvent(_e);
		}
	}
}