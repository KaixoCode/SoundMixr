#include "Controller.hpp"
#include "Gui/ChannelPanel.hpp"
#include "Gui/DeviceList.hpp"
#include "Gui/Channel.hpp"
#include "Gui/Units.hpp"

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





struct SliderParser;
class Slider : public Parameter
{
public:
	struct Settings
	{
		StateColors background{ {
			.base = { 26, 26, 26 },
		} };

		StateColors color{ {
			.base = { 36, 36, 36 },
		} };

		struct Border
		{
			StateColors color{ {
				.base = { 12, 12, 12 },
			} };
			float width = 1;
		} border;

		struct Text
		{
			StateColors color{ {
				.base = { 255, 255, 255 },
			} };
			float size = 14;
		} text;

		std::string font = GraphicsBase::DefaultFont;

		struct Display
		{
			bool name = false;
			bool value = true;
		} display;

		int decimals = 0;
	} settings;

	Slider(const Settings& settings = {})
		: settings(settings)
	{
		Init();
	}

	Slider(Slider&& other) = delete;
	Slider(const Slider&) = delete;
	Slider& operator=(Slider&& other) = delete;
	Slider& operator=(const Slider& other) = delete;

	void Update() override
	{
		m_Value = Units::units[Units::PAN].Format(value, settings.decimals);
	}

	void Render(CommandCollection& d) const override
	{
		d.Fill(settings.border.color.Current());
		d.Quad(dimensions);

		d.Fill(settings.background.Current());
		d.Quad({ x + settings.border.width, y + settings.border.width, width - 2 * settings.border.width, height - 2 * settings.border.width });

		float _width = width - 2 * settings.border.width;
		float _start = x + settings.border.width;
		float _end = Normalize(value) * _width;
		if (range.start < 0 && range.end > 0)
		{
			_start += _width / 2;
			_end -= _width / 2;
		}
		d.Fill(settings.color.Current());
		d.Quad({ _start, y + settings.border.width, _end, height - 2 * settings.border.width });

		d.Fill(settings.text.color.Current());
		d.TextAlign(Align::Center);
		d.FontSize(settings.text.size);
		d.Font(settings.font);
		d.Text(m_Value, { x + width / 2, y + height / 2 });
	}

private:
	Ref<float> m_BorderWidth = settings.border.width;
	Ref<StateColors> m_Color = settings.color;
	Ref<StateColors> m_BorderColor = settings.border.color;
	Ref<StateColors> m_Background = settings.background;
	Ref<StateColors> m_TextColor = settings.text.color;
	Ref<float> m_FontSize = settings.text.size;
	Ref<std::string> m_Font = settings.font;

	std::string m_Value;

	void Init()
	{
		Parameter::settings.vertical = false;
		settings.background.Link(this);
		settings.color.Link(this);
		settings.border.color.Link(this);
		settings.text.color.Link(this);
	}

	friend class SliderParser;
};

struct SliderParser : public ParameterParser
{
	SliderParser()
	{
		Parser::Link<ParameterParser>();
		settings.name = "slider";
		Attribute("border-width", &Slider::m_BorderWidth);
		Attribute("border-color", &Slider::m_BorderColor);
		Attribute("color", &Slider::m_Color);
		Attribute("background", &Slider::m_Background);
		Attribute("text-color", &Slider::m_TextColor);
		Attribute("font-size", &Slider::m_FontSize);
		Attribute("font", &Slider::m_Font);
	}

	Pointer<Component> Create()
	{
		return new Slider;
	}
};

void Controller::Run()
{
	Gui _gui;
	Parser::Link<DeviceListButtonParser>();
	Parser::Link<NormalButtonParser>();
	Parser::Link<FrameParser>();
	Parser::Link<TextParser>();
	Parser::Link<TextBoxParser>();
	Parser::Link<TextAreaParser>();
	Parser::Link<ChannelPanelParser>();
	Parser::Link<TextDisplayerParser>();
	Parser::Link<ChannelParser>();
	Parser::Link<SliderParser>();
	Parser::Callback("print", [](bool a, const std::string& b) { if (a) std::cout << b << std::endl; });
	Parser::Callback("exit", [&](bool) { _gui.Close(); });
	Parser::Callback("showSettings", [&](bool) { settings->State<Visible>(Show); });

	GraphicsBase::LoadFont("C:/Windows/fonts/segoeui.ttf", "segoeui");
	GraphicsBase::LoadFont("C:/Windows/fonts/consola.ttf", "consolas");
	GraphicsBase::DefaultFont = "segoeui";

	std::ifstream t("./settings/layout.gc2");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string content = buffer.str();

	auto _components = Parser::Parse(content);

	auto _frame = std::find_if(_components.begin(), _components.end(), [](Parser::Scope& scope) { return scope.name == "frame" && scope.attributes["info.name"] == "SoundMixr"; });
	auto _settings = std::find_if(_components.begin(), _components.end(), [](Parser::Scope& scope) { return scope.name == "frame" && scope.attributes["info.name"] == "Settings"; });
	auto _channel = std::find_if(_components.begin(), _components.end(), [](Parser::Scope& scope) { return scope.name == "channel"; });

	if (_settings != _components.end())
		settings = _settings->Generate();

	if (_frame != _components.end())
		window = _frame->Generate();

	if (_channel != _components.end())
		Channel::generator = *_channel;

	settings->State<Visible>(Hide);
	settings->Loop();
	settings->info.hideOnClose = true;

	_gui.push_back(settings);
	_gui.push_back(window);

	OpenDevice(-1);

	Midi::Get().OpenInputPort(0);

	while (_gui.Loop())
	{
		Midi::Get().ReadMessages();
	}
	audio.Close();
	window->panel.panels.clear();
	settings->panel.panels.clear();

	std::cout << Component::COUNTER << std::endl;
}


void Controller::OpenDevice(int device)
{	
	// Close the stream if it wasn't closed.
	if (audio.stream.Information().state != Closed)
		audio.Close();

	if (device == -1)
		return;

	// Open and start the stream
	audio.Open(device);
	audio.Start();
}