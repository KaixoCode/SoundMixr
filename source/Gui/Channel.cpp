#include "Gui/Channel.hpp"
#include "Controller.hpp"

Channel::Channel(const Settings& s)
	: settings(s)
{}

Channel& Channel::operator=(Channel&& other)
{
	settings = std::move(other.settings);
	Panel::operator=(std::move(other));
	return *this;
}

void Channel::operator=(const Pointer<Audio::Channel>& c)
{
	if (channel == c)
		return;

	channel = c;
	name->content = c->name;
	name->displayer.RecalculateLines();
	gain->value = lin2db(c->volume);
	gain->midi = c->volume.midi;
	mutebutton->State(Selected) = c->mute;
	monobutton->State(Selected) = c->mono;
	panslider->value = panslider->Unnormalize(c->pan * 0.5 + 0.5);
	panslider->midi = panslider->midi;

}

void Channel::Init(bool input)
{
	//settings = {
	//	.padding{ 2, 2, 2, 2 },
	//	.margin{ 4, 4, 4, 4 },
	//	.size{ Auto, Inherit },
	//	.background{ 26, 26, 26 }
	//};

	if (auto _p = Find(ROUTE_BUTTON))
		if (_p->component)
			routebutton = _p->component;

	if (auto _p = Find(MUTE_BUTTON))
		if (_p->component)
		{
			mutebutton = _p->component;
			mutebutton->settings.type = Button::Toggle;
			mutebutton->settings.callback = [this](bool v)
			{
				channel->mute = v;
			};
		}

	if (auto _p = Find(MONO_BUTTON))
		if (_p->component)
		{
			monobutton = _p->component;
			monobutton->settings.type = Button::Toggle;
			monobutton->settings.callback = [this](bool v)
			{
				channel->mono = v;
			};
		}
	if (auto _p = Find(NAME_BOX))
		name = _p;

	if (auto _p = Find(GAIN_SLIDER))
		if (_p->component)
			gain = _p->component;
	
	if (auto _p = Find(PAN_SLIDER))
		if (_p->component)
			panslider = _p->component;

	border.color.Link(this);
	background.Link(this);

	width = 70;
	routebutton->State(Input) = input;
	routebutton->settings.type = Button::Toggle;
	routebutton->settings.callback = [this](bool v)
	{
		if (selected && selected != this)
		{
			if (channel->type & Audio::Channel::Type::Output && selected->channel->type & Audio::Channel::Type::Input)
				if (v)
					selected->channel->Connect(channel);
				else
					selected->channel->Disconnect(channel);
			else if (channel->type & Audio::Channel::Type::Input && selected->channel->type & Audio::Channel::Type::Output)
				if (v)
					channel->Connect(selected->channel);
				else
					channel->Disconnect(selected->channel);
		}
	};

	//name.align = Align::Center;
	//name.fontSize = 12;
	name->listener += [this](const Unfocus&)
	{
		channel->name = name->content;
	};

	//settings.layout = Layout::Column;
	//panels.push_back(new Panel{ {.margin{ 0, 0, 0, 5 }, .size{ Inherit, 25 } }, name });
	//panels.push_back(new Panel{ {.ratio = 1, .size{ Auto, Inherit } }, gain });
	//panels.push_back(new Panel{ {.size{ Inherit, 25 } }, button });

	listener += [this, input](const MouseRelease& e)
	{
		if (e.button == MouseButton::Right && !e.Handled())
		{
			menu->Clear();
			SubMenuButton& _sub = menu->push_back(new SubMenuButton{ {.name = "Select Channels"} });
			auto& _endpoints = input ? Controller::Get().audio.inputs : Controller::Get().audio.outputs;
			for (auto& i : _endpoints)
			{
				Audio::EndpointChannel& _channel = channel;
				MenuButton& _button = _sub.menu.push_back(new MenuButton{ {
					.type = Button::Toggle,
					.callback = [&](bool v) {
						if (v)
							_channel.Add(i->id);
						else
							_channel.Remove(i->id);
					},
					.name = i->name
				} });
				_button.State(Selected) = _channel.Contains(i->id);
			}

			menu->push_back(new MenuButton{ {
				.callback = [this](bool v) {
					Controller::Get().audio.remove(channel);
					ContextMenu::Close(menu);
				},
				.name = "Delete"
			} });

			ContextMenu::Open(menu, e.pos, false);
			e.Handle();
		}
	};

	listener += [this](const MousePress& e)
	{
		// Don't switch when pressing button
		if (!routebutton->State(Disabled) && routebutton->Hitbox(e.pos)
			|| monobutton->Hitbox(e.pos) || mutebutton->Hitbox(e.pos) || panslider->Hitbox(e.pos))
		{
			e.Handle();
			return;
		}

		if (Channel::selected)
			Channel::selected->State(Selected) = false;

		Channel::selected = this;
		State(Selected) = true;
		e.Handle();
	};
}

void Channel::NewSelect()
{
	if (!selected || (selected->channel->type & channel->type & (Audio::Channel::Type::Input | Audio::Channel::Type::Output))
		|| ((channel->type & Audio::Channel::Type::Forward) || (selected->channel->type & Audio::Channel::Type::Forward)))
		routebutton->State(Disabled) = true;
	else
		routebutton->State(Disabled) = false, routebutton->State(Selected) = channel->Connected(selected->channel);
}

void Channel::Update() 
{
	gain->channel = channel;
	channel->volume = gain->Gain();
	channel->volume.midi = gain->midi;
	channel->pan = panslider->Normalize(panslider->value) * 2 - 1;
	channel->pan.midi = panslider->midi;
	channel->UpdatePans();
}

void Channel::Render(CommandCollection& d) const 
{
	if (border.width != 0)
	{
		d.Fill(border.color.Current());
		d.Quad(dimensions);
	}
	d.Fill(background.Current());
	d.Quad({ x + border.width, y + border.width, width - border.width * 2, height - border.width * 2 });
}

ChannelParser::ChannelParser()
{
	Parser::Link<GainSliderParser>();
	Parser::Link<RouteButtonParser>();
	settings.name = "channel";
	Attribute("background", &Channel::m_Background);
	Attribute("border-color", &Channel::m_BorderColor);
	Attribute("border-width", &Channel::m_BorderWidth);

	enumMap["route-button"] = Channel::ROUTE_BUTTON;
	enumMap["mono-button"] = Channel::MONO_BUTTON;
	enumMap["mute-button"] = Channel::MUTE_BUTTON;
	enumMap["gain-slider"] = Channel::GAIN_SLIDER;
	enumMap["name-box"] = Channel::NAME_BOX;
	enumMap["pan-slider"] = Channel::PAN_SLIDER;
}

Pointer<Component> ChannelParser::Create()
{
	return new Channel{};
}