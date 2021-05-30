#include "audio/Effect.hpp"

Effect::Effect(SoundMixr::EffectBase* effect)
		: m_Effect(effect), Plugin(effect)
{
	// Set the name to the effect name
	m_Name = effect->Name();

	// Set the size.
	m_Size.height = m_Small ? 24 : m_Effect->Height() + 24;
	m_RealHeight = m_Effect->Height() + 24;

	// Add ze buttons
	m_MinimB = &Emplace<Button<NOTHING, ButtonType::Toggle>>(&m_Small, "");
	m_MinimB->Size({ 24, 24 });

	// Keep track of when hovering over draggable area.
	m_Listener += [this](Event::MouseMoved& e)
	{
		m_HoveringDrag = false;
		if (e.x > 24 && e.y > Height() - 24 && e.x < Width() - 24)
			m_HoveringDrag = true;
	};

	// This offsets events when hovering over draggable area to prevent clicking on
	// any parameters when the effect is minimized.
	m_Listener += [this](Event& e)
	{
		if (e.x - X() >= 24 && e.x - X() <= Width() - 24 && m_Small && e.type != Event::Type::KeyPressed
			&& e.type != Event::Type::KeyReleased && e.type != Event::Type::KeyTyped)
			e.y = Y() + 1;
	};
}

void Effect::GetMenu(MenuBase& menu)
{
	// Generate rightclick menu
	menu.Clear();
	menu.ButtonSize({ 160, 20 });
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Effect->Name()).Disable();
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Enabled, "Enable");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Small, "Minimize");
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { SavePreset(); }, "Save Preset").Enabled(m_Preset != "");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { SavePresetAs();}, "Save Preset As...");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { LoadPreset(); }, "Load Preset...");
	menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { DefaultPreset(); }, "Default Preset");
	menu.Emplace<MenuDivider>(160, 1, 0, 2);
}

double Effect::Process(double s, int c)
{
	// Only process when not bypassed or disabled.
	if (m_Bypass || !m_Enabled)
		return s;

	return m_Effect->Process(s, c);
}

void Effect::Bypass(bool b)
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
		m_Effect->Update();
	}
	else
	{
		// When bypassing, disable all parameters/buttons 
		// except the minimize button.
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

	// Small was toggled on.
	if (m_Small && !m_PSmall)
	{
		NeedsRecalc(true);
		m_PSmall = true;
		m_RealHeight = Height();
		Height(24);
	}

	// Small was toggled off.
	else if (!m_Small && m_PSmall)
	{
		NeedsRecalc(true);
		m_PSmall = false;
		if (m_RealHeight == -1)
			m_RealHeight = Height();

		Height(m_RealHeight);
	}

	m_Size.height = m_Small ? 24 : m_Effect->Height() + 24;
	m_Enable->Position({ 3, Height() - 21 });
	m_MinimB->Position({ Width() - 24, Height() - 24 });
	Background(ThemeT::Get().effect_background);
	Panel::Update({Position(), Size()});
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
	_json["type"] = Name();
	return _json;
}

void Effect::operator=(const nlohmann::json& json)
{
	m_Enable->Active(json.at("enabled").get<bool>());
	m_Small = json.at("small").get<bool>();
	m_Preset = json.at("preset").get<std::string>();

	if (m_Preset != "")
		m_Name = m_Effect->Name() + " - " + m_Preset.stem().string();

	*m_Effect = json;
}