#pragma once
#include "pch.hpp"
#include "ui/Graphics.hpp"
#include "EffectBase.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- DropDown Option ------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename Enum>
class DropDownOption : public Button<SoundMixrGraphics::Menu, ButtonType::List>
{
public:
	template<typename T, typename E>
	DropDownOption(const std::string& name, Enum value, int key, T* parent, E e = [](Enum v) {})
		: Button<SoundMixrGraphics::Menu, ButtonType::List>([parent, name, value, e] { parent->SelectP(value); parent->Name(name); e(value); }, name, key),
		m_Value(value)
	{}

	template<typename T, typename E>
	DropDownOption(Effects::DropDown& d, Effects::DropDown::Option& i, int key, T* parent, E e = [](Enum v) {})
		: Button<SoundMixrGraphics::Menu, ButtonType::List>([parent, &i, &d, e] { d.Select(i.id); parent->SelectP(i.id); parent->Name(i.name); e(i.id); }, i.name, key),
		m_Value(i.id)
	{}

	Enum Value() { return m_Value; }

private:
	Enum m_Value;
};

// -------------------------------------------------------------------------- \\
// ------------------------------ DropDown ---------------------------------- \\
// -------------------------------------------------------------------------- \\

template<typename Enum, typename Graphics>
class DropDown : public ButtonType::Normal
{
public:
	using Callback = std::function<void(Enum)>;

	DropDown()
		: ButtonType::Normal{ [this] { RightClickMenu::Get().Open(&m_Menu); } }, 
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
	}

	DropDown(Effects::DropDown& d)
		: ButtonType::Normal{ [this] { RightClickMenu::Get().Open(&m_Menu); } },
		m_DropDown(&d),
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
		for (auto& i : d.Options())
		{
			AddOption(d, i);
		} 
	}

	DropDown(RightClickMenu* r)
		: ButtonType::Normal{ [this, r] { if (r) r->Open(&m_Menu); } },
		m_Key(ButtonType::List::NewKey())
	{
		m_Menu.ButtonSize({ 140, 20 });
	}

	DropDownOption<Enum>& AddOption(const std::string& name, Enum value, Callback e = [](Enum v) {})
	{
		auto& a = m_Menu.Emplace<DropDownOption<Enum>>(name, value, m_Key, this, e);

		// Select if first component
		if (m_Menu.Components().size() == 1)
			a.Selected(true), m_Value = value, Name(name);

		return a;
	}

	DropDownOption<Enum>& AddOption(Effects::DropDown& d, Effects::DropDown::Option& i, Callback e = [](Enum v) {})
	{
		auto& a = m_Menu.Emplace<DropDownOption<Enum>>(d, i, m_Key, this, e);

		// Select if first component
		if (d.Selected() == i.id)
			a.Selected(true), m_Value = i.id, Name(i.name);

		return a;
	}

	void Select(Enum v) 
	{
		for (auto& i : m_Menu.Components())
		{
			auto _b = dynamic_cast<DropDownOption<Enum>*>(i.get());
			if (_b && _b->Value() == v)
				_b->Selected(true), m_Value = v, Name(_b->Name());
		}
	}

	Enum Value() { return m_Value; }

	void Update(const Vec4<int>& v) override
	{
		if (m_DropDown)
		{
			m_Pos = { m_DropDown->Position().x, m_DropDown->Position().y };
			m_Size = { m_DropDown->Size().width, m_DropDown->Size().height };

			if (m_DropDown->Selected() != m_Value)
				Select((Enum)m_DropDown->Selected());
		}
		ButtonType::Normal::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		ButtonType::Normal::Render(d);
		Graphics::Render(*this, d);
	};

	void ButtonSize(const Vec2<int>& s) { m_Menu.ButtonSize(s); }

	void ButtonWidth(int w) { m_Menu.ButtonWidth(w); }

	void ButtonHeight(int h) { m_Menu.ButtonHeight(h); }

private:

	void SelectP(Enum v) { m_Value = v; }
	template<typename Enum>
	friend class DropDownOption;
	int m_Key;
	Enum m_Value;

	Effects::DropDown* m_DropDown = nullptr;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
};

