#include "Controller.hpp"
#include "Gui/ChannelPanel.hpp"
#include "Gui/DeviceList.hpp"
#include "Gui/Channel.hpp"

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
	Parser::Callback("print", [](bool a, const std::string& b) { if (a) std::cout << b << std::endl; });
	Parser::Callback("exit", [&](bool) { _gui.Close(); });

	GraphicsBase::LoadFont("C:/Windows/fonts/segoeui.ttf", "segoeui");
	GraphicsBase::LoadFont("C:/Windows/fonts/consola.ttf", "consolas");
	GraphicsBase::DefaultFont = "segoeui";

	std::ifstream t("./settings/layout.gc2");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string content = buffer.str();

	auto _components = Parser::Parse(content);

	

	auto _frame = std::find_if(_components.begin(), _components.end(), [](Parser::Scope& scope) { return scope.name == "frame"; });
	auto _channel = std::find_if(_components.begin(), _components.end(), [](Parser::Scope& scope) { return scope.name == "channel"; });

	if (_frame != _components.end())
		window = std::move(_frame->Generate().Get<Frame>());

	if (_channel != _components.end())
		Channel::generator = *_channel;

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

	if (device == -1)
		return;

	// Open and start the stream
	audio.Open(device);
	audio.Start();
}