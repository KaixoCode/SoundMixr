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

struct GainSlider : public Component
{
	GainSlider(float& gain, Pointer<ChannelBase>& channel)
		: gain(gain), channel(channel)
	{}

	float& gain;
	Pointer<ChannelBase>& channel;
	
	float scale = 2;
	float min = -96;
	float max = 12;

	void Render(CommandCollection& d) const override
	{
		float _padding = 5;
		float _x = x + _padding;
		float _w = (width - _padding) / channel->lines;
		for (int i = 0; i < channel->lines; i++)
		{
			float _db = lin2db(channel->smoothed[i]);
			float _norm = std::max((_db - min) / (max - min), 0.f);
			float _scaled = std::pow(_norm, scale);

			float _h = _scaled * (height - 2 * _padding);
			d.Fill({ 0, 0, 0 });
			d.Quad({ _x, y + _padding, _w - _padding, height - 2 * _padding });
			d.Fill({ 0, 255, 0 });
			d.Quad({ _x, y + height - _padding, _w - _padding, -_h });
			_x += _w;
		}
	}
};


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

struct Channel : public Panel
{
	static inline Channel* selected = nullptr;
	Pointer<ChannelBase> channel;
	Pointer<GainSlider> gain;

	Channel(bool input)
	{
		channel = new EndpointChannel{ input };
		gain = new GainSlider{ channel->volume, channel };

		width = 50;
		background = { 26, 26, 26 };
		margin = { 4, 4, 4, 4 };
		Controller::Get().audio.push_back(channel);
		listener += [this, input](const MouseRelease& e) 
		{
			if (e.button == MouseButton::Right && !e.Handled())
			{
				menu.Clear();
				auto& _endpoints = input ? Controller::Get().audio.inputs : Controller::Get().audio.outputs;
				for (auto& i : _endpoints)
				{
					EndpointChannel& _channel = channel;
					MenuButton& _button = menu.push_back(new MenuButton{ {
						.type = Button::Toggle,
						.callback = [&](bool v) {
							if (v)
								_channel.Add(i);
							else
								_channel.Remove(i);
						},
						.name = i->name
					} });
					_button.State<Selected>(_channel.Contains(i));
				}
				ContextMenu::Open(menu, e.pos, true);
				e.Handle();
			}
		};

		listener += [this](const MousePress& e)
		{
			Channel::selected = this;
		};

		panels.push_back(new Panel{ {.ratio = 1 }, gain });
	}

	~Channel()
	{
		Controller::Get().audio.remove(channel);
	}

	void Update()
	{
		width = std::max(50, channel->lines * 25);
	}

	Menu menu;
};

struct ChannelPanel : public Panel
{
	ChannelPanel()
	{
		layout = Layout::Row;
		overflow = { Overflow::Scroll, Overflow::Hide };
		background = { 16, 16, 16 };

		outputs = panels.push_back(new Panel{ {
			.padding{ 4, 4, 4, 4 },
			.margin{ 0, 0, 4, 0 },
			.size{ Auto, Inherit },
			.background{ 16, 16, 16 }
		} });

		inputs = panels.push_back(new Panel{ {
			.padding{ 4, 4, 4, 4 },
			.margin{ 0, 0, 0, 0 }, 
			.size{ Auto, Inherit }, 
			.background{ 16, 16, 16 }
		} });

		menu.push_back(new MenuButton{ {
			.callback = [this](bool) {
				inputs->panels.push_back(new Channel{ true });
			},
			.name = "Add Input Channel"
		} });

		menu.push_back(new MenuButton{ {
			.callback = [this](bool) {
				outputs->panels.push_back(new Channel{ false });
			},
			.name = "Add Output Channel"
		} });

		listener += [this](const MouseRelease& e) 
		{
			if (e.button == MouseButton::Right && !e.Handled())
				ContextMenu::Open(menu, e.pos, true);
		};

		listener += [this](const MousePress& e)
		{
			if (!e.Handled())
				Channel::selected = nullptr;
		};
	}

	Pointer<Panel> inputs;
	Pointer<Panel> outputs;
	Menu menu;
};


void Controller::Run()
{
	Parser::Link<DeviceListButtonParser>();
	Parser::Link<FrameParser>();
	Parser::Link<TextParser>();
	Parser::Link<TextBoxParser>();
	Parser::Link<TextAreaParser>();
	Parser::Link<TextDisplayerParser>();
	Parser::Callback("print", [](bool a, const std::string& b) { if (a) std::cout << b << std::endl; });

	GraphicsBase::LoadFont("C:/Windows/fonts/segoeui.ttf", "segoeui");
	GraphicsBase::DefaultFont = "segoeui";
	GraphicsBase::LoadFont("C:/Windows/fonts/consola.ttf", "consolas");

	Frame _frame{ {.name = "SoundMixr" } };

	_frame.panel = {
		{.padding{ 8, 8, 8, 8 }, .background{ 26, 26, 26 } },
		new ChannelPanel{}
	};

	MenuBarButton& _button = _frame.titlebar.menu.push_back(new MenuBarButton{ {.name = "File" } });
	_button.menu.push_back(new DeviceListButton{ {.name = "Devices" } });

	Gui _gui;
	_gui.push_back(_frame);

	while (_gui.Loop());

	audio.Close();

	return;


	{
		Gui _gui;

		auto parts = Parser::Parse(R"~(
		<frame info.name="SoundMixr" position="{ 100, 100 }" size="{ 600, 600 }" background="{ 26, 26, 26 }">
			<titlebar>
				<menu name="Select Stuff">
					<button></button>
					<device-list></device-list>
				</menu>
			</titlebar>
			<panel layout="column" margin="{ 8, 8, 8, 8 }" padding="{ 4, 4, 4, 4 }" background="{ 16, 16, 16 }">
				<panel size.height="20" layout="row" margin="{ 4, 4, 4, 4 }">
					<panel ratio="1">
						<text align="center" text-color="{ 255, 255, 255 }" font-size="12" content="helloworld"></text>
					</panel>
					<panel ratio="1">
						<menu-button height="20" name="helloworld"></menu-button>
					</panel>
				</panel>
				<panel size.height="20" layout="row" margin="{ 4, 4, 4, 4 }">
					<panel ratio="1">
						<text align="center" text-color="{ 255, 255, 255 }" font-size="12" content="helloworld"></text>
					</panel>
					<panel ratio="1">
						<menu-button height="20" name="helloworld"></menu-button>
					</panel>
				</panel>
			</panel>
		</frame>
		)~");

		std::list<Pointer<Frame>> _frames;

		for (auto& i : parts)
			_frames.push_back(std::move(i.Generate()));
		
		for (auto& i : _frames)
			_gui.push_back(*i);

		while (_gui.Loop());
	}

	std::cout << Component::COUNTER << std::endl;
}