#pragma once
#include "pch.hpp"


class SettingsPanel : public Panel
{
public:

	SettingsPanel(AsioDevice& a)
		: m_Asio(a)
	{
		

	}

	void Update(const Vec4<int>& v) override
	{

		Panel::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		Panel::Render(d);
	}

	void UpdateSettings()
	{

	}

private:
	DropDown<int, DropdownButton>* m_Dropdown;
	AsioDevice& m_Asio;
};