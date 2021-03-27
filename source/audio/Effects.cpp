#include "audio/Effects.hpp"
#include "FileDialog.hpp"

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

Effect::~Effect()
{
	m_Effect->Destroy();
}

Effect::Effect(Effects::EffectBase* effect)
	: m_Effect(effect), m_Channels(0)
{
	Init();
	if (effect)
		m_Name = effect->Name();

	m_RealHeight = -1;
	m_MinimB = &Emplace<Button<NOTHING, ButtonType::Toggle>>(&m_Small, "");
	m_Enable = &Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(&m_Enabled, "");
	m_Enable->Size({ 18, 18 });
	m_MinimB->Size({ 24, 24 });

	m_Menu.ButtonSize({ 160, 20 });
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Effect->Name()).Disable();
	m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Enabled, "Enable");
	m_Minim = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Small, "Minimize");
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
				nlohmann::json _json = operator nlohmann::json();
				_of << _json;
				_of.close();

				m_Preset = path;
				m_Name = m_Effect->Name() + " - " + m_Preset.stem().string();
				m_SavePreset->Enable();
			}
			catch (...)
			{
				LOG("Failed to save preset");
			}
		}, "Save As Preset...");
	m_SavePreset = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this]
		{
			try
			{
				std::ofstream _of{ m_Preset };
				nlohmann::json _json = operator nlohmann::json();
				_of << _json;
				_of.close();
			}
			catch (...)
			{
				LOG("Failed to save preset");
			}
		}, "Save Preset");
	m_SavePreset->Disable();
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

				*this = _json;
				m_SavePreset->Enable();
				m_Preset = path;
				m_Name = m_Effect->Name() + " - " + m_Preset.stem().string();
			}
			catch (...)
			{
				LOG("Failed to open preset");
			}
		}, "Open Preset...");
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this]
		{
			try
			{
				m_SavePreset->Disable();
				m_Preset = "";
				m_Name = m_Effect->Name();

				for (auto& i : m_Effect->Objects())
					i->Default();
			}
			catch (...)
			{
				LOG("Failed to open preset");
			}
		}, "Default Preset");
	m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { m_Delete = true; }, "Remove");
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
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
		if (e.x > 24 && e.y > Height() - 24 && e.x < Width() - 24)
			m_HoveringDrag = true;
	};

	m_Listener += [this](Event& e)
	{
		if (e.x - X() >= 24 && e.x - X() <= Width() - 24 && m_Small && e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
			e.y = Y() + 1;
	};
}

void Effect::Bypass(bool b)
{
	m_Bypass = b;
	if (!m_Bypass)
	{
		m_Enable->Enable();
		if (!m_Enabled)
			return;

		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Enable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Enable();
		m_Effect->Update();
	}
	else
	{
		for (auto& i : Components())
			if (i.get() == m_MinimB)
				continue;
			else if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Disable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Disable();
	}
}

void Effect::Update(const Vec4<int>& v) 
{
	if (m_Enabled && !m_PEnabled && !m_Bypass)
	{
		m_PEnabled = true;
		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Enable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Enable();
		m_Effect->Update();
	}
	else if (!m_Enabled && m_PEnabled && !m_Bypass)
	{
		m_PEnabled = false;
		for (auto& i : Components())
			if (i.get() == m_Enable || i.get() == m_MinimB)
				continue;
			else if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Disable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Disable();
	}

	if (m_Small && !m_PSmall)
	{
		m_PSmall = true;
		m_RealHeight = Height();
		Height(24);
	}
	else if (!m_Small && m_PSmall)
	{
		m_PSmall = false;
		if (m_RealHeight == -1)
			m_RealHeight = Height();

		Height(m_RealHeight);
	}
	

	m_Size.height = m_Small ? 24 : m_Effect->Height() + 24;
	m_Enable->Position({ 3, Height() - 21 });
	m_MinimB->Position({ Width() - 24, Height() - 24 });
	Background(ThemeT::Get().effect_background);
	Panel::Update(v);
}

void Effect::Render(CommandCollection& d)
{
	using namespace Graphics;
	if (!m_Small)
		Panel::Render(d);

	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	d.Command<Fill>(ThemeT::Get().effect_title_bar);
	d.Command<Quad>(Vec4<int>{ 0, Height() - 24, Width(), 24 });
	d.Command<Font>(Fonts::Gidole16, 16.0f);

	if (m_Enabled)
		d.Command<Fill>(ThemeT::Get().effect_title_text);
	else
		d.Command<Fill>(ThemeT::Get().effect_title_text_off);

	d.Command<TextAlign>(Align::LEFT, Align::CENTER);
	d.Command<Text>(&m_Name, Vec2<int>{ 30, Height() - 12});
	
	d.Command<Fill>(ThemeT::Get().effect_minimize_button);
	if (!m_Small)
		d.Command<Quad>(Vec4<int>{Width() - 17, Height() - 13, 10, 2});
	else
		d.Command<Triangle>(Vec4<int>{Width() - 12, Height() - 12, 10, 10}, 270.0f);

	m_Enable->Render(d);
	m_MinimB->Render(d);

	if (!m_Small)
	{
		d.Command<Fill>(ThemeT::Get().effect_divider);
		for (auto& i : m_Dividers)
			d.Command<Quad>(i);
	}

	d.Command<PopMatrix>();
}

Effect::operator nlohmann::json()
{
	nlohmann::json _json = *m_Effect;
	_json["enabled"] = m_Enable->Active();
	_json["small"] = m_Small;
	_json["preset"] = m_Preset.string();
	return _json;
}

void Effect::operator=(const nlohmann::json& json)
{
	m_Enable->Active(json.at("enabled").get<bool>());
	m_Small = json.at("small").get<bool>();
	m_Preset = json.at("preset").get<std::string>();

	if (m_Preset != "")
	{
		m_Name = m_Effect->Name() + " - " + m_Preset.stem().string();

		m_SavePreset->Enable();
	}

	*m_Effect = json;
}

void Effect::Init()
{
	InitDiv(m_Effect->Div(), { 0, 0, 300, m_Effect->Height() });
}

void Effect::InitDiv(Effects::Div& div, const Vec4<int>& dim)
{
	if (div.DivType() == Effects::Div::Type::Object)
		SetObject(div, dim);

	else
		if (div.Align() == Effects::Div::Alignment::Horizontal)
		{
			// Get all the sizes of the sub-divs
			std::vector<int> sizes;
			int width = dim.width, amt = 0, obamt = 0;
			for (auto& i : div.Divs())
				if (i->DivSize() == Effects::Div::AUTO)
				{
					if (i->DivType() == Effects::Div::Type::Object)
						sizes.push_back(i->Object().Size().width + i->Padding()), width -= i->Object().Size().width + i->Padding(), obamt++;
					else
						sizes.push_back(0), amt++;
				}
				else
					width -= i->DivSize(), sizes.push_back(-i->DivSize());

			// See what's left to divide
			int x = dim.x;
			int w = width;
			if (amt)
				w /= amt;
			else if (obamt)
				w /= obamt;
			
			// If there's some space left, either give it to divs or objects
			if (w > 0)
				for (auto& i : sizes)
					if (amt && i == 0) // Give space to div with no given size
						i = w;
					else if (obamt && i > 0) // Give space to object
						i += w;

			// if we're short on space, take some away from objects or divs.
			if (width < 0)
				for (auto& i : sizes)
					if (i > 0 && obamt) // Take away from object
						i -= w;
					else if (i < 0 && !obamt) // Take away from div with a given size
						i -= w;

			// Set sizes and positions
			for (int i = 0; i < div.Divs().size(); i++)
			{
				if (div.Dividers() && i != 0)
					m_Dividers.emplace_back(Vec4<int>{ x, dim.y + 8, 1, dim.height - 16 });

				if (sizes[i] < 0) // If div with given space, size is negative so negate
					InitDiv(*div.Divs()[i], { x + div.Padding(), dim.y + div.Padding(), -sizes[i] - 2 * div.Padding(), dim.height - 2 * div.Padding() }), x += -sizes[i];
				else // Otherwise just recurse
					InitDiv(*div.Divs()[i], { x + div.Padding(), dim.y + div.Padding(), sizes[i] - 2 * div.Padding(), dim.height - 2 * div.Padding() }), x += sizes[i];
			}
		}
		else
		{
			// Get all the sizes of the sub-divs
			std::vector<int> sizes;
			int height = dim.height, amt = 0, obamt = 0;
			for (auto& i : div.Divs())
				if (i->DivSize() == Effects::Div::AUTO)
				{
					if (i->DivType() == Effects::Div::Type::Object)
						sizes.push_back(i->Object().Size().height + i->Padding()), height -= i->Object().Size().height + i->Padding(), obamt++;
					else
						sizes.push_back(0), amt++;
				}
				else
					height -= i->DivSize(), sizes.push_back(-i->DivSize());

			// See what's left to divide
			int y = dim.y;
			int h = height;
			if (amt)
				h /= amt;
			else if (obamt)
				h /= obamt;

			// If there's some space left, either give it to divs or objects
			if (h > 0)
				for (auto& i : sizes)
					if (amt && i == 0) // Give space to div with no given size
						i = h;
					else if (obamt && i > 0) // Give space to object
						i += h;

			// if we're short on space, take some away from objects or divs.
			if (height < 0)
				for (auto& i : sizes)
					if (i > 0 && obamt) // Take away from object
						i += h;
					else if (i < 0 && !obamt) // Take away from div with a given size
						i += h;

			// Set sizes and positions
			for (int i = 0; i < div.Divs().size(); i++)
			{
				if (div.Dividers() && i != 0)
					m_Dividers.emplace_back(Vec4<int>{ dim.x + 8, y, dim.width - 16, 1 });

				if (sizes[i] < 0) // If div with given space, size is negative so negate
					InitDiv(*div.Divs()[i], { dim.x + div.Padding(), y + div.Padding(), dim.width - 2 * div.Padding(), -sizes[i] - 2 * div.Padding() }), y += -sizes[i];
				else // Otherwise just recurse
					InitDiv(*div.Divs()[i], { dim.x + div.Padding(), y + div.Padding(), dim.width - 2 * div.Padding(), sizes[i] - 2 * div.Padding() }), y += sizes[i];
			}
		}
}

void Effect::SetObject(Effects::Div& div, const Vec4<int>& dim)
{
	// Get the object from the div
	Effects::Object* object = &div.Object();

	// Calculate the position using the alignment
	Vec2<int> position = { dim.x, dim.y };
	if (div.Align() == Effects::Div::Alignment::Center)
		position += { dim.width / 2 - object->Size().width / 2, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == Effects::Div::Alignment::Right)
		position += { dim.width - object->Size().width, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == Effects::Div::Alignment::Left)
		position += { 0, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == Effects::Div::Alignment::Bottom)
		position += { dim.width / 2 - object->Size().width / 2, 0 };
	else if (div.Align() == Effects::Div::Alignment::Top)
		position += { dim.width / 2 - object->Size().width / 2, dim.height - object->Size().height };


	// Determine type and add to effect.
	auto xy = dynamic_cast<Effects::XYController*>(object);
	if (xy != nullptr)
	{
		Emplace<XYController>(*xy), xy->Position({ position.x, position.y });
		return;
	}

	auto fc = dynamic_cast<Effects::FilterCurve*>(object);
	if (fc != nullptr)
	{
		Emplace<FilterCurve>(*fc), fc->Position({ position.x, position.y });
		return;
	}

	auto sc = dynamic_cast<Effects::SimpleFilterCurve*>(object);
	if (sc != nullptr)
	{
		Emplace<SimpleFilterCurve>(*sc), sc->Position({ position.x, position.y });
		return;
	}

	auto rb = dynamic_cast<Effects::RadioButton*>(object);
	if (rb != nullptr)
	{
		Emplace<RadioButton>(*rb, m_RadioButtonKeys, m_RadioButtons), rb->Position({ position.x, position.y });
		return;
	}

	auto dy = dynamic_cast<Effects::DynamicsSlider*>(object);
	if (dy != nullptr)
	{
		Emplace<DynamicsSlider>(*dy), dy->Position({ position.x, position.y });
		return;
	}

	auto vs = dynamic_cast<Effects::VolumeSlider*>(object);
	if (vs != nullptr)
	{
		Emplace<VolumeSlider>(*vs), vs->Position({ position.x, position.y - 5 });
		return;
	}

	auto param = dynamic_cast<Effects::Parameter*>(object);
	if (param != nullptr)
	{
		if (param->Type() == Effects::ParameterType::Slider)
			Emplace<Slider>(*param), param->Position({ position.x, position.y });
		else if (param->Type() == Effects::ParameterType::Knob)
			Emplace<Knob>(*param), param->Position({ position.x, position.y });
		return;
	}

	auto dd = dynamic_cast<Effects::DropDown*>(object);
	if (dd != nullptr)
	{
		Emplace<DropDown<int, DropdownButton>>(*dd), dd->Position({ position.x, position.y });
		return;
	}

	auto toggle = dynamic_cast<Effects::ToggleButton*>(object);
	if (toggle != nullptr)
	{
		Emplace<ToggleButton>(*toggle), toggle->Position({ position.x, position.y });
		return;
	}
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

EffectsGroup::operator nlohmann::json()
{
	nlohmann::json _json = nlohmann::json::array();
	for (auto& i : m_Effects)
	{
		nlohmann::json _j = *i;
		_j["type"] = i->Name();
		_json.push_back(_j);
	}
	return _json;
}

void EffectsGroup::operator=(const nlohmann::json& json)
{
	for (auto effect : json)
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
				auto& a = Add(e);
				a = effect;
			}
		}
		}
		catch(...)
		{ }
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

Effect& EffectsGroup::Add(Effects::EffectBase* e)
{
	int p = m_EffectCount;
	m_EffectCount = 0;
	auto& a = m_Effects.emplace_back(std::make_unique<Effect>(e));
	a->Channels(m_Channels);
	m_EffectCount = p + 1;
	return *a;
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
		if (_c->Visible() &&
			_c->X() + _c->Width() >= m_Viewport.x && _c->Y() + _c->Height() >= m_Viewport.y &&
			_c->X() <= m_Viewport.x + m_Viewport.width && _c->Y() <= m_Viewport.y + m_Viewport.height)
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
			d.Command<Fill>(ThemeT::Get().effect_drag_divider);
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

int EffectsGroup::GetIndex(int y) const
{
	int index = 0;
	int _ri = 0;
	for (auto& i : m_Effects)
	{
		if (y < i->Y() + i->Height() / 2)
			_ri = index + 1;

		index++;
	}

	if (m_Dragging && y < m_Dragging->Y() + m_Dragging->Height() / 2)
		_ri -= 1;

	return _ri;
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