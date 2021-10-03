#include "Gui/Parameter.hpp"

Parameter::Parameter(const Settings& settings)
	: settings(settings)
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
			? (e.pos.y - m_PrevPos) / height
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
}