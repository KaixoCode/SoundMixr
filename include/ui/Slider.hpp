#pragma once
#include "pch.hpp"

class ParameterComponent : public Component
{
public:
	ParameterComponent(Parameter& param)
		: m_Parameter(param)
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
				Value(Value() + (m_Shift ? Multiplier() / 4 : Multiplier()) * ((e.y - m_PressMouse) / (float)(Height()))), m_PressMouse = e.y;

			else
				Value(Value() + (m_Shift ? Multiplier() / 4 : Multiplier()) * ((e.x - m_PressMouse) / (float)(Width()))), m_PressMouse = e.x;
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

	virtual void   Name(const std::string& n) { m_Parameter.Name(n); }
	virtual auto   Name() -> std::string& { return m_Parameter.Name(); }
	virtual void   Range(const Vec2<double>& r) { m_Parameter.Range(r); }
    virtual auto   Range() -> Vec2<double> const { return m_Parameter.Range(); }
	virtual void   Value(double v) { m_Parameter.Value(v); }
    virtual double Value() const { return m_Parameter.Value(); }
	virtual double NormalizedValue() const { return m_Parameter.NormalizedValue(); }
	virtual void   NormalizedValue(double v) { m_Parameter.NormalizedValue(v); }
	virtual void   ResetValue(double v) { m_Parameter.ResetValue(v); }
	virtual void   ResetValue() { m_Parameter.ResetValue(); }
	virtual void   Multiplier(double v) { m_Parameter.Multiplier(v); }
	virtual double Multiplier() { return m_Parameter.Multiplier(); }
	virtual void   Power(double v) { m_Parameter.Power(v); }
	virtual double Power() { return m_Parameter.Power(); }
	virtual bool   Vertical() { return m_Parameter.Vertical(); }
	virtual void   Vertical(bool v) { m_Parameter.Vertical(v); }
	virtual auto   ValueText() -> std::string& const { return m_ValueText; }
	virtual void   Unit(const std::string& str, int tenp = 0) { m_Parameter.Unit(str, tenp); }
	virtual void   Decimals(int d) { m_Parameter.Decimals(d); }
	virtual int    Decimals() { return m_Parameter.Decimals(); }
	virtual bool   DisplayValue() { return m_Parameter.DisplayValue(); }
	virtual void   DisplayValue(bool v) { m_Parameter.DisplayValue(v); }
	virtual bool   DisplayName() { return m_Parameter.DisplayName(); }
	virtual void   DisplayName(bool v) { m_Parameter.DisplayName(v); }
	virtual bool   Disabled() { return m_Parameter.Disabled(); }
	virtual void   Disable() { m_Parameter.Disable(); }
	virtual void   Enable() { m_Parameter.Enable(); }

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
			std::sprintf(s, (std::string("%.") + std::to_string(m_Parameter.Decimals()) + "f").c_str(), _v / _p);
			m_ValueText += s;
			m_ValueText += i;
		}
		else
		{
			char s[10];
			std::sprintf(s, (std::string("%.") + std::to_string(m_Parameter.Decimals()) + "f").c_str(), _v);
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
	Parameter& m_Parameter;

	int m_Counter = 0;

	double m_PressMouse = 0;

    bool m_Hovering = false,
		m_Dragging = false,
		m_Shift = false;

	std::string m_ValueText;
	std::unordered_map<int, std::string> m_Units;
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

