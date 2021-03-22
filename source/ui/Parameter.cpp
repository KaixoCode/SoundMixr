#include "ui/Parameter.hpp"

ParameterBase::~ParameterBase()
{
	Midi::Get().Remove(m_Callback, m_CallbackId);
}

ParameterBase::ParameterBase(Effects::Parameter& param)
	: m_Parameter(param)
{
	m_Callback = [this](Midi::Event::ControlChange& a)
	{ 
		if (m_Linking)
		{
			if (m_UnlinkButton)
				m_UnlinkButton->Enable();
			if (m_LinkButton)
				m_LinkButton->Name(std::string("Link Midi - Linked: ") + std::to_string((int)a.control));
			m_Parameter.MidiLink({ a.channel, a.control, a.device });
			m_Linking = false;
		}

		if (m_Parameter.MidiLink() == Effects::MidiCCLink{ a.channel, a.control, a.device })
		{
			NormalizedValue(a.value / 127.0);
		}
	};
	m_CallbackId = (Midi::Get() += m_Callback);
	m_Menu.Clear();
	m_Menu.ButtonSize({ 160, 20 });
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT)
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
				m_LinkButton = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_Linking, "Link Midi");
				m_UnlinkButton = &m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { m_MidiLink = -1; m_LinkButton->Name("Link Midi"); m_UnlinkButton->Disable(); }, "Unlink");
				m_UnlinkButton->Disable();
			}
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
		if (m_Counter > 0) // Double Click
			ResetValue();

		if (e.button == Event::MouseButton::LEFT)
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
	};

	m_Listener += [this](Event::KeyReleased& e)
	{
		if (e.key == VK_SHIFT)
			m_Shift = false;
	};
}

void ParameterBase::Update(const Vec4<int>& vp) 
{
	m_Size = { m_Parameter.Size().width, m_Parameter.Size().height };
	m_Pos = { m_Parameter.Position().x, m_Parameter.Position().y };

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
			if (_v < 0 && std::abs(_v) >= _p && _unit <= 0)
				_unit = i.first;
		}
		else
		{
			int _p = std::pow(10, i.first);
			if (_v >= 0 && std::abs(_v) >= _p && _unit >= 0)
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

	Component::Update(vp);
}

void ParameterBase::Render(CommandCollection& d)
{
	if (m_Counter > 0)
		m_Counter--;

	NeedsRedraw(false);
};
