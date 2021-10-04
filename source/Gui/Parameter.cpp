#include "Gui/Parameter.hpp"

Parameter::Parameter(const Settings& settings)
	: settings(settings),
	m_CCLink(Midi::Get() += [this](Midi::Event::ControlChange& cc)
	{
		if (m_Linking)
		{
			midi.channel = cc.channel;
			midi.device = cc.device;
			midi.cc = cc.control;
			m_Linking = false;
		}
		else if (cc.channel == midi.channel && cc.device == midi.device && cc.control == midi.cc)
		{
			float v = cc.value / 127.0;
			if (std::abs(v - 0.5) < 1 / 127.0)
				v = 0.5;
			value = Unnormalize(v);
		}
	})
{
	listener += [this](const MousePress& e)
	{
		m_PressVal = inverse((value - range.start) / (range.end - range.start));
		m_PrevPos = vertical ? e.pos.y : e.pos.x;
	};

	listener += [this](const MouseDrag& e)
	{
		if (~e.buttons & MouseButton::Left)
			return;

		float _value = vertical
			? (m_PrevPos - e.pos.y) / height
			: (e.pos.x - m_PrevPos) / width;

		m_PrevPos = vertical ? e.pos.y : e.pos.x;

		if (e.mod & EventMods::Shift)
			_value *= shift;

		m_PressVal += _value;

		if (scaling)
			value = scaling(constrain(m_PressVal, 0.f, 1.f)) * (range.end - range.start) + range.start;
	};

	listener += [this](const MouseClick& e)
	{
		if (e.button != MouseButton::Left)
			return;
		auto _now = std::chrono::steady_clock::now();
		auto _duration = std::chrono::duration_cast<std::chrono::milliseconds>(_now - m_ChangeTime).count();
		if (_duration < 500)
			value = reset;

		m_ChangeTime = _now; // Get changetime
	};

	listener += [this](const MouseRelease& e)
	{
		if (e.button == MouseButton::Right && !e.Handled())
		{
			m_Linking = false;
			bool _linked = midi.device != -1 && midi.cc != -1 && midi.channel != -1;
			menu.Clear();
			menu.push_back(new MenuButton{ {.name = name } })->State<Disabled>(true);
			menu.push_back(new Divider{});
			menu.push_back(new MenuButton{ {.callback = [this](bool) { value = reset; }, .name = "Reset Value" } });
			menu.push_back(new Divider{});
			if (_linked)
			{
				menu.push_back(new MenuButton{ {.type = Button::Toggle, .callback = [this](bool) { m_Linking = true; }, .name = "Linked: " + std::to_string(midi.cc) + ":" + std::to_string(midi.channel) + ":" + std::to_string(midi.device) } });
				menu.push_back(new MenuButton{ {.callback = [this](bool) { midi = { -1, -1, -1 }; }, .name = "Unlink Midi" } });
			}
			else
			{
				menu.push_back(new MenuButton{ {.type = Button::Toggle, .callback = [this](bool) { m_Linking = true; }, .name = "Link Midi" } });
				menu.push_back(new MenuButton{ {.name = "Unlink Midi" } })->State<Disabled>(true);
			}
			ContextMenu::Open(menu, e.pos, true);
			e.Handle();
		}
	};

	listener += [this](const Unfocus&)
	{
		m_Linking = false;
	};
}