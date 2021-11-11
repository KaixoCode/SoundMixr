#include "Gui/ChannelPanel.hpp"
#include "Gui/Channel.hpp"
#include "Controller.hpp"

ChannelPanel::ChannelPanel()
{
	overflow = { Overflow::Scroll, Overflow::Hide };
	ratio = 1;
	margin = { 8, 8, 8, 8 };

	panels.push_back(inputs);
	panels.push_back(new Panel{ {.size = { 4, Inherit }, .background = { {.base { 33, 37, 41  } } } } });
	panels.push_back(outputs);

	Init();
}

void ChannelPanel::Init()
{
	menu.components.push_back(new Button{ {
		.callback = [this](bool) {
			Controller::Get().audio.push_back(new Audio::EndpointChannel{ true });
		},
		.name = "Add Input Channel",
		.graphics = new MenuButtonGraphics
	} });

	menu.components.push_back(new Button{ {
		.callback = [this](bool) {
			Controller::Get().audio.push_back(new Audio::EndpointChannel{ false });
		},
		.name = "Add Output Channel",
		.graphics = new MenuButtonGraphics
	} });

	*this += [this](const MouseRelease& e)
	{
		if (e.button == MouseButton::Right && !e.Handled())
			ContextMenu::Open(menu, e.pos, true);
	};

	*this += [this](const Unfocus&)
	{
		if (Channel::selected)
			Channel::selected->State(Selected) = false;
		Channel::selected = nullptr;
	};

	*this += [this](const MousePress& e)
	{
		if (!e.Handled())
		{
			if (Channel::selected)
				Channel::selected->State(Selected) = false;
			Channel::selected = nullptr;
		}

		for (Channel& i : inputs.panels)
			i.NewSelect();

		for (Channel& i : outputs.panels)
			i.NewSelect();
	};
}

void ChannelPanel::Update()
{
	int _ins = 0;
	int _outs = 0;
	for (auto& i : Controller::Get().audio.channels)
	{
		if (i->type & Audio::Channel::Type::Input) _ins++;
		if (i->type & Audio::Channel::Type::Output) _outs++;
	}

	while (inputs.panels.size() < _ins)
		inputs.panels.push_back(new Channel{ true });

	while (inputs.panels.size() > _ins)
		inputs.panels.pop_back(), Channel::selected = nullptr;

	_ins = 0;
	auto _inputs = std::ranges::filter_view{
		Controller::Get().audio.channels,
		[](auto& c) { return c->type & Audio::Channel::Type::Input;}
	};

	auto _itIn = _inputs.begin();

	for (Channel& i : inputs.panels)
		i = *(_itIn++);
	
	while (outputs.panels.size() < _outs)
		outputs.panels.push_back(new Channel{ false  });

	while (outputs.panels.size() > _outs)
		outputs.panels.pop_back(), Channel::selected = nullptr;

	_outs = 0;
	auto _outputs = std::ranges::filter_view{
		Controller::Get().audio.channels,
		[](auto& c) { return c->type & Audio::Channel::Type::Output;}
	};

	auto _itOut = _outputs.begin();

	for (Channel& i : outputs.panels)
		i = *(_itOut++);
}