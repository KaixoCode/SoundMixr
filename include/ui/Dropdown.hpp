#pragma once
#include "pch.hpp"
#include "ui/Graphics.hpp"
#include "EffectBase.hpp"

/**
 * Single
 */
template<typename Enum>
class DropDownOption : public Button<SoundMixrGraphics::Menu, ButtonType::List>
{
public:
	using Callback = std::function<void(Enum)>;

	template<typename T>
	DropDownOption(const std::string& name, Enum value, int key, T* parent, Callback e = [](Enum v) {})
		: Button<SoundMixrGraphics::Menu, ButtonType::List>([parent, name, value, e] { parent->SelectP(value); parent->Name(name); e(value); }, name, key),
		m_Value(value), m_Callback(e)
	{}

	template<typename T>
	DropDownOption(Effects::DropDown& d, Effects::DropDown::Option& i, int key, T* parent, Callback e = [](Enum v) {})
		: Button<SoundMixrGraphics::Menu, ButtonType::List>([parent, &i, &d, e] { d.Select(i.id); parent->SelectP(i.id); parent->Name(i.name); e(i.id); }, i.name, key),
		m_Value(i.id), m_Callback(e)
	{}

	Enum Value() { return m_Value; }

	void Selected(bool v) { if (v && !m_Selected) m_Callback(m_Value); m_Selected = v; }
	bool Selected() { return m_Selected; }

private:
	Callback m_Callback;
	Enum m_Value;
};

template<typename Enum, typename Graphics>
class DropDown : public ButtonType::Normal
{
public:
	using Callback = std::function<void(Enum)>;

	/**
	 * Constructor.
	 * Normal dropdown constructor.
	 */
	DropDown()
		: ButtonType::Normal{ [this] { RightClickMenu::Get().Open(&m_Menu, false, m_AbsPos); m_Active = false; } },
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
	}

	/**
	 * Constructor.
	 * Wrapper for the EffectBase dropdown.
	 */
	DropDown(Effects::DropDown& d)
		: ButtonType::Normal{ [this] { RightClickMenu::Get().Open(&m_Menu, false, m_AbsPos); m_Active = false; } },
		m_DropDown(&d),
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
		for (auto& i : d.Options())
			AddOption(d, i);
	}

	/**
	 * Add an option to the <code>DropDown</code>. It will automatically select the option if it is the
	 * first one to be added.
	 * @param name name
	 * @param value value
	 * @param e callback specific for this option
	 */
	DropDownOption<Enum>& AddOption(const std::string& name, Enum value, Callback e = [](Enum v) {})
	{
		auto& a = m_Menu.Emplace<DropDownOption<Enum>>(name, value, m_Key, this, e);

		// Select if first component
		if (m_Menu.Components().size() == 1)
			a.Selected(true), m_Value = value, Name(name);

		return a;
	}

	/**
	 * Add an option to the <code>DropDown</code>. It will automatically select the option if it is the
	 * first one to be added.
	 * @param d EffectBase dropdown
	 * @param i EffectBase dropdown option
	 * @param e callback specific for this option
	 */
	DropDownOption<Enum>& AddOption(Effects::DropDown& d, Effects::DropDown::Option& i, Callback e = [](Enum v) {})
	{
		auto& a = m_Menu.Emplace<DropDownOption<Enum>>(d, i, m_Key, this, e);

		// Select if first component
		if (d.Selected() == i.id)
			a.Selected(true), m_Value = i.id, Name(i.name);

		return a;
	}

	/**
	 * Select value.
	 * @param v value
	 */
	void Select(Enum v) 
	{
		for (auto& i : m_Menu.Components())
		{
			auto _b = dynamic_cast<DropDownOption<Enum>*>(i.get());
			if (_b)
				_b->Selected(false);
			if (_b && _b->Value() == v)
				_b->Selected(true), m_Value = v, Name(_b->Name());
		}
	}

	/**
	 * Get the value.
	 * @return value of this dropdown.
	 */
	Enum Value() { return m_Value; }

	void Update(const Vec4<int>& v) override
	{
		if constexpr (std::is_same_v<Enum, int>)
		{
			if (m_DropDown)
			{
				m_Pos = { m_DropDown->Position().x, m_DropDown->Position().y };
				m_Size = { m_DropDown->Size().width, m_DropDown->Size().height };

				if (m_DropDown->Selected() != m_Value)
					Select((Enum)m_DropDown->Selected());
			}
		}
		ButtonType::Normal::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		ButtonType::Normal::Render(d);
		Graphics::Render(*this, d);
		m_AbsPos = d.Translate() + Position();
		if (m_DropDown)
		{
			if (Disabled())
				d.Command<::Graphics::Fill>(ThemeT::Get().dropdown_button_disabled_text);
			else if (Active())
				d.Command<::Graphics::Fill>(ThemeT::Get().dropdown_button_active_text);
			else if (Hovering())
				d.Command<::Graphics::Fill>(ThemeT::Get().dropdown_button_hovering_text);
			else
				d.Command<::Graphics::Fill>(ThemeT::Get().dropdown_button_idle_text);
		
			d.Command<::Graphics::TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<::Graphics::Text>(&m_DropDown->Name(), Position() + Vec2<int>{ Width() / 2, Height() + 5 });
		}
	};

	void ButtonSize(const Vec2<int>& s) { m_Menu.ButtonSize(s); }

	void ButtonWidth(int w) { m_Menu.ButtonWidth(w); }

	void ButtonHeight(int h) { m_Menu.ButtonHeight(h); }

	void Clear()
	{
		m_Menu.Clear();
		ButtonType::Normal::Clear();
	}

private:

	void SelectP(Enum v) { m_Value = v; }
	template<typename Enum>
	friend class DropDownOption;
	int m_Key;
	Enum m_Value;

	Vec2<int> m_AbsPos{ 0, 0 };

	Effects::DropDown* m_DropDown = nullptr;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
};

