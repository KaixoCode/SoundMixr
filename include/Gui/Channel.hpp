#pragma once
#include "pch.hpp"
#include "GuiCode2/Panel.hpp"
#include "Audio/Channel.hpp"
#include "Gui/GainSlider.hpp"
#include "Gui/RouteButton.hpp"

struct ChannelParser : public PanelParser
{
	ChannelParser();
	Pointer<Component> Create();
};

struct Channel : public Panel
{
	static inline int ROUTE_BUTTON = 0xA9FB391A;
	static inline int MUTE_BUTTON = 0xA9FB391B;
	static inline int MONO_BUTTON = 0xA9FB391C;
	static inline int GAIN_SLIDER = 0xA9FB391D;
	static inline int NAME_BOX = 0xA9FB391E;
	static inline int PAN_SLIDER = 0xA9FB391F;

	static inline Parser::Scope generator;
	static inline Channel* selected = nullptr;

	Pointer<Audio::Channel> channel;

	Pointer<Menu> menu = new Menu;
	Pointer<TextBox> name = new TextBox;
	Pointer<GainSlider> gain = new GainSlider;
	Pointer<Button> routebutton = new RouteButton;
	Pointer<Button> monobutton = new Button;
	Pointer<Button> mutebutton = new Button;
	Pointer<Parameter> panslider = new Parameter;

	struct Settings
	{
		StateColors background{ {
			.base = { 26, 26, 26 },
			.colors{
				{ Selected, { 36, 36, 36 } },
			}
		} };

		struct Border
		{
			StateColors color{ {
				.base = { 36, 36, 36 },
				.colors{
					{ Selected, { 46, 46, 46 } },
				}
			} };
			float width = 0;
		} border;
	} settings;

	Channel(const Settings& s = {});
	Channel(Channel&& other) = delete;
	Channel(const Channel&) = delete;
	Channel& operator=(Channel&& other);
	Channel& operator=(const Channel& other) = delete;

	void operator=(const Pointer<Audio::Channel>& c);

	void Init(bool input);

	void NewSelect();

	void Update() override;
	void Render(CommandCollection& d) const override;

	Settings::Border& border = settings.border;
	StateColors& background = settings.background;

private:
	Ref<float> m_BorderWidth = settings.border.width;
	Ref<StateColors> m_BorderColor = settings.border.color;
	Ref<StateColors> m_Background = settings.background;

	friend class ChannelParser;
};

