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

struct Parameter : public Component
{
	struct Settings
	{
		Vec2<float> range{ 0, 100 };
		float value = 0;
		float reset = 0;
		float shift = 0.25;
		bool vertical = true;
		Function<float(float)> scaling = [](float in) { return in; };
		Function<float(float)> inverse = [](float in) { return in; };
	};

	Settings settings;

	Vec2<float>& range = settings.range;
	float& value = settings.value;
	float& reset = settings.reset;
	float& shift = settings.shift;
	bool& vertical = settings.vertical;
	Function<float(float)>& scaling = settings.scaling;
	Function<float(float)>& inverse = settings.inverse;

	Parameter(const Settings& settings = {})
		: settings(settings)
	{
		listener += [this](const MousePress& e)
		{
			m_PressVal = inverse((value - range.start) / (range.end - range.start));
			m_PrevPos = vertical ? e.pos.y : e.pos.x;
		};

		listener += [this](const MouseDrag& e)
		{
			if (~e.buttons & MouseButton::Left)
				return;

			float _value = vertical
				? (e.pos.y - m_PrevPos) / height
				: (e.pos.x - m_PrevPos) / width;

			m_PrevPos = vertical ? e.pos.y : e.pos.x;

			if (e.mod & EventMods::Shift)
				_value *= shift;

			m_PressVal += _value;

			if (scaling)
				value = scaling(constrain(m_PressVal, 0.f, 1.f)) * (range.end - range.start) + range.start;
		};

		listener += [this](const MouseClick& e)
		{
			auto _now = std::chrono::steady_clock::now();
			auto _duration = std::chrono::duration_cast<std::chrono::milliseconds>(_now - m_ChangeTime).count();
			if (_duration < 500)
				value = reset;

			m_ChangeTime = _now; // Get changetime
		};
	}

private:
	std::chrono::steady_clock::time_point m_ChangeTime;
	float m_PressVal = 0;
	float m_PrevPos = 0;
};

struct GainSlider : public Parameter
{

	static inline std::map<int, std::string> strings;
	GainSlider(float& gain, Pointer<ChannelBase>& channel)
		: gain(gain), channel(channel),
		Parameter{ {
			.range{ -66, 12 },
			.value = 0,
			.reset = 0,
			.vertical = true,
			.scaling = [&](float v) {
				float _a = 1 / std::tanh(b);
				float _x = 1 + (std::atanh(((1 - v) - 1) / _a) / b);
				if (_x < 0.001) _x = 0; // Completely mute below threshold
				return _x;
			},
			.inverse = [&](float v) {
				float _a = 1 / std::tanh(b);
				return _a * std::tanh(b * (1 - v));
			}
		} }
	{}

	float& gain;
	Pointer<ChannelBase>& channel;
	float b = 1.3;
	float step = 3;
	bool numbers = true;
	bool lines = true;
	StateColors bar{ {.link = this, .base{ 100, 100, 100 } } };

	void Update() override
	{
		width = std::max(70, channel->lines * 16 + 8 + 30);

		if (value == range.start)
			gain = 0;
		else
			gain = db2lin(value);
	}

	void Render(CommandCollection& d) const override
	{
		float _step = step;
		if (height < 300)
			_step = step * 2;
		if (height < 100)
			_step = step * 4;
		float _bx = 4;
		float _sidepadding = 26;
		float _a = 1 / std::tanh(b);
		float _padding = 2;
		float _ypadding = 2;
		float _x = _bx + x + _padding;
		float _w = ((width - (_bx + _sidepadding)) - _padding) / channel->lines;
		for (int i = 0; i < channel->lines; i++)
		{
			float _db = lin2db(channel->smoothed[i]);
			float _norm = std::max((_db - range.start) / (range.end - range.start), 0.f);
			float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
			float _0db = std::max((0 - range.start) / (range.end - range.start), 0.f);
			float _0scaled = 1 - _a * std::tanh(b * (1 - _0db));

			float _h = _scaled * (height - 2 * _ypadding);
			float _0h = std::round(_0scaled * (height - 2 * _padding));
			d.Fill({ 0, 0, 0 });
			d.Quad({ _x, y + height - _ypadding, _w - _padding, -_0h + 1 });
			d.Quad({ _x, y + _ypadding, _w - _padding, height - _0h - 2 * _ypadding });
			d.Fill({ 0, 255, 0 });
			d.Quad({ _x, y + height - _ypadding, _w - _padding, -_h });
			_x += _w;
		}
		_x -= _padding;

		if (lines)
		{
			d.Fill({ 128, 128, 128 });
			d.TextAlign(Align::Left | Align::Middle);
			for (int i = range.start + _step; i <= range.end; i += _step)
			{
				float _norm = std::max((i - range.start) / (range.end - range.start), 0.f);
				float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
				float _y = std::round(y + height - _ypadding - _scaled * (height - 2 * _ypadding));
				d.Quad({ _x, _y, 5, 1 });
			}
		}

		if (numbers)
		{
			d.Fill({ 255, 255, 255 });
			d.TextAlign(Align::Right | Align::Middle);
			for (int i = range.start + step * 2; i <= range.end; i += _step * 2)
			{
				float _norm = std::max((i - range.start) / (range.end - range.start), 0.f);
				float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
				float _y = std::round(y + height - _ypadding - _scaled * (height - 2 * _ypadding));
				if (lines)
					d.Quad({ _x, _y, 5, 1 });
				
				int abs = i < 0 ? -i : i;
				if (!strings.contains(abs))
					strings[abs] = std::to_string(abs);
				d.Text(strings[abs], { x + width - 4, _y });
			}
		}

		_padding = 6;
		float _h = 25;
		float _he = _h - _padding * 2;
		float _norm = std::max((value - range.start) / (range.end - range.start), 0.f);
		float _scaled = _a * std::tanh(b * (1 - _norm));
		float _y = y + std::round(_scaled * (height - 2 * _ypadding)) - _padding + 1;
		d.Fill(bar.Current());
		d.Triangle(Vec4<float>{x + _bx, _y + _padding, 8, _he}, 0.0f);
		d.Triangle(Vec4<float>{x + width - _sidepadding, _y + _padding, 8, _he}, 180.0f);
		d.Quad(Vec4<float>{x + _bx, _y + _padding - 1, width - _sidepadding, 3});

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
				SubMenuButton& _sub = menu.push_back(new SubMenuButton{ {.name = "Select Channels"} });
				auto& _endpoints = input ? Controller::Get().audio.inputs : Controller::Get().audio.outputs;
				for (auto& i : _endpoints)
				{
					EndpointChannel& _channel = channel;
					MenuButton& _button = _sub.menu.push_back(new MenuButton{ {
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