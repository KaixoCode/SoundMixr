#include "Gui/ChannelPanel.hpp"
#include "Gui/Channel.hpp"
#include "Controller.hpp"

ChannelPanel::ChannelPanel()
{
	layout = Layout::Row;
	overflow = { Overflow::Scroll, Overflow::Hide };
	background = { 16, 16, 16 };
	id = ID;

	outputs = panels.push_back(new Panel{ {
		.padding{ 4, 4, 4, 4 },
		.margin{ 0, 0, 4, 0 },
		.size{ Auto, Inherit }
	} });

	inputs = panels.push_back(new Panel{ {
		.padding{ 4, 4, 4, 4 },
		.margin{ 0, 0, 0, 0 },
		.size{ Auto, Inherit }
	} });

	Init();
}

ChannelPanel::ChannelPanel(ChannelPanel&& other)
	: inputs(std::move(other.inputs)),
	outputs(std::move(other.outputs)),
	Panel(std::move(other))
{
	background = { 16, 16, 16 };
	settings = other.settings;
	layout = Layout::Row;
	overflow = { Overflow::Scroll, Overflow::Hide };
	id = ID;

	Init();
}

void ChannelPanel::Init()
{
	menu->push_back(new MenuButton{ {
		.callback = [this](bool) {
			Controller::Get().audio.push_back(new EndpointChannel{ true });
		},
		.name = "Add Input Channel"
	} });

	menu->push_back(new MenuButton{ {
		.callback = [this](bool) {
			Controller::Get().audio.push_back(new EndpointChannel{ false });
		},
		.name = "Add Output Channel"
	} });

	listener += [this](const MouseRelease& e)
	{
		if (e.button == MouseButton::Right && !e.Handled())
			ContextMenu::Open(menu, e.pos, true);
	};

	listener += [this](const Unfocus&)
	{
		if (Channel::selected)
			Channel::selected->State<Selected>(false);
		Channel::selected = nullptr;
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

void ChannelPanel::Update()
{
	int _ins = 0;
	int _outs = 0;
	for (auto& i : Controller::Get().audio.channels)
	{
		if (i->type & ChannelBase::Type::Input) _ins++;
		if (i->type & ChannelBase::Type::Output) _outs++;
	}

	while (inputs->panels.size() < _ins)
	{
		Pointer<Channel> _channel = Channel::generator.Generate();
		_channel->Init(true);
		inputs->panels.push_back(std::move(_channel));
	}
	while (outputs->panels.size() < _outs)
	{
		Pointer<Channel> _channel = Channel::generator.Generate();
		_channel->Init(false);
		outputs->panels.push_back(std::move(_channel));
	}
	while (inputs->panels.size() > _ins)
		inputs->panels.pop_back(), Channel::selected = nullptr;

	while (outputs->panels.size() > _outs)
		outputs->panels.pop_back(), Channel::selected = nullptr;

	_ins = 0;
	_outs = 0;

	auto _inputs = std::ranges::filter_view{
		Controller::Get().audio.channels,
		[](auto& c) { return c->type & ChannelBase::Type::Input;}
	};

	auto _outputs = std::ranges::filter_view{
		Controller::Get().audio.channels,
		[](auto& c) { return c->type & ChannelBase::Type::Output;}
	};

	auto _itIn = _inputs.begin();
	auto _itOut = _outputs.begin();

	for (Channel& i : inputs->panels)
		i = *(_itIn++);

	for (Channel& i : outputs->panels)
		i = *(_itOut++);
}


ChannelPanelParser::ChannelPanelParser()
{
	settings.name = "channel-panel";
}

Pointer<Component> ChannelPanelParser::Create()
{
	return new ChannelPanel{};
}

void ChannelPanelParser::Append(Component& c, Pointer<Component>&& obj)
{}
