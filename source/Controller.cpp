#include "Controller.hpp"
#include "Gui/Units.hpp"
#include "Gui/Slider.hpp"

class RoutePanel : public Panel
{
public:
	RoutePanel(Settings s)
		: Panel{ s }
	{
		ratio = 1;
		layout = Layout::Column;
		background.base = { 13, 13, 13 };
		padding = { 4, 4, 4, 4 };
		size = { Auto, Auto };
	}

	void Init()
	{
		auto& audio = Controller::Get().audio;
		panels.clear();
		float min = 25;

		float maxtextwidth = 10;
		for (auto& i : audio.inputs)
		{
			auto w = GraphicsBase::StringWidth(i->name, GraphicsBase::DefaultFont, 14);
			if (w > maxtextwidth)
				maxtextwidth = w;
		}
		maxtextwidth += 16;

		for (int i = -1; i < (int)audio.inputs.size(); i++)
		{
			auto& panel = panels.push_back(new Panel{ {.ratio = i == -1 ? 0.f : 1.f, .layout = Layout::Row, .size{ Inherit, i == -1 ? 25.f : Inherit }, .min{ min, min } } });

			for (int j = -1; j < (int)audio.outputs.size(); j++)
			{
				if (i == -1 || j == -1)
				{
					if (i == -1 && j == -1)
					{
						panel.panels.push_back(new Panel{ {.layout = Layout::Column, .margin{ 4, 4, 4, 4 }, .size{ maxtextwidth, 25 }, .min{ min, min } } });
						continue;
					}
					Text* text = new Text;
					if (i == -1)
						text->content = audio.outputs[j]->name;
					else 
						text->content = audio.inputs[i]->name;

					panel.panels.push_back(new Panel{ {.ratio = j == -1 ? 0.f : 1.f, .layout = Layout::Column, .margin{ 4, 4, 4, 4 }, .size{ maxtextwidth, Inherit } }, text });
				}
				else
					panel.panels.push_back(new Panel{ {.ratio = 1, .layout = Layout::Column, .margin{ 4, 4, 4, 4 }, .min{ min, min } }, new Slider{ {.value = audio.mix[i][j] }} });

			}
		}
	}
};


Controller::Controller()
{
	GraphicsBase::DefaultFont = "segoeui";
}

void Controller::Run()
{
	Gui _gui;

	window = new Frame;

	window->panel = Panel{ 
		{ .overflow = Overflow::Scroll, .padding{ 8, 8, 8, 8 }, .background{ {.base{ 26, 26, 26 } } } },
		new RoutePanel{ {} }
	};

	window->Create();

	_gui.push(window);

	OpenDevice(2);

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
	window->panel.component.Get<RoutePanel>()->Init();
}