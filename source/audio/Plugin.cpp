#include "audio/Plugin.hpp"

Plugin::Plugin(SoundMixr::PluginBase* plugin)
	: m_Plugin(plugin)
{
	// Init the div
	Init();

	m_Enable = &Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(&m_Enabled, "");
	m_Enable->Size({ 18, 18 });
}

void Plugin::GetMenu(MenuBase& menu)
{
	// Generate rightclick menu
	menu.Clear();
	menu.ButtonSize({ 160, 20 });
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Plugin->Name()).Disable();
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Enabled, "Enable");
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { SavePreset(); }, "Save Preset").Enabled(m_Preset != "");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { SavePresetAs();}, "Save Preset As...");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { LoadPreset(); }, "Load Preset...");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { DefaultPreset(); }, "Default Preset");
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
}

void Plugin::Bypass(bool b)
{
	m_Bypass = b;
	if (!m_Bypass)
	{
		// Enable the enable button
		m_Enable->Enable();

		// If not enabled, don't enable the parameters/buttons.
		if (!m_Enabled)
			return;

		// Otherwise enable the parameters/buttons again
		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Enable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Enable();

		// And trigger an update in the effect.
		m_Plugin->Update();
	}
	else
	{
		// When bypassing, disable all parameters/buttons 
		// except the minimize button.
		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Disable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Disable();
	}
}

void Plugin::UpdatePlugin()
{
	// Only update if not bypassed or disabled.
	if (m_Bypass || !m_Enabled)
		return;

	m_Plugin->Update();
}

void Plugin::Update(const Vec4<int>& v)
{
	// Makes sure the parameters/buttons are enabled/disabled
	// whenever they should be.
	if (m_Enabled && !m_PEnabled && !m_Bypass)
	{
		m_PEnabled = true;
		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Enable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Enable();
		m_Plugin->Update();
	}
	else if (!m_Enabled && m_PEnabled && !m_Bypass)
	{
		m_PEnabled = false;
		for (auto& i : Components())
			if (auto p = dynamic_cast<ParameterBase*>(i.get()))
				p->Disable();
			else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
				p->Disable();
	}

	m_Enable->Position({ 3, Height() - 21 });
	Background(ThemeT::Get().effect_background);
	Panel::Update({ Position(), Size() });
}

void Plugin::Render(CommandCollection& d)
{
	using namespace Graphics;

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

	m_Enable->Render(d);

	d.Command<Fill>(ThemeT::Get().effect_divider);
	for (auto& i : m_Dividers)
		d.Command<Quad>(i);

	d.Command<PopMatrix>();
}

Plugin::operator nlohmann::json()
{
	nlohmann::json _json = *m_Plugin;
	_json["preset"] = m_Preset.string();
	_json["enabled"] = m_Enable->Active();
	_json["type"] = Name();
	return _json;
}

void Plugin::operator=(const nlohmann::json& json)
{
	m_Enable->Active(json.at("enabled").get<bool>());
	m_Preset = json.at("preset").get<std::string>();

	if (m_Preset != "")
		m_Name = m_Plugin->Name() + " - " + m_Preset.stem().string();

	*m_Plugin = json;
}


void Plugin::SavePreset() 
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
}

void Plugin::SavePresetAs() 
{
	RightClickMenu::Get().Close();
	std::string path = FileDialog::SaveFile(FileType::JSON);
	if (path.empty())
		return;
	try
	{
		std::ofstream _of{ path };
		nlohmann::json _json = operator nlohmann::json();
		_of << _json;
		_of.close();

		m_Preset = path;
		m_Name = m_Plugin->Name() + " - " + m_Preset.stem().string();
	}
	catch (...)
	{
		LOG("Failed to save preset");
	}
}

void Plugin::LoadPreset() 
{
	RightClickMenu::Get().Close();
	std::string path = FileDialog::OpenFile(FileType::JSON);
	if (path.empty())
		return;

	try
	{
		std::ifstream _in{ path };
		nlohmann::json _json;
		_json << _in;
		_in.close();

		if (_json.at("type") != Name())
			return;

		*this = _json;
		m_Preset = path;
		m_Name = m_Plugin->Name() + " - " + m_Preset.stem().string();
	}
	catch (...)
	{
		LOG("Failed to load preset");
	}
}

void Plugin::DefaultPreset() 
{
	try
	{
		m_Preset = "";
		m_Name = m_Plugin->Name();

		for (auto& i : m_Plugin->Objects())
			i->Default();
	}
	catch (...)
	{
		LOG("Failed to set to default");
	}
}


void Plugin::Init()
{
	InitDiv(m_Plugin->Div(), { 0, 0, 300, m_Plugin->Height() });
}

void Plugin::InitDiv(SoundMixr::Div& div, const Vec4<int>& dim)
{
	// If it's an object, set the object
	if (div.DivType() == SoundMixr::Div::Type::Object)
		SetObject(div, dim);

	// Otherwise divide the space between divs and recurse to next divs.
	else

		// Dividing depends on the alignment of the div.
		if (div.Align() == SoundMixr::Div::Alignment::Horizontal)
		{
			// Get all the sizes of the sub-divs
			std::vector<int> sizes;
			int width = dim.width, amt = 0, obamt = 0;
			for (auto& i : div.Divs())
				if (i->DivSize() == SoundMixr::Div::AUTO)
				{
					if (i->DivType() == SoundMixr::Div::Type::Object)
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
				if (i->DivSize() == SoundMixr::Div::AUTO)
				{
					if (i->DivType() == SoundMixr::Div::Type::Object)
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

void Plugin::SetObject(SoundMixr::Div& div, const Vec4<int>& dim)
{
	// Get the object from the div
	SoundMixr::Object* object = &div.Object();

	// Calculate the position using the alignment
	Vec2<int> position = { dim.x, dim.y };
	if (div.Align() == SoundMixr::Div::Alignment::Center)
		position += { dim.width / 2 - object->Size().width / 2, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == SoundMixr::Div::Alignment::Right)
		position += { dim.width - object->Size().width, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == SoundMixr::Div::Alignment::Left)
		position += { 0, dim.height / 2 - object->Size().height / 2 };
	else if (div.Align() == SoundMixr::Div::Alignment::Bottom)
		position += { dim.width / 2 - object->Size().width / 2, 0 };
	else if (div.Align() == SoundMixr::Div::Alignment::Top)
		position += { dim.width / 2 - object->Size().width / 2, dim.height - object->Size().height };

	// Determine type and add to effect.
	Component* ob = nullptr;
	auto xy = dynamic_cast<SoundMixr::XYController*>(object);
	if (xy != nullptr)
	{
		ob = &Emplace<XYController>(*xy), xy->Position({ position.x, position.y });
		goto theback;
	}

	auto fc = dynamic_cast<SoundMixr::FilterCurve*>(object);
	if (fc != nullptr)
	{
		ob = &Emplace<FilterCurve>(*fc), fc->Position({ position.x, position.y });
		goto theback;
	}

	auto sc = dynamic_cast<SoundMixr::SimpleFilterCurve*>(object);
	if (sc != nullptr)
	{
		ob = &Emplace<SimpleFilterCurve>(*sc), sc->Position({ position.x, position.y });
		goto theback;
	}

	auto rb = dynamic_cast<SoundMixr::RadioButton*>(object);
	if (rb != nullptr)
	{
		ob = &Emplace<RadioButton>(*rb, m_RadioButtonKeys, m_RadioButtons), rb->Position({ position.x, position.y });
		goto theback;
	}

	auto dy = dynamic_cast<SoundMixr::DynamicsSlider*>(object);
	if (dy != nullptr)
	{
		ob = &Emplace<DynamicsSlider>(*dy), dy->Position({ position.x, position.y });
		goto theback;
	}

	auto vs = dynamic_cast<SoundMixr::VolumeSlider*>(object);
	if (vs != nullptr)
	{
		ob = &Emplace<VolumeSlider>(*vs), vs->Position({ position.x, position.y - 5 });
		goto theback;
	}

	auto param = dynamic_cast<SoundMixr::Parameter*>(object);
	if (param != nullptr)
	{
		if (param->Type() == SoundMixr::ParameterType::Slider)
			ob = &Emplace<Slider>(*param), param->Position({ position.x, position.y });
		else if (param->Type() == SoundMixr::ParameterType::Knob)
			ob = &Emplace<Knob>(*param), param->Position({ position.x, position.y });
		goto theback;
	}

	auto dd = dynamic_cast<SoundMixr::DropDown*>(object);
	if (dd != nullptr)
	{
		ob = &Emplace<DropDown<int, DropdownButton>>(*dd), dd->Position({ position.x, position.y });
		goto theback;
	}

	auto toggle = dynamic_cast<SoundMixr::ToggleButton*>(object);
	if (toggle != nullptr)
	{
		ob = &Emplace<ToggleButton>(*toggle), toggle->Position({ position.x, position.y });
		goto theback;
	}

theback:

	if (ob == nullptr)
		return;

	// Set the tab object.
	if (m_PrevObj)
	{
		m_PrevObj->TabComponent(ob);
		ob->BackTabComponent(m_PrevObj);
	}

	m_PrevObj = ob;
}

