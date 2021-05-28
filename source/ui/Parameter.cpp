#include "ui/Parameter.hpp"

ParameterBase::ParameterBase(SoundMixr::Parameter& param)
	: m_Parameter(param),
	m_Value(Emplace<SMXRTextBox>())
{
	m_Size = { param.Size().width, param.Size().height };
	m_Value.Background(Color{ 0, 0, 0, 0 });
	m_Value.AlignLines(Align::CENTER);
	m_Value.Hide();

	m_Value.Listener() += [this](Event::Unfocused& e) 
	{
		auto content = m_Value.Content();
		std::regex reg{ "[^\\d\\.\\-]+" };
		std::string out = std::regex_replace(content, reg, "");
		m_Value.Hide();
		try {
			double i = std::stod(out);
			Value(i);
		}
		catch (std::invalid_argument const& e) {
		}
		catch (std::out_of_range const& e) {
		}
	};

	m_Value.Listener() += [this](Event::KeyTyped& e)
	{
		if (e.key == Key::ENTER && Focused())
		{
			Event e{ Event::Type::Unfocused };
			m_Value.AddEvent(e);
			m_Value.Focused(false);
			m_FocusedComponent = nullptr;
		};
	};

	Listener() += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT)
			m_PressBox = 100;
	};

	Listener() += [this](Event::MouseClicked& e)
	{
		if (e.button == Event::MouseButton::LEFT)
		{
			if (m_PressBox < 80)
			{
				m_Value.Focused(true);
				m_FocusedComponent = &m_Value;
				m_Value.Displayer().Container().Select({ 0, (int)m_Value.Content().length() });
				m_Value.Visible(true);
			}
			m_PressBox = 0;
		}
	};

	m_Menu.Clear();
	m_Menu.ButtonSize({ 160, 20 });
	m_Listener += [&](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT && !m_Value.Hovering())
		{
			m_PressMouse = Vertical() ? e.y : e.x;

			m_Dragging = true;
			m_NeedsRedraw = true;
		}

		if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
		{
			if (m_Menu.Components().size() == 0)
			{
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Parameter.Name()).Disable();
				m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { ResetValue(); }, "Reset Value");
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { ResetValue(Value()); }, "Set Reset Value");
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { ResetValue(DefaultReset()); }, "Default Reset Value");
				m_Menu.Emplace<MenuDivider>(160, 1, 0, 2);
				m_LinkButton = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Linking, "Link Midi");
				m_UnlinkButton = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
					[&] { 
						m_Parameter.MidiLink({ -1, -1, -1 });
						m_LinkButton->Name("Link Midi");
						m_UnlinkButton->Disable(); 
					}, "Unlink");
				m_UnlinkButton->Disable();
			}
			auto& link = m_Parameter.MidiLink();
			if (link.control != -1)
				m_UnlinkButton->Enable();
			if (link.control == -1)
				m_UnlinkButton->Disable();
			if (link.control == -1)
				m_LinkButton->Name("Link Midi");
			else
				m_LinkButton->Name("Linked: " + std::to_string(link.control) + ":" + std::to_string(link.channel) + ":" + std::to_string(link.device));
			RightClickMenu::Get().Open(&m_Menu);
		}
	};

	m_Listener += [this](Event::Unfocused& e)
	{
		m_Linking = false;
	};

	m_Listener += [this](Event::MouseReleased& e)
	{
		m_Dragging = false;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		if (e.button != Event::MouseButton::LEFT || !m_Dragging)
			return;

		m_NeedsRedraw = true;
		if (Vertical())
			NormalizedValue(NormalizedValue() + (m_Shift ? Multiplier() / 8 : Multiplier()) * ((e.y - m_PressMouse) / (float)(Height()))), m_PressMouse = e.y;

		else
			NormalizedValue(NormalizedValue() + (m_Shift ? Multiplier() / 8 : Multiplier()) * ((e.x - m_PressMouse) / (float)(Width()))), m_PressMouse = e.x;
	};

	m_Listener += [this](Event::MouseClicked& e)
	{
		if (m_Counter > 0 && !m_Value.Hovering()) // Double Click
			ResetValue();

		if (e.button == Event::MouseButton::LEFT && !m_Value.Hovering())
			m_Counter = 20;
	};

	m_Listener += [this](Event::MouseEntered& e)
	{
		m_Hovering = true;
	};

	m_Listener += [this](Event::MouseExited& e)
	{
		m_Hovering = false;
	};

	m_Listener += [this](Event::KeyPressed& e)
	{
		if (e.key == VK_SHIFT)
			m_Shift = true;

		if (!Focused())
			return;

		double amt = 0.01;
		if (e.keymod & Event::Mod::CONTROL)
			amt *= 4;
		else if (e.keymod & Event::Mod::SHIFT)
			amt *= 0.1;

		if (e.key == Key::UP || e.key == Key::RIGHT)
			m_Parameter.NormalizedValue(m_Parameter.NormalizedValue() + amt);

		else if (e.key == Key::DOWN || e.key == Key::LEFT)
			m_Parameter.NormalizedValue(m_Parameter.NormalizedValue() - amt);
	};

	m_Listener += [this](Event::KeyReleased& e)
	{
		if (e.key == VK_SHIFT)
			m_Shift = false;
	};

	m_Listener += [this](Event::MouseWheel& e)
	{
		if (Hovering() && e.wheelmod == Event::Mod::CONTROL)
			m_Parameter.NormalizedValue(m_Parameter.NormalizedValue() + e.amount * 0.0001);

		if (Hovering() && e.wheelmod == (Event::Mod::CONTROL | Event::Mod::SHIFT))
			m_Parameter.NormalizedValue(m_Parameter.NormalizedValue() + e.amount * 0.00001);
	};
}

void ParameterBase::Update(const Vec4<int>& vp) 
{
	if (m_PressBox > 1)
		m_PressBox--;

	m_Size = { m_Parameter.Size().width, m_Parameter.Size().height };
	m_Pos = { m_Parameter.Position().x, m_Parameter.Position().y };

	if (Disabled())
		m_Value.TextColor(ThemeT::Get().knob_disabled_value_text);
	else if (Dragging())
		m_Value.TextColor(ThemeT::Get().knob_active_value_text);
	else if (Hovering())
		m_Value.TextColor(ThemeT::Get().knob_hovering_value_text);
	else
		m_Value.TextColor(ThemeT::Get().knob_idle_value_text);

	if (!DisplayValue())
		m_Value.Visible(false);
	if (m_Parameter.Type() == SoundMixr::ParameterType::Knob)
	{
		m_Dims = { m_Pos.x - 15, m_Pos.y - 15, m_Size.width + 30, m_Size.height + 15 };
	}
	else
	{
		m_Dims = { m_Pos.x, m_Pos.y, m_Size.width, m_Size.height };
	}

	m_Value.Size({ m_Dims.width, 20 });
	m_Value.Position({ m_Dims.x, m_Dims.y });
	
	if (m_Parameter.Type() == SoundMixr::ParameterType::Slider)
	{
		m_Value.Position({ m_Dims.x, m_Dims.y + m_Dims.height / 2 - 10 });
	}

	m_ValueText = "";
	double _v = Value();
	int _unit = -9999999;
	for (auto& i : Units())
	{
		if (i.first == INT_MAX && _v == 0)
		{
			_unit = INT_MAX;
			break;
		}

		if (i.first == -1 && _unit == -9999999 && _v < 0)
			_unit = -1;

		if (i.first == 0 && _unit == -9999999)
			_unit = 0;

		if (i.first < 0)
		{
			int _p = std::pow(10, -i.first - 1);
			if (_v < 0 && std::abs(std::round(_v)) >= _p && _unit <= 0)
				_unit = i.first;
		}
		else
		{
			int _p = std::pow(10, i.first);
			if (_v >= 0 && std::abs(std::round(_v)) >= _p && _unit >= 0)
				_unit = i.first;
		}
	}

	if (_unit != -9999999)
	{
		auto& i = Units()[_unit];
		char s[30];
		int _p = std::pow(10, _unit < 0 ? -_unit - 1 : _unit);
		int d = m_Parameter.Decimals();
		if (d == -1)
		{
			d += 1;
			if (_v < 0)
				_v = -_v;

			if (_v == 0)
				std::sprintf(s, "");
			else
				std::sprintf(s, (std::string("%.") + std::to_string(d) + "f").c_str(), _v / _p);
		}
		else
			std::sprintf(s, (std::string("%.") + std::to_string(d) + "f").c_str(), _v / _p);
		m_ValueText += s;
		m_ValueText += i;
	}
	else
	{
		char s[30];
		std::sprintf(s, (std::string("%.") + std::to_string(m_Parameter.Decimals()) + "f").c_str(), _v);
		m_ValueText += s;
	}

	if (!m_Value.Focused())
		m_Value.Content(m_ValueText);
	Component::Update(vp);
	m_Value.Update(vp);
}

void ParameterBase::Render(CommandCollection& d)
{
	if (m_Counter > 0)
		m_Counter--;

	NeedsRedraw(false);
	Component::Render(d);
	m_Value.Render(d);
};
