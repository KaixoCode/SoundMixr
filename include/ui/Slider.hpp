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
				NormalizedValue(NormalizedValue() + (m_Shift ? Multiplier() / 4 : Multiplier()) * ((e.y - m_PressMouse) / (float)(Height()))), m_PressMouse = e.y;

			else
				NormalizedValue(NormalizedValue() + (m_Shift ? Multiplier() / 4 : Multiplier()) * ((e.x - m_PressMouse) / (float)(Width()))), m_PressMouse = e.x;
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
	virtual void   Range(const Vec2<double>& r) { m_Parameter.Range({ r.start, r.end }); }
	virtual auto   Range() -> Vec2<double> const { return Vec2<double>{ m_Parameter.Range().start, m_Parameter.Range().end }; }
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
	virtual auto   Units() -> std::unordered_map<int, std::string>& { return m_Parameter.Units(); }
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
};

template<typename Graphics>
class SliderBase : public ParameterComponent
{
public:
	using ParameterComponent::ParameterComponent;

	void Render(CommandCollection& d) override
	{
		ParameterComponent::Render(d);
		Graphics::Render(*this, d);
	};
};