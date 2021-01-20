#pragma once
#include "pch.hpp"
#include "ChannelPanel.hpp"

enum class Type
{
	Output = 0,
	Input = 1,
};

template<Type T>
class ListPanel : public ScrollPanel
{
public:
	ListPanel()
	{
		// Styling
		Background(Theme::Get(Theme::VIEW_BACKGROUND));
		EnableScrollbars(false, true);

		// Right click menu
		namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
		using MenuButton = Button<BG::Menu, BT::Normal>;
		int _height = 20, _width = 140;
		m_Menu.Emplace<MenuButton>([&] { this->Component().Emplace<EndpointPanel>((bool)T); }, "Add", Vec2<int>{ _width, _height });
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT)
				RightClickMenu::Get().Open(&m_Menu);
		};
	}

	void Update(const Vec4<int>& viewport) override
	{
		auto& _component = Component();
		auto& _components = _component.Components();
		for (int i = _components.size() - 1; i >= 0; i--)
		{
			auto& _c = _components[i];
			EndpointPanel* _p;
			if (_p = dynamic_cast<EndpointPanel*>(_c.get()))
			{
				if (_p->Close())
					_component.Erase(_components.begin() + i);
			}
		}
		ScrollPanel::Update(viewport);
	}

private:
	Menu<MenuGraphics::Vertical, MenuType::Normal> m_Menu;
};