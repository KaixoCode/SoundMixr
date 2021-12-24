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
{
	GraphicsBase::DefaultFont = "segoeui";
}

void Controller::Run()
{
	Gui _gui;

	window = new Frame;
	settings = new Frame{ {.state = Hide, .hideOnClose = true} };

	window->panel = Panel{ 
		{},
		new ChannelPanel
	};

	window->Create();
	settings->Create();

	_gui.push(settings);
	_gui.push(window);

	OpenDevice(1);

	Midi::Get().OpenInputPort(0);

	while (_gui.Loop())
	{
		Midi::Get().ReadMessages();
	}

	audio.Close();
	window = {};
	settings = {};
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