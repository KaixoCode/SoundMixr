#include "Controller.hpp"
#include "Gui/ChannelPanel.hpp"
#include "Gui/DeviceList.hpp"
#include "Gui/Channel.hpp"
#include "Gui/Units.hpp"
#include "Gui/Slider.hpp"

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
	Parser::Callback("asioControlPanel", [&](bool) { audio.stream.OpenControlPanel(); });

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