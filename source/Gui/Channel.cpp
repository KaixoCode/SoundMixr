#include "Gui/Channel.hpp"
#include "Controller.hpp"

Channel::Channel(bool input)
	: routebutton( {
			.type = Button::Toggle,
			.callback = [this](bool v)
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
			},
			.graphics = new RouteButtonGraphics,
		}), monobutton(), mutebutton(), gain()
{
	routebutton.settings.graphics.Get<RouteButtonGraphics>()->input = input;
	border.color.Link(this);
	background.Link(this);

	this->Panel::operator=(Panel{ {
		.layout = Layout::Column,
		.padding = { 2, 2, 2, 2 },
		.margin = { 2, 2, 2, 2 },
		.size = { Auto, Inherit },
		.background = { {.base{ 73, 80, 87 } } }
	}, { {
		name, 
		new Panel{ { 
			.ratio = 1,
			.size = { Auto, Inherit }
		}, { gain } },
		new Panel{ {
			.margin = { 0, 4, 0, 2 },
			.size = { Inherit, 35 }
		}, { {
			new Panel{ {
				.ratio = 1,
				.margin = { 3, 3, 3, 3 }
			}, { monobutton  } },
			new Panel{ {
				.ratio = 1,
				.margin = { 3, 3, 3, 3 }
			}, { mutebutton  }  }
		} } },
		new Panel{ {
			.margin = { 2, 0, 2, 3 },
			.size = { Inherit, 18 }
		}, { panslider } },
		new Panel{ {
			.layout = Layout::Column,
			.size = { Inherit, 22 }
		}, { routebutton } },
	} } });

	width = 70;

	name += [this](const Unfocus&)
	{
		channel->name = name.content;
	};

	*this += [this, input](const MouseRelease& e)
	{
		if (e.button == MouseButton::Right && !e.Handled())
		{
			menu.components.clear();
			SubMenuButton& _sub = menu.components.emplace_back(new SubMenuButton{ {.name = "Select Channels"} });
			auto& _endpoints = input ? Controller::Get().audio.inputs : Controller::Get().audio.outputs;
			for (auto& i : _endpoints)
			{
				Audio::EndpointChannel& _channel = channel;
				Button& _button = _sub.menu.components.emplace_back(new Button{ {
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

			menu.components.push_back(new Button{ {
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

	*this += [this](const MousePress& e)
	{
		// Don't switch when pressing button
		if (!routebutton.State(Disabled) && routebutton.Hitbox(e.pos)
			|| monobutton.Hitbox(e.pos) || mutebutton.Hitbox(e.pos) || panslider.Hitbox(e.pos))
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

void Channel::operator=(const Pointer<Audio::Channel>& c)
{
	if (channel == c)
		return;

	channel = c;
	name.content = c->name;
	name.displayer.RecalculateLines();
	gain.value = lin2db(c->volume);
	gain.midi = c->volume.midi;
	mutebutton.State(Selected) = c->mute;
	monobutton.State(Selected) = c->mono;
	panslider.value = panslider.Unnormalize(c->pan * 0.5 + 0.5);
	panslider.midi = panslider.midi;

}

void Channel::NewSelect()
{
	if (!selected || (selected->channel->type & channel->type & (Audio::Channel::Type::Input | Audio::Channel::Type::Output))
		|| ((channel->type & Audio::Channel::Type::Forward) || (selected->channel->type & Audio::Channel::Type::Forward)))
		routebutton.State(Disabled) = true;
	else
		routebutton.State(Disabled) = false, routebutton.State(Selected) = channel->Connected(selected->channel);
}

void Channel::Update() 
{
	gain.channel = channel;
	channel->volume = gain.Gain();
	channel->volume.midi = gain.midi;
	channel->pan = panslider.Normalize(panslider.value) * 2 - 1;
	channel->pan.midi = panslider.midi;
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