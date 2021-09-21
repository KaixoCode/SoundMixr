#include "Controller.hpp"

/**
 * Aspects of SoundMixr:
 *  - Loading/Saving information.
 *  - Theme color loading.
 *  - Audio routing.
 *  - Choosing audio/midi device.
 *  - Midi linking to parameters.
 *  - Loading screen.
 */


Controller::Controller()
{}

/**
 * 
 * SoundMixr GUI:
 *  - Channels
 *  - Effects
 *  - 
 * 
 * 
 */


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
				menu.Clear();
				for (auto& d : audio.stream.Devices())
				{
					int id = d.id;
					menu.push_back(new MenuButton{ {
						.group = group, // add button to group
						.type = Radio,  // It's a radio selection

						// Callback of button simply opens and starts the selected device.
						.callback = [id, selected, &audio](bool b) {
							if (b)
							{
								// Close the stream if it wasn't closed.
								if (audio.stream.Information().state != Closed)
									audio.Close();

								// Open and start the stream
								audio.Open(id);
								audio.Start();
							}
						},
						.name = d.name, // Set button name
					} }
					
						// Set selected state to true if currently selected
					)->State<Selected>(id == selected);
				}
				ContextMenu::Open(menu, { x + width, y });
			}
			else
				ContextMenu::Close(menu);
		};
	}
};

struct DeviceListButtonParser : public MenuButtonParser
{
	DeviceListButtonParser()
	{
		settings.name = "device-list";
		alias["button"] = "menu-button";
		alias["menu"] = "sub-menu-button";
	}

	Pointer<Component> Create() override
	{
		return new DeviceListButton{};
	}

	void Append(Component& c, Pointer<Component>&& obj)
	{
		if (SubMenuButton* _t = dynamic_cast<SubMenuButton*>(&c))
			_t->menu.push_back(std::move(obj));
	}
};


void Controller::Run()
{
	Parser::Link<DeviceListButtonParser>();
	Parser::Link<FrameParser>();
	Parser::Callback("print", [](bool a, const std::string& b) { if (a) std::cout << b << std::endl; });

	{
		Gui _gui;

		auto parts = Parser::Parse(R"~(
		<frame info.name="SoundMixr" position="{ 100, 100 }" size="{ 600, 600 }">
			<titlebar>
				<menu name="Select Stuff">
					<button></button>
					<device-list></device-list>
				</menu>
			</titlebar>
			<panel layout="Column" padding="{ 8, 8, 8, 8 }">
				<panel overflow="Scroll" ratio="1" background="{ 255, 0, 0 }">

				</panel>
				<panel overflow="Scroll" ratio="2" background="{ 0, 255, 0 }">

				</panel>
				<panel overflow="Scroll" ratio="1" background="{ 255, 255, 0 }">

				</panel>
			</panel>
		</frame>
		)~");

		std::list<Pointer<Frame>> _frames;

		for (auto& i : parts)
			_frames.push_back(std::move(i.Generate()));

		GraphicsBase::LoadFont("C:/Windows/fonts/segoeui.ttf", "segoeui");
		
		for (auto& i : _frames)
			_gui.push_back(*i);

		while (_gui.Loop());
	}

	std::cout << Component::COUNTER << std::endl;
}