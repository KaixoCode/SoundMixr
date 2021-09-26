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
	: window({.name = "SoundMixr"})
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

	void Update() override
	{
		width = std::max(70, channel->lines * 16 + 8 + 30);
	}

	void Render(CommandCollection& d) const override
	{
		float _padding = 2;
		float _x = 8 + x + _padding;
		float _w = ((width - 30) - _padding) / channel->lines;
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
				menu.push_back(new MenuButton{ {
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
					menu.push_back(new MenuButton{ {
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

struct RouteButton : public Button
{
	StateColors background{ {
		.link = this, 
		.base = { 100, 100, 100, 0 }, 
		.transition = 10.f,
	} };

	StateColors triangle{ {
		.link = this,
		.base = { 50, 50, 50, 255 },
		.colors = {
			{ Hovering, { 70, 70, 70, 255 } },
			{ Selected, { 100, 100, 100, 255 } },
			{ Pressed, { 100, 100, 100, 255 } },
		},
		.transition = 10.f,
	} };

	bool input = false;

	RouteButton()
	{
		State<Disabled>(true);
		settings = {
			.type = Toggle
		};
	}

	void Render(CommandCollection& d) const override
	{
		if (State<Disabled>())
			return;

		double _div = 3;
		float _w = 3 * height / _div;
		float _h = 2 * height / _div;
		d.Fill(background.Current());
		d.Quad(dimensions);
		d.Fill(triangle.Current());
		d.Triangle(Vec4<float>{ x + width / 2, y + height / 2, _w, _h }, input ? -90.0f : 90.0f);
	}
};

struct Channel : public Panel
{
	static inline Channel* selected = nullptr;

	Pointer<ChannelBase> channel;

	GainSlider gain;
	RouteButton button;
	StateColors background{ {
		.link = this,
		.base = { 26, 26, 26 },
		.colors{
			{ Selected, { 36, 36, 36 } },
		}
	} };

	struct Border
	{
		StateColors color{ {
			.base = { 36, 36, 36 },
			.colors{
				{ Selected, { 46, 46, 46 } },
			}
		} };
		float width = 0;
	} border;

	Channel(bool input)
		: gain(channel->volume, channel),
		channel(new EndpointChannel{ input })
	{
		Controller::Get().audio.push_back(channel);
		
		settings = {
			.padding{ 2, 2, 2, 2 },
			.margin{ 4, 4, 4, 4 },
			.size{ Auto, Inherit },
			.background{ 26, 26, 26 }
		};
		
		border.color.Link(this);

		width = 70;
		button.input = input;
		button.settings.callback = [this](bool v)
		{
			if (selected && selected != this)
			{
				if (channel->type & ChannelBase::Type::Output && selected->channel->type & ChannelBase::Type::Input)
					if (v)
						selected->channel->Connect(channel);
					else
						selected->channel->Disconnect(channel);
				else if (channel->type & ChannelBase::Type::Input && selected->channel->type & ChannelBase::Type::Output)
					if (v)
						channel->Connect(selected->channel);
					else
						channel->Disconnect(selected->channel);
			}
		};

		settings.layout = Layout::Column;
		panels.push_back(new Panel{ {.ratio = 1, .size{ Auto, Inherit } }, gain });
		panels.push_back(new Panel{ {.size{ Inherit, 25 } }, button });

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
				ContextMenu::Open(menu, e.pos, false);
				e.Handle();
			}
		};

		listener += [this](const MousePress& e)
		{
			// Don't switch when pressing button
			if (button.Hitbox(e.pos))
			{
				e.Handle();
				return;
			}
			if (Channel::selected)
				Channel::selected->State<Selected>(false);

			Channel::selected = this;
			State<Selected>(true);
			e.Handle();
		};
	}

	void NewSelect()
	{
		if (!selected || (selected->channel->type & channel->type & (ChannelBase::Type::Input | ChannelBase::Type::Output))
			|| ((channel->type & ChannelBase::Type::Forward) || (selected->channel->type & ChannelBase::Type::Forward)))
			button.State<Disabled>(true);
		else
			button.State<Disabled>(false), button.State<Selected>(channel->Connected(selected->channel));
	}

	void Render(CommandCollection& d) const override
	{
		if (border.width != 0)
		{
			d.Fill(border.color.Current());
			d.Quad(dimensions);
		}
		d.Fill(background.Current());
		d.Quad({ x + border.width, y + border.width, width - border.width * 2, height - border.width * 2 });
	}

	~Channel()
	{
		Controller::Get().audio.remove(channel);
	}

	Menu menu;

	void Init()
	{

	}
};

struct ChannelPanel : public Panel
{
	static inline int ID = 8919451;
	ChannelPanel()
	{
		layout = Layout::Row;
		overflow = { Overflow::Scroll, Overflow::Hide };
		background = { 16, 16, 16 };
		id = 8919451;

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

		Init();
	}

	ChannelPanel(ChannelPanel&& other)
		: inputs(std::move(other.inputs)),
		outputs(std::move(other.outputs)),
		Panel(std::move(other))
	{
		background = { 16, 16, 16 };
		settings = other.settings;
		layout = Layout::Row;
		overflow = { Overflow::Scroll, Overflow::Hide };
		id = 8919451;

		Init();
	}

	Pointer<Panel> inputs;
	Pointer<Panel> outputs;
	Menu menu;

	void Init()
	{

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
			{
				if (Channel::selected)
					Channel::selected->State<Selected>(false);
				Channel::selected = nullptr;
			}

			for (Channel& i : inputs->panels)
				i.NewSelect();

			for (Channel& i : outputs->panels)
				i.NewSelect();
		};
	}
};

struct ChannelPanelParser : public PanelParser
{
	ChannelPanelParser()
	{
		settings.name = "channel-panel";
	}

	Pointer<Component> Create()
	{
		return new ChannelPanel{};
	}
};

void Controller::Run()
{
	Gui _gui;
	Parser::Link<DeviceListButtonParser>();
	Parser::Link<FrameParser>();
	Parser::Link<TextParser>();
	Parser::Link<TextBoxParser>();
	Parser::Link<TextAreaParser>();
	Parser::Link<ChannelPanelParser>();
	Parser::Link<TextDisplayerParser>();
	Parser::Callback("print", [](bool a, const std::string& b) { if (a) std::cout << b << std::endl; });
	Parser::Callback("exit", [&](bool) { _gui.Close(); });

	GraphicsBase::LoadFont("C:/Windows/fonts/segoeui.ttf", "segoeui");
	GraphicsBase::LoadFont("C:/Windows/fonts/consola.ttf", "consolas");
	GraphicsBase::DefaultFont = "segoeui";

	auto _components = Parser::Parse(R"~~(
let SoundMixr = frame name="SoundMixr";
let palette1.0 = { 248, 249, 250 };
let palette1.1 = { 233, 236, 239 };
let palette1.2 = { 222, 226, 230 };
let palette1.3 = { 206, 212, 218 };
let palette1.4 = { 173, 181, 189 };
let palette1.5 = { 108, 117, 125 };
let palette1.6 = {  73,  80,  87 };
let palette1.7 = {  52,  58,  64 };
let palette1.8 = {  33,  37,  41 };

let red_light = { 229, 56, 59 };
let red_dark = { 164, 22, 26 };

let frameColor = palette1.8;

let menubarcolors = color="frameColor" color:pressed="palette1.6" color:hovering="palette1.7" border-width="0";

<SoundMixr background="frameColor">
	<titlebar background="frameColor" text="palette1.0">
		<button type="close" color="frameColor" color:pressed="red_light" color:hovering="red_dark"/>
		<button type="minimize" menubarcolors/>
		<button type="maximize" menubarcolors/>
		<menu name="File" menubarcolors>
			<device-list></device-list>
			<button name="Exit" callback="exit()"></button>
		</menu>
	</titlebar>

	<panel>
		<channel-panel ratio="1" background="palette1.7" margin="{ 8, 8, 8, 8 }">
		</channel-panel>
	</panel>
</SoundMixr>

	)~~");

	window = std::move(_components[0].Generate().Get<Frame>());

	_gui.push_back(window);


	OpenDevice(-1);

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


void Controller::OpenDevice(int device)
{	
	// Close the stream if it wasn't closed.
	if (audio.stream.Information().state != Closed)
		audio.Close();

	// Clear everything from the channel panel
	Panel* _panel = window.panel.Find(ChannelPanel::ID);
	if (_panel)
	{
		ChannelPanel* _channels = dynamic_cast<ChannelPanel*>(_panel);
		_channels->inputs->panels.clear();
		_channels->outputs->panels.clear();
		Channel::selected = nullptr;
	}

	// Empty window when no device
	if (device == -1)
	{
		return;
	}

	// Open and start the stream
	audio.Open(device);
	audio.Start();
}