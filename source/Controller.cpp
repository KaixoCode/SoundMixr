#include "Controller.hpp"
#include "Gui/DeviceList.hpp"
#include "Gui/Units.hpp"
#include "Gui/Slider.hpp"

Controller::Controller()
{
	GraphicsBase::DefaultFont = "segoeui";
}

void Controller::Run()
{
	Gui _gui;

	window = new Frame;

	window->panel = Panel{ 
		{},
	};

	window->Create();

	_gui.push(window);

	OpenDevice(1);

	Midi::Get().OpenInputPort(0);

	while (_gui.Loop())
	{
		Midi::Get().ReadMessages();
	}

	audio.Close();
	window = {};
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