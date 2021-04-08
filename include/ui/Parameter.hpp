#pragma once
#include "pch.hpp"
#include "Graphics.hpp"
#include "Midi.hpp"

class ParameterBase : public Container
{
public:
	ParameterBase(Effects::Parameter& param);
	~ParameterBase();

	virtual void   X(int x) override { m_Parameter.Position({ x, m_Parameter.Position().y }); }
	virtual int    X() const override { return m_Parameter.Position().x; }
	virtual void   Y(int y) override { m_Parameter.Position({ m_Parameter.Position().x, y }); }
	virtual int    Y() const override { return m_Parameter.Position().y; }
	virtual void   Position(const Vec2<int>& pos) override { m_Parameter.Position({ pos.x, pos.y }); }
	virtual auto   Position() const -> Vec2<int> override { return { m_Parameter.Position().x, m_Parameter.Position().y }; }
	virtual void   Size(const Vec2<int>& size) override { m_Parameter.Size({ size.width, size.height }); }
	virtual auto   Size() const -> Vec2<int> override { return { m_Parameter.Size().width, m_Parameter.Size().height }; }
	virtual void   Width(int w) override { m_Parameter.Size({ w, m_Parameter.Size().height }); }
	virtual int    Width() const override { return m_Parameter.Size().width; }
	virtual void   Height(int h) override { m_Parameter.Size({ m_Parameter.Size().width, h }); }
	virtual int    Height() const override { return m_Parameter.Size().height; }
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
	virtual double DefaultReset() { return m_Parameter.DefaultReset(); }
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
	virtual bool   Hovering() const { return m_Hovering; }
	virtual bool   Dragging() const { return m_Dragging; }

	virtual operator nlohmann::json() { return m_Parameter.operator nlohmann::json(); };
	virtual void operator=(const nlohmann::json& json) { m_Parameter = json; };

	void Update(const Vec4<int>& vp) override;
	void Render(CommandCollection& d) override;

protected:
	Effects::Parameter& m_Parameter;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	int m_Counter = 0;

	double m_PressMouse = 0;

    bool m_Hovering = false,
		m_Dragging = false,
		m_Shift = false,
		m_Linking = false;

	int m_MidiLink = -1;

	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_LinkButton;
	Button<SoundMixrGraphics::Menu, ButtonType::Normal>* m_UnlinkButton;

	int m_CallbackId = -1;

	std::string m_ValueText;
};

template<typename Graphics>
class Parameter : public ParameterBase
{
public:
	using ParameterBase::ParameterBase;

	void Render(CommandCollection& d) override
	{
		ParameterBase::Render(d);
		Graphics::Render(*this, d);
	};
};