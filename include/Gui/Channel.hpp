#pragma once
#include "pch.hpp"
#include "Audio/Channel.hpp"
#include "Gui/GainSlider.hpp"
#include "Gui/RouteButton.hpp"
#include "Gui/Slider.hpp"

struct Channel : public Panel
{
	static inline Channel* selected = nullptr;

	Pointer<Audio::Channel> channel;

	Menu menu;
	TextBox name;
	GainSlider gain;
	Button routebutton;
	Button monobutton;
	Button mutebutton;
	Slider panslider;

	Channel(bool input);
	Channel(const Channel&) = delete;
	Channel(Channel&& other) = delete;
	Channel& operator=(const Channel& other) = delete;
	Channel& operator=(Channel&& other) = delete;

	void operator=(const Pointer<Audio::Channel>& c);

	void Init(bool input);

	void NewSelect();

	void Update() override;
	void Render(CommandCollection& d) const override;
};

