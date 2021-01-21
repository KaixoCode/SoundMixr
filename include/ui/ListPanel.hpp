#pragma once
#include "pch.hpp"
#include "ui/ChannelPanel.hpp"

class ChannelListPanel : public ScrollPanel
{
public:
	ChannelListPanel(AudioIO& audioio)
	{
		// Styling
		Background(Theme::Get(Theme::VIEW_BACKGROUND));
		EnableScrollbars(false, true);

		// Right click menu
		namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
		using MenuButton = Button<BG::Menu, BT::Normal>;
		int _height = 20, _width = 140;

		m_Menu.Emplace<MenuButton>([&] { this->Component().Emplace<ChannelPanel>(audioio); }, "Add Channel", Vec2<int>{ _width, _height });
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
			ChannelPanel* _p;
			if (_p = dynamic_cast<ChannelPanel*>(_c.get()))
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