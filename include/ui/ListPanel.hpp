#pragma once
#include "pch.hpp"
#include "ChannelPanel.hpp"

template<size_t T>
class ListPanel : public ScrollPanel
{
public:
	ListPanel()
		: ScrollPanel()
	{
		namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
		using MenuButton = Button<BG::Menu, BT::Normal>;
		int _height = 20, _width = 140;
		EnableScrollbars(false, true);
		Width(400);
		Background(Theme::Get(Theme::VIEW_BACKGROUND));
		MinWidth(200);
		m_Menu.Emplace<MenuButton>([&] { this->Component().Emplace<ChannelPanel>(T); }, "Add", Vec2<int>{ _width, _height });
		m_Menu.Emplace<MenuButton>([&] { LOG("REMOVE"); }, "Remove", Vec2<int>{ _width, _height });
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};
	}

private:
	Menu<MenuGraphics::Vertical, MenuType::Normal> m_Menu;
};