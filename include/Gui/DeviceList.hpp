#pragma once
#include "pch.hpp"
#include "Controller.hpp"

/**
 * Button with menu graphics, that opens a
 * sub-menu when pressed.
 */
struct DeviceListButton : public SubMenuButton
{
	DeviceListButton(const Settings& settings = {}) : SubMenuButton(settings) { Init(); }
	DeviceListButton(DeviceListButton&& other) : SubMenuButton(std::forward<DeviceListButton>(other)) { Init(); }
	DeviceListButton(const DeviceListButton& other) : SubMenuButton(other) { Init(); }

private:
	void Init()
	{
		auto& audio = Controller::Get().audio;
		Button::settings.type = Hover;
		Button::settings.callback = [this, &audio](bool v)
		{
			if (v)
			{
				// Get the currently selected device
				int selected = audio.stream.Information().state != Closed ? audio.stream.Information().input : -1;

				// Clear the menu and add all buttons to the menu
				Button::Group group;
				menu.components.clear();
				menu.components.push_back(new Button{ {
					.group = group, // add button to group
					.type = Radio,  // It's a radio selection

					// Callback of button simply opens and starts the selected device.
					.callback = [](bool b) {
						if (b)
							Controller::Get().OpenDevice(-1);
					},
					.name = "No Device", // Set button name
				} });

				for (auto& d : audio.stream.Devices())
				{
					int id = d.id;
					menu.components.emplace_back(new Button{ {
						.group = group, // add button to group
						.type = Radio,  // It's a radio selection

						// Callback of button simply opens and starts the selected device.
						.callback = [id](bool b) {
							if (b)
								Controller::Get().OpenDevice(id);
						},
						.name = d.name, // Set button name
					} }

						// Set selected state to true if currently selected
					)->State(Selected) = id == selected;
				}
				ContextMenu::Open(menu, { x + width, y });
			}
			else
				ContextMenu::Close(menu);
		};
	}
};