#pragma once
#include "pch.hpp"
#include "ui/Graphics.hpp"

template<typename Enum>
class DropdownOption : public Button<SoundMixrGraphics::Menu, ButtonType::List>
{
public:
	template<typename T>
	DropdownOption(const std::string& name, Enum value, int key, T* parent)
		: Button<SoundMixrGraphics::Menu, ButtonType::List>([parent, name, value] { parent->SelectP(value); parent->Name(name); }, name, key),
		m_Value(value)
	{}

	Enum Value() { return m_Value; }

private:
	Enum m_Value;
};

template<typename Enum, typename Graphics>
class Dropdown : public Button<Graphics, ButtonType::Normal>
{
public:
	Dropdown()
		: Button<Graphics, ButtonType::Normal>{ [this] { RightClickMenu::Get().Open(&m_Menu); } }, 
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
	}

	DropdownOption<Enum>& AddOption(const std::string& name, Enum value)
	{
		auto& a = m_Menu.Emplace<DropdownOption<Enum>>(name, value, m_Key, this);

		// Select if first component
		if (m_Menu.Components().size() == 1)
			a.Selected(true), m_Value = value, Name(name);

		return a;
	}

	void Select(Enum v) 
	{
		for (auto& i : m_Menu.Components())
		{
			auto _b = dynamic_cast<DropdownOption<Enum>*>(i.get());
			if (_b && _b->Value() == v)
				_b->Selected(true), m_Value = v, Name(_b->Name());
		}
	}

	Enum Value() { return m_Value; }

private:

	void SelectP(Enum v) { m_Value = v; }
	template<typename Enum>
	friend class DropdownOption;
	int m_Key;
	Enum m_Value;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
};

