#pragma once
#include "pch.hpp"

class Parameter : public Component
{
public:
	Parameter(const std::string& name = "")
		: m_Name(name)
	{
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT)
			{
				m_PressMouse = Vertical() ? e.y : e.x;

				m_Dragging = true;
				m_NeedsRedraw = true;
			}
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
				m_Value += (m_Shift ? m_Mult / 4 : m_Mult) * ((e.y - m_PressMouse) / (float)(Height())), m_PressMouse = e.y;

			else
				m_Value += (m_Shift ? m_Mult / 4 : m_Mult) * ((e.x - m_PressMouse) / (float)(Width())), m_PressMouse = e.x;

			ConstrainValue();
		};

		m_Listener += [this](Event::MouseClicked& e)
		{
			if (m_Counter > 0) // Double Click
				Value(m_ResetValue);

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

	virtual void   Name(const std::string& n) { m_Name = n; }
	virtual auto   Name() -> std::string& { return m_Name; }
	virtual void   Range(const Vec2<double>& r) { m_Range = r; ConstrainValue(); }
    virtual auto   Range() -> Vec2<double> const { return m_Range; }
	virtual void   Value(double v) { m_Value = Normalize(v); ConstrainValue(); }
    virtual double Value() const { return Convert(m_Value); }
	virtual double NormalizedValue() const { return m_Value; }
	virtual void   ResetValue(double v) { m_ResetValue = v; }
	virtual void   ResetValue() { m_Value = Normalize(m_ResetValue); }
	virtual void   Multiplier(double v) { m_Mult = v; }
	virtual void   Power(double v) { m_Power = v; }
	virtual bool   Vertical() { return m_Vertical; }
	virtual void   Vertical(bool v) { m_Vertical = v; }
	virtual auto   ValueText() -> std::string& const { return m_ValueText; }
	virtual void   Unit(const std::string& str, int tenp = 0) { m_Units.emplace(tenp, str); }
	virtual void   Decimals(int d) { m_Decimals = d; }

    virtual bool Hovering() const { return m_Hovering; }

	void Update(const Vec4<int>& vp) override
	{
		m_ValueText = "";
		double _v = Value();
		int _unit = -1;
		for (auto& i : m_Units)
		{
			int _p = std::pow(10, i.first);
			if (std::abs(_v) >= _p)
				_unit = i.first;

			if (i.first == 0 && _unit == -1)
				_unit = 0;
		}

		if (_unit != -1)
		{
			auto& i = m_Units[_unit];
			char s[10];
			int _p = std::pow(10, _unit);
			std::sprintf(s, (std::string("%.") + std::to_string(m_Decimals) + "f").c_str(), _v / _p);
			m_ValueText += s;
			m_ValueText += i;
		}
		else
		{
			char s[10];
			std::sprintf(s, (std::string("%.") + std::to_string(m_Decimals) + "f").c_str(), _v);
			m_ValueText += s;
		}

		Component::Update(vp);
	}

	void Render(CommandCollection& d) override
	{
		if (m_Counter > 0)
			m_Counter--;

		NeedsRedraw(false);
	};

protected:
	int m_Counter = 0,
		m_Decimals = 1;

    Vec2<double> m_Range{ 0, 100 };

    double m_Value = 0,
        m_PressMouse = 0,
		m_Power = 1,
		m_ResetValue = 0,
		m_Mult = 1;

    bool m_Hovering = false,
		m_Vertical = true,
		m_Dragging = false,
		m_Shift = false;

	std::string m_ValueText, m_Name;
	std::unordered_map<int, std::string> m_Units;

    void ConstrainValue() { m_Value = constrain(m_Value, 0, 1); }
	double Convert(double v) const { return std::powf(v, m_Power) * (m_Range.end - m_Range.start) + m_Range.start; }
	double Normalize(double v) const { return std::powf((v - m_Range.start) / (m_Range.end - m_Range.start), 1.0 / m_Power); }
};

template<typename Graphics>
class SliderBase : public Parameter
{
public:
	using Parameter::Parameter;

	void Render(CommandCollection& d) override
	{
		Parameter::Render(d);
		Graphics::Render(*this, d);
	};
};

