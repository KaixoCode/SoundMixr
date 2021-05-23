#pragma once
#include "pch.hpp"

/**
 * Extremely big mess, just... look in here whenever you need to... 
 * it's all just draw instructions and the theme thing...
 */

class ThemeT
{
public:
	Color
		text{ 255, 255, 255, 255 },
		text_small{ 200, 200, 200, 255 },
		textOff{ 128, 128, 128, 255 };

	Color // Button
		button_disabled_background{ 29, 29, 29, 255 },
		button_disabled_border{ 23, 23, 23, 255 },
		button_disabled_text{ 128, 128, 128, 255 },
		button_active_background{ 65, 65, 65, 255 },
		button_active_border{ 23, 23, 23, 255 },
		button_active_text{ 255, 255, 255, 255 },
		button_hovering_background{ 40, 40, 40, 255 },
		button_hovering_border{ 23, 23, 23, 255 },
		button_hovering_text{ 255, 255, 255, 255 },
		button_idle_background{ 29, 29, 29, 255 },
		button_idle_border{ 23, 23, 23, 255 },
		button_idle_text{ 255, 255, 255, 255 };


	Color // Toggle button
		toggle_button_disabled_background{ 29, 29, 29, 255 },
		toggle_button_disabled_border{ 23, 23, 23, 255 },
		toggle_button_disabled_text{ 128, 128, 128, 255 },
		toggle_button_active_background{ 65, 65, 65, 255 },
		toggle_button_active_border{ 23, 23, 23, 255 },
		toggle_button_active_text{ 255, 255, 255, 255 },
		toggle_button_hovering_background{ 40, 40, 40, 255 },
		toggle_button_hovering_border{ 23, 23, 23, 255 },
		toggle_button_hovering_text{ 128, 128, 128, 255 },
		toggle_button_idle_background{ 29, 29, 29, 255 },
		toggle_button_idle_border{ 23, 23, 23, 255 },
		toggle_button_idle_text{ 128, 128, 128, 255 };


	Color // DropDown button
		dropdown_button_disabled_background{ 29, 29, 29, 255 },
		dropdown_button_disabled_border{ 23, 23, 23, 255 },
		dropdown_button_disabled_text{ 128, 128, 128, 255 },
		dropdown_button_active_background{ 65, 65, 65, 255 },
		dropdown_button_active_border{ 23, 23, 23, 255 },
		dropdown_button_active_text{ 255, 255, 255, 255 },
		dropdown_button_hovering_background{ 40, 40, 40, 255 },
		dropdown_button_hovering_border{ 23, 23, 23, 255 },
		dropdown_button_hovering_text{ 255, 255, 255, 255 },
		dropdown_button_idle_background{ 29, 29, 29, 255 },
		dropdown_button_idle_border{ 23, 23, 23, 255 },
		dropdown_button_idle_text{ 255, 255, 255, 255 };

	Color // Radio button
		radio_button_disabled_background{ 29, 29, 29, 255 },
		radio_button_disabled_border{ 23, 23, 23, 255 },
		radio_button_disabled_text{ 128, 128, 128, 255 },
		radio_button_active_background{ 65, 65, 65, 255 },
		radio_button_active_border{ 23, 23, 23, 255 },
		radio_button_active_text{ 255, 255, 255, 255 },
		radio_button_hovering_background{ 40, 40, 40, 255 },
		radio_button_hovering_border{ 23, 23, 23, 255 },
		radio_button_hovering_text{ 128, 128, 128, 255 },
		radio_button_idle_background{ 29, 29, 29, 255 },
		radio_button_idle_border{ 23, 23, 23, 255 },
		radio_button_idle_text{ 128, 128, 128, 255 };

	Color // Menu button
		menu_button_disabled_background{ 0, 0, 0, 0 },
		menu_button_disabled_text{ 128, 128, 128, 255 },
		menu_button_disabled_text_small{ 128, 128, 128, 255 },
		menu_button_active_background{ 255, 255, 255, 26 },
		menu_button_active_text{ 255, 255, 255, 255 },
		menu_button_active_text_small{ 200, 200, 200, 255 },
		menu_button_hovering_background{ 255, 255, 255, 13 },
		menu_button_hovering_text{ 255, 255, 255, 255 },
		menu_button_hovering_text_small{ 200, 200, 200, 255 },
		menu_button_idle_background{ 0, 0, 0, 0 },
		menu_button_idle_text{ 255, 255, 255, 255 },
		menu_button_idle_text_small{ 200, 200, 200, 255 };

	Color // Toggle menu button
		toggle_menu_button_disabled_background{ 0, 0, 0, 0 },
		toggle_menu_button_disabled_text{ 128, 128, 128, 255 },
		toggle_menu_button_disabled_text_small{ 128, 128, 128, 255 },
		toggle_menu_button_active_background{ 255, 255, 255, 26 },
		toggle_menu_button_active_square{ 255, 255, 255, 26 },
		toggle_menu_button_active_text{ 255, 255, 255, 255 },
		toggle_menu_button_active_text_small{ 200, 200, 200, 255 },
		toggle_menu_button_hovering_background{ 255, 255, 255, 13 },
		toggle_menu_button_hovering_text{ 255, 255, 255, 255 },
		toggle_menu_button_hovering_text_small{ 200, 200, 200, 255 },
		toggle_menu_button_idle_background{ 0, 0, 0, 0 },
		toggle_menu_button_idle_text{ 255, 255, 255, 255 },
		toggle_menu_button_idle_text_small{ 200, 200, 200, 255 };

	Color // Scrollbar
		scrollbar_idle_handle{ 55, 55, 55, 255 },
		scrollbar_idle_bar{ 0, 0, 0, 0 },
		scrollbar_hovering_handle{ 75, 75, 75, 255 },
		scrollbar_hovering_bar{ 55, 55, 55, 255 },
		scrollbar_active_handle{ 75, 75, 75, 255 },
		scrollbar_active_bar{ 55, 55, 55, 255 };

	Color // Slider
		slider_disabled_background{ 29, 29, 29, 255 },
		slider_disabled_border{ 23, 23, 23, 255 },
		slider_disabled_value{ 65, 65, 65, 255 },
		slider_disabled_value_text{ 128, 128, 128, 255 },
		slider_disabled_name_text{ 128, 128, 128, 255 },
		slider_idle_background{ 29, 29, 29, 255 },
		slider_idle_border{ 23, 23, 23, 255 },
		slider_idle_value{ 65, 65, 65, 255 },
		slider_idle_value_text{ 255, 255, 255, 255 },
		slider_idle_name_text{ 255, 255, 255, 255 },
		slider_hovering_background{ 29, 29, 29, 255 },
		slider_hovering_border{ 23, 23, 23, 255 },
		slider_hovering_value{ 65, 65, 65, 255 },
		slider_hovering_value_text{ 255, 255, 255, 255 },
		slider_hovering_name_text{ 255, 255, 255, 255 },
		slider_active_background{ 29, 29, 29, 255 },
		slider_active_border{ 23, 23, 23, 255 },
		slider_active_value{ 65, 65, 65, 255 },
		slider_active_value_text{ 255, 255, 255, 255 },
		slider_active_name_text{ 255, 255, 255, 255 };

	Color // Knob
		knob_disabled_handle{ 120, 120, 120, 255 },
		knob_disabled_border{ 45, 45, 45, 255 },
		knob_disabled_value{ 85, 85, 85, 255 },
		knob_disabled_name_text{ 128, 128, 128, 255 },
		knob_disabled_value_text{ 128, 128, 128, 255 },
		knob_idle_handle{ 120, 120, 120, 255 },
		knob_idle_border{ 45, 45, 45, 255 },
		knob_idle_value{ 85, 85, 85, 255 },
		knob_idle_name_text{ 255, 255, 255, 255 },
		knob_idle_value_text{ 255, 255, 255, 255 },
		knob_hovering_handle{ 120, 120, 120, 255 },
		knob_hovering_border{ 45, 45, 45, 255 },
		knob_hovering_value{ 85, 85, 85, 255 },
		knob_hovering_name_text{ 255, 255, 255, 255 },
		knob_hovering_value_text{ 255, 255, 255, 255 },
		knob_active_handle{ 120, 120, 120, 255 },
		knob_active_border{ 45, 45, 45, 255 },
		knob_active_value{ 85, 85, 85, 255 },
		knob_active_name_text{ 255, 255, 255, 255 },
		knob_active_value_text{ 255, 255, 255, 255 };

	Color // Volume slider
		volume_slider_disabled_handle{ 100, 100, 100, 255 },
		volume_slider_disabled_db_text{ 128, 128, 128, 255 },
		volume_slider_disabled_value_text{ 255, 255, 255, 255 },
		volume_slider_disabled_line{ 64, 64, 64, 255 },
		volume_slider_disabled_line_highlight{ 128, 128, 128, 255 },
		volume_slider_disabled_meter{ 15, 15, 15, 255 },
		volume_slider_disabled_meter_value{ 0, 255, 0, 255 },
		volume_slider_disabled_meter_value_c1{ 255, 255, 0, 255 },
		volume_slider_disabled_meter_value_c2{ 255, 126, 0, 255 },
		volume_slider_disabled_meter_value_c3{ 255, 0, 0, 255 },
		volume_slider_idle_handle{ 100, 100, 100, 255 },
		volume_slider_idle_db_text{ 128, 128, 128, 255 },
		volume_slider_idle_value_text{ 255, 255, 255, 255 },
		volume_slider_idle_line{ 64, 64, 64, 255 },
		volume_slider_idle_line_highlight{ 128, 128, 128, 255 },
		volume_slider_idle_meter{ 15, 15, 15, 255 },
		volume_slider_idle_meter_value{ 0, 255, 0, 255 },
		volume_slider_idle_meter_value_c1{ 255, 255, 0, 255 },
		volume_slider_idle_meter_value_c2{ 255, 126, 0, 255 },
		volume_slider_idle_meter_value_c3{ 255, 0, 0, 255 },
		volume_slider_hovering_handle{ 100, 100, 100, 255 },
		volume_slider_hovering_db_text{ 128, 128, 128, 255 },
		volume_slider_hovering_value_text{ 255, 255, 255, 255 },
		volume_slider_hovering_line{ 64, 64, 64, 255 },
		volume_slider_hovering_line_highlight{ 128, 128, 128, 255 },
		volume_slider_hovering_meter{ 15, 15, 15, 255 },
		volume_slider_hovering_meter_value{ 0, 255, 0, 255 },
		volume_slider_hovering_meter_value_c1{ 255, 255, 0, 255 },
		volume_slider_hovering_meter_value_c2{ 255, 126, 0, 255 },
		volume_slider_hovering_meter_value_c3{ 255, 0, 0, 255 },
		volume_slider_active_handle{ 100, 100, 100, 255 },
		volume_slider_active_db_text{ 128, 128, 128, 255 },
		volume_slider_active_value_text{ 255, 255, 255, 255 },
		volume_slider_active_line{ 64, 64, 64, 255 },
		volume_slider_active_line_highlight{ 128, 128, 128, 255 },
		volume_slider_active_meter{ 15, 15, 15, 255 },
		volume_slider_active_meter_value{ 0, 255, 0, 255 },
		volume_slider_active_meter_value_c1{ 255, 255, 0, 255 },
		volume_slider_active_meter_value_c2{ 255, 126, 0, 255 },
		volume_slider_active_meter_value_c3{ 255, 0, 0, 255 };

	Color // XY-Controller
		effect_graph_background{ 15, 15, 15, 255 },
		effect_graph_graph_lines{ 30, 30, 30, 255 },
		effect_graph_graph_lines_highlight{ 40, 40, 40, 255 },
		effect_graph_disabled_circle{ 90, 90, 90, 255 },
		effect_graph_disabled_line{ 70, 70, 70, 255 },
		effect_graph_idle_circle{ 150, 150, 150, 255 },
		effect_graph_idle_line{ 130, 130, 130, 255 },
		effect_graph_hovering_circle{ 150, 150, 150, 255 },
		effect_graph_hovering_line{ 130, 130, 130, 255 },
		effect_graph_active_circle{ 150, 150, 150, 255 },
		effect_graph_active_line{ 130, 130, 130, 255 };

	Color // Dynamics
		dynamics_background{ 15, 15, 15, 255 },
		dynamics_border{ 100, 100, 100, 100 },
		dynamics_line{ 64, 64, 64, 255 },
		dynamics_line_highlight{ 128, 128, 128, 255 },
		dynamics_compressor{ 100, 100, 100, 50 },
		dynamics_compressor_line{ 100, 100, 100, 100 },
		dynamics_meter_value{ 0, 255, 0, 255 },
		dynamics_db_text = text,
		dynamics_value_text = text;

	Color // Route button
		route_button_idle_background{ 0, 0, 0, 0 },
		route_button_idle_triangle{ 50, 50, 50, 255 },
		route_button_hovering_background{ 0, 0, 0, 0 },
		route_button_hovering_triangle{ 70, 70, 70, 255 },
		route_button_active_background{ 0, 0, 0, 0 },
		route_button_active_triangle{ 100, 100, 100, 255 };

	Color // Channel
		channel_idle_name_text{ 255, 255, 255, 255 },
		channel_idle_background{ 33, 33, 33, 255 },
		channel_hovering_name_text{ 255, 255, 255, 255 },
		channel_hovering_background{ 33, 33, 33, 255 },
		channel_active_name_text{ 255, 255, 255, 255 },
		channel_active_background{ 29, 29, 29, 255 };

	Color // mute mono button
		mute_mono_button_idle_background{ 50, 50, 50, 255 },
		mute_mono_button_idle_text{ 128, 128, 128, 255 },
		mute_mono_button_hovering_background{ 70, 70, 70, 255 },
		mute_mono_button_hovering_text{ 128, 128, 128, 255 },
		mute_mono_button_active_background{ 90, 90, 90, 255 },
		mute_mono_button_active_text = text;

	Color // Effect
		effect_title_text{ 255, 255, 255, 255 },
		effect_title_text_off{ 128, 128, 128, 255 },
		effect_title_bar{ 29, 29, 29, 255 },
		effect_background{ 33, 33, 33, 255 },
		effect_minimize_button{ 128, 128, 128, 255 },
		effect_divider{ 51, 51, 51, 255 },
		effect_drag_divider{ 51, 51, 51, 255 };

	Color // Divider
		divider{ 51, 51, 51, 255 };

	Color // Window
		window_border{ 23, 23, 23, 255 },
		window_frame{ 33, 33, 33, 255 },
		window_panel{ 40, 40, 40, 255 },
		window_title_text{ 200, 200, 200, 255 },
		window_menu_text{ 255, 255, 255, 255 },
		window_close_button_icon{ 255, 255, 255, 255 },
		window_close_button_idle_background{ 0, 0, 0, 0 },
		window_close_button_hovering_background{ 255, 0, 0, 111 },
		window_close_button_active_background{ 170, 0, 0, 111 },
		window_button_icon{ 255, 255, 255, 255 },
		window_button_idle_background{ 0, 0, 0, 0 },
		window_button_hovering_background{ 255, 255, 255, 31 },
		window_button_active_background{ 255, 255, 255, 58 };

	Color // Menu
		menu_background{ 18, 18, 18, 255 },
		menu_border{ 64, 64, 64, 255 },
		menu_divider{ 51, 51, 51, 255 };

	Color // Selection
		text_selection{ 128, 128, 255, 255 };

	int font = Graphics::Fonts::Gidole;

	ThemeT()
	{}

	~ThemeT()
	{}

	void FreeFonts()
	{
		// Free the fonts
		if (font != Graphics::Fonts::Gidole)
			Graphics::FreeFont(font);
	}

	ThemeT(nlohmann::json json)
		: m_Json(json)
	{
		LoadTheme();
	}

	void LoadTheme()
	{
		try
		{
			try { m_Name = m_Json.at("name").get<std::string>(); }
			catch (std::exception) { m_Name = "Theme"; }

			try { for (auto& [key, v] : m_Json.at("variables").items())
				m_Variables.emplace(key, Color{ v.at(0), v.at(1), v.at(2), v.at(3) }); }
			catch (std::exception) {}

			try { m_Font = m_Json.at("font").get<std::string>(); }
			catch (std::exception) { m_Font = ""; }
			
#define SetColor(name, ...) GetColor(name, #name, __VA_ARGS__);

			SetColor(text      , "text");
			SetColor(text_small, "textsmall");
			SetColor(textOff   , "textoff");

			SetColor(button_disabled_background, "button", "disabled", "background");
			SetColor(button_disabled_border	   , "button", "disabled", "border");
			SetColor(button_disabled_text	   , "button", "disabled", "text");
			SetColor(button_active_background  , "button", "active", "background");
			SetColor(button_active_border	   , "button", "active", "border");
			SetColor(button_active_text		   , "button", "active", "text");
			SetColor(button_hovering_background, "button", "hovering", "background");
			SetColor(button_hovering_border	   , "button", "hovering", "border");
			SetColor(button_hovering_text	   , "button", "hovering", "text");
			SetColor(button_idle_background	   , "button", "idle", "background");
			SetColor(button_idle_border		   , "button", "idle", "border");
			SetColor(button_idle_text		   , "button", "idle", "text");

			SetColor(toggle_button_disabled_background, "toggle-button", "disabled", "background");
			SetColor(toggle_button_disabled_border	  , "toggle-button", "disabled", "border");
			SetColor(toggle_button_disabled_text	  , "toggle-button", "disabled", "text");
			SetColor(toggle_button_active_background  , "toggle-button", "active", "background");
			SetColor(toggle_button_active_border	  , "toggle-button", "active", "border");
			SetColor(toggle_button_active_text		  , "toggle-button", "active", "text");
			SetColor(toggle_button_hovering_background, "toggle-button", "hovering", "background");
			SetColor(toggle_button_hovering_border	  , "toggle-button", "hovering", "border");
			SetColor(toggle_button_hovering_text	  , "toggle-button", "hovering", "text");
			SetColor(toggle_button_idle_background	  , "toggle-button", "idle", "background");
			SetColor(toggle_button_idle_border		  , "toggle-button", "idle", "border");
			SetColor(toggle_button_idle_text		  , "toggle-button", "idle", "text");

			SetColor(dropdown_button_disabled_background, "dropdown-button", "disabled", "background");
			SetColor(dropdown_button_disabled_border	, "dropdown-button", "disabled", "border");
			SetColor(dropdown_button_disabled_text		, "dropdown-button", "disabled", "text");
			SetColor(dropdown_button_active_background	, "dropdown-button", "active", "background");
			SetColor(dropdown_button_active_border		, "dropdown-button", "active", "border");
			SetColor(dropdown_button_active_text		, "dropdown-button", "active", "text");
			SetColor(dropdown_button_hovering_background, "dropdown-button", "hovering", "background");
			SetColor(dropdown_button_hovering_border	, "dropdown-button", "hovering", "border");
			SetColor(dropdown_button_hovering_text		, "dropdown-button", "hovering", "text");
			SetColor(dropdown_button_idle_background	, "dropdown-button", "idle", "background");
			SetColor(dropdown_button_idle_border		, "dropdown-button", "idle", "border");
			SetColor(dropdown_button_idle_text			, "dropdown-button", "idle", "text");

			SetColor(radio_button_disabled_background, "radio-button", "disabled", "background");
			SetColor(radio_button_disabled_border	 , "radio-button", "disabled", "border");
			SetColor(radio_button_disabled_text		 , "radio-button", "disabled", "text");
			SetColor(radio_button_active_background	 , "radio-button", "active", "background");
			SetColor(radio_button_active_border		 , "radio-button", "active", "border");
			SetColor(radio_button_active_text		 , "radio-button", "active", "text");
			SetColor(radio_button_hovering_background, "radio-button", "hovering", "background");
			SetColor(radio_button_hovering_border	 , "radio-button", "hovering", "border");
			SetColor(radio_button_hovering_text		 , "radio-button", "hovering", "text");
			SetColor(radio_button_idle_background	 , "radio-button", "idle", "background");
			SetColor(radio_button_idle_border		 , "radio-button", "idle", "border");
			SetColor(radio_button_idle_text			 , "radio-button", "idle", "text");

			SetColor(menu_button_disabled_background, "menu-button", "disabled", "background");
			SetColor(menu_button_disabled_text		, "menu-button", "disabled", "text");
			SetColor(menu_button_disabled_text_small, "menu-button", "disabled", "text-small");
			SetColor(menu_button_active_background	, "menu-button", "active", "background");
			SetColor(menu_button_active_text		, "menu-button", "active", "text");
			SetColor(menu_button_active_text_small	, "menu-button", "active", "text-small");
			SetColor(menu_button_hovering_background, "menu-button", "hovering", "background");
			SetColor(menu_button_hovering_text		, "menu-button", "hovering", "text");
			SetColor(menu_button_hovering_text_small, "menu-button", "hovering", "text-small");
			SetColor(menu_button_idle_background	, "menu-button", "idle", "background");
			SetColor(menu_button_idle_text			, "menu-button", "idle", "text");
			SetColor(menu_button_idle_text_small	, "menu-button", "idle", "text-small");

			SetColor(toggle_menu_button_disabled_background, "toggle-menu-button", "disabled", "background");
			SetColor(toggle_menu_button_disabled_text	   , "toggle-menu-button", "disabled", "text");
			SetColor(toggle_menu_button_disabled_text_small, "toggle-menu-button", "disabled", "text-small");
			SetColor(toggle_menu_button_active_background  , "toggle-menu-button", "active", "background");
			SetColor(toggle_menu_button_active_text		   , "toggle-menu-button", "active", "text");
			SetColor(toggle_menu_button_active_text_small  , "toggle-menu-button", "active", "text-small");
			SetColor(toggle_menu_button_active_square      , "toggle-menu-button", "active", "square");
			SetColor(toggle_menu_button_hovering_background, "toggle-menu-button", "hovering", "background");
			SetColor(toggle_menu_button_hovering_text	   , "toggle-menu-button", "hovering", "text");
			SetColor(toggle_menu_button_hovering_text_small, "toggle-menu-button", "hovering", "text-small");
			SetColor(toggle_menu_button_idle_background	   , "toggle-menu-button", "idle", "background");
			SetColor(toggle_menu_button_idle_text		   , "toggle-menu-button", "idle", "text");
			SetColor(toggle_menu_button_idle_text_small	   , "toggle-menu-button", "idle", "text-small");

			SetColor(scrollbar_idle_handle	  , "scrollbar", "idle", "handle");
			SetColor(scrollbar_idle_bar		  , "scrollbar", "idle", "bar");
			SetColor(scrollbar_hovering_handle, "scrollbar", "hovering", "handle");
			SetColor(scrollbar_hovering_bar	  , "scrollbar", "hovering", "bar");
			SetColor(scrollbar_active_handle  , "scrollbar", "active", "handle");
			SetColor(scrollbar_active_bar	  , "scrollbar", "active", "bar");

			SetColor(slider_disabled_background, "slider", "disabled", "background");
			SetColor(slider_disabled_border	   , "slider", "disabled", "border");
			SetColor(slider_disabled_value	   , "slider", "disabled", "value");
			SetColor(slider_disabled_value_text, "slider", "disabled", "value-text");
			SetColor(slider_disabled_name_text , "slider", "disabled", "name-text");
			SetColor(slider_idle_background	   , "slider", "idle", "background");
			SetColor(slider_idle_border		   , "slider", "idle", "border");
			SetColor(slider_idle_value		   , "slider", "idle", "value");
			SetColor(slider_idle_value_text	   , "slider", "idle", "value-text");
			SetColor(slider_idle_name_text	   , "slider", "idle", "name-text");
			SetColor(slider_hovering_background, "slider", "hovering", "background");
			SetColor(slider_hovering_border	   , "slider", "hovering", "border");
			SetColor(slider_hovering_value	   , "slider", "hovering", "value");
			SetColor(slider_hovering_value_text, "slider", "hovering", "value-text");
			SetColor(slider_hovering_name_text , "slider", "hovering", "name-text");
			SetColor(slider_active_background  , "slider", "active", "background");
			SetColor(slider_active_border	   , "slider", "active", "border");
			SetColor(slider_active_value	   ,"slider", "active", "value");
			SetColor(slider_active_value_text  , "slider", "active", "value-text");
			SetColor(slider_active_name_text   , "slider", "active", "name-text");

			SetColor(knob_disabled_handle	 , "knob", "disabled", "handle");
			SetColor(knob_disabled_border	 , "knob", "disabled", "border");
			SetColor(knob_disabled_value	 , "knob", "disabled", "value");
			SetColor(knob_disabled_name_text , "knob", "disabled", "name-text");
			SetColor(knob_disabled_value_text, "knob", "disabled", "value-text");
			SetColor(knob_idle_handle		 , "knob", "idle", "handle");
			SetColor(knob_idle_border		 , "knob", "idle", "border");
			SetColor(knob_idle_value		 , "knob", "idle", "value");
			SetColor(knob_idle_name_text	 , "knob", "idle", "name-text");
			SetColor(knob_idle_value_text	 , "knob", "idle", "value-text");
			SetColor(knob_hovering_handle	 , "knob", "hovering", "handle");
			SetColor(knob_hovering_border	 , "knob", "hovering", "border");
			SetColor(knob_hovering_value	 , "knob", "hovering", "value");
			SetColor(knob_hovering_name_text , "knob", "hovering", "name-text");
			SetColor(knob_hovering_value_text, "knob", "hovering", "value-text");
			SetColor(knob_active_handle		 , "knob", "active", "handle");
			SetColor(knob_active_border		 , "knob", "active", "border");
			SetColor(knob_active_value		 , "knob", "active", "value");
			SetColor(knob_active_name_text	 , "knob", "active", "name-text");
			SetColor(knob_active_value_text	 , "knob", "active", "value-text");

			SetColor(volume_slider_disabled_handle		  , "volume-slider", "disabled", "handle");
			SetColor(volume_slider_disabled_db_text		  , "volume-slider", "disabled", "db-text");
			SetColor(volume_slider_disabled_value_text	  , "volume-slider", "disabled", "value-text");
			SetColor(volume_slider_disabled_line		  , "volume-slider", "disabled", "line");
			SetColor(volume_slider_disabled_line_highlight, "volume-slider", "disabled", "line-highlight");
			SetColor(volume_slider_disabled_meter		  , "volume-slider", "disabled", "meter");
			SetColor(volume_slider_disabled_meter_value	  , "volume-slider", "disabled", "meter-value");
			SetColor(volume_slider_disabled_meter_value_c1, "volume-slider", "disabled", "meter-value-c1");
			SetColor(volume_slider_disabled_meter_value_c2, "volume-slider", "disabled", "meter-value-c2");
			SetColor(volume_slider_disabled_meter_value_c3, "volume-slider", "disabled", "meter-value-c3");
			SetColor(volume_slider_idle_handle			  , "volume-slider", "idle", "handle");
			SetColor(volume_slider_idle_db_text			  , "volume-slider", "idle", "db-text");
			SetColor(volume_slider_idle_value_text		  , "volume-slider", "idle", "value-text");
			SetColor(volume_slider_idle_line			  , "volume-slider", "idle", "line");
			SetColor(volume_slider_idle_line_highlight	  , "volume-slider", "idle", "line-highlight");
			SetColor(volume_slider_idle_meter			  , "volume-slider", "idle", "meter");
			SetColor(volume_slider_idle_meter_value		  , "volume-slider", "idle", "meter-value");
			SetColor(volume_slider_idle_meter_value_c1	  , "volume-slider", "idle", "meter-value-c1");
			SetColor(volume_slider_idle_meter_value_c2	  , "volume-slider", "idle", "meter-value-c2");
			SetColor(volume_slider_idle_meter_value_c3	  , "volume-slider", "idle", "meter-value-c3");
			SetColor(volume_slider_hovering_handle		  , "volume-slider", "hovering", "handle");
			SetColor(volume_slider_hovering_db_text		  , "volume-slider", "hovering", "db-text");
			SetColor(volume_slider_hovering_value_text	  , "volume-slider", "hovering", "value-text");
			SetColor(volume_slider_hovering_line		  , "volume-slider", "hovering", "line");
			SetColor(volume_slider_hovering_line_highlight, "volume-slider", "hovering", "line-highlight");
			SetColor(volume_slider_hovering_meter		  , "volume-slider", "hovering", "meter");
			SetColor(volume_slider_hovering_meter_value	  , "volume-slider", "hovering", "meter-value");
			SetColor(volume_slider_hovering_meter_value_c1, "volume-slider", "hovering", "meter-value-c1");
			SetColor(volume_slider_hovering_meter_value_c2, "volume-slider", "hovering", "meter-value-c2");
			SetColor(volume_slider_hovering_meter_value_c3, "volume-slider", "hovering", "meter-value-c3");
			SetColor(volume_slider_active_handle		  , "volume-slider", "active", "handle");
			SetColor(volume_slider_active_db_text		  , "volume-slider", "active", "db-text");
			SetColor(volume_slider_active_value_text      , "volume-slider", "active", "value-text");
			SetColor(volume_slider_active_line			  , "volume-slider", "active", "line");
			SetColor(volume_slider_active_line_highlight  , "volume-slider", "active", "line-highlight");
			SetColor(volume_slider_active_meter			  , "volume-slider", "active", "meter");
			SetColor(volume_slider_active_meter_value	  , "volume-slider", "active", "meter-value");
			SetColor(volume_slider_active_meter_value_c1  , "volume-slider", "active", "meter-value-c1");
			SetColor(volume_slider_active_meter_value_c2  , "volume-slider", "active", "meter-value-c2");
			SetColor(volume_slider_active_meter_value_c3  , "volume-slider", "active", "meter-value-c3");

			SetColor(effect_graph_background		   , "effect-graph", "background");
			SetColor(effect_graph_graph_lines		   , "effect-graph", "graph-lines");
			SetColor(effect_graph_graph_lines_highlight, "effect-graph", "graph-lines-highlight");
			SetColor(effect_graph_disabled_circle	   , "effect-graph", "disabled", "circle");
			SetColor(effect_graph_disabled_line		   , "effect-graph", "disabled", "line");
			SetColor(effect_graph_idle_circle		   , "effect-graph", "idle", "circle");
			SetColor(effect_graph_idle_line   		   , "effect-graph", "idle", "line");
			SetColor(effect_graph_hovering_circle	   , "effect-graph", "hovering", "circle");
			SetColor(effect_graph_hovering_line		   , "effect-graph", "hovering", "line");
			SetColor(effect_graph_active_circle		   , "effect-graph", "active", "circle");
			SetColor(effect_graph_active_line		   , "effect-graph", "active", "line");

			SetColor(dynamics_background	 , "dynamics", "background");
			SetColor(dynamics_border		 , "dynamics", "border");
			SetColor(dynamics_line			 , "dynamics", "line");
			SetColor(dynamics_line_highlight , "dynamics", "line-highlight");
			SetColor(dynamics_compressor	 , "dynamics", "compressor");
			SetColor(dynamics_compressor_line, "dynamics", "compressor-line");
			SetColor(dynamics_meter_value	 , "dynamics", "meter-value");
			SetColor(dynamics_db_text		 , "dynamics", "db-text");
			SetColor(dynamics_value_text	 , "dynamics", "value-text");

			SetColor(route_button_idle_background	 , "route-button", "idle", "background");
			SetColor(route_button_idle_triangle		 , "route-button", "idle", "triangle");
			SetColor(route_button_hovering_background, "route-button", "hovering", "background");
			SetColor(route_button_hovering_triangle	 , "route-button", "hovering", "triangle");
			SetColor(route_button_active_background	 , "route-button", "active", "background");
			SetColor(route_button_active_triangle	 , "route-button", "active", "triangle");

			SetColor(channel_idle_name_text		, "channel", "idle", "name-text");
			SetColor(channel_idle_background	, "channel", "idle", "background");
			SetColor(channel_hovering_name_text	, "channel", "hovering", "name-text");
			SetColor(channel_hovering_background, "channel", "hovering", "background");
			SetColor(channel_active_name_text	, "channel", "active", "name-text");
			SetColor(channel_active_background	, "channel", "active", "background");

			SetColor(mute_mono_button_idle_background	 , "mute-mono-button", "idle", "background");
			SetColor(mute_mono_button_idle_text			 , "mute-mono-button", "idle", "text");
			SetColor(mute_mono_button_hovering_background, "mute-mono-button", "hovering", "background");
			SetColor(mute_mono_button_hovering_text		 , "mute-mono-button", "hovering", "text");
			SetColor(mute_mono_button_active_background	 , "mute-mono-button", "active", "background");
			SetColor(mute_mono_button_active_text		 , "mute-mono-button", "active", "text");

			SetColor(effect_title_text     , "effect", "title-text");
			SetColor(effect_title_text_off , "effect", "title-text-off");
			SetColor(effect_title_bar	   , "effect", "title-bar");
			SetColor(effect_background	   , "effect", "background");
			SetColor(effect_minimize_button, "effect", "minimize-button");
			SetColor(effect_divider		   , "effect", "divider");
			SetColor(effect_drag_divider   , "effect", "drag-divider");

			SetColor(divider, "divider");

			SetColor(window_border                          , "window", "border");
			SetColor(window_frame                           , "window", "frame");
			SetColor(window_panel                           , "window", "panel");
			SetColor(window_title_text                      , "window", "title-text");
			SetColor(window_menu_text                       , "window", "menu-text");
			SetColor(window_close_button_icon               , "window", "close-button", "icon");
			SetColor(window_close_button_idle_background    , "window", "close-button", "idle", "background");
			SetColor(window_close_button_hovering_background, "window", "close-button", "hovering", "background");
			SetColor(window_close_button_active_background  , "window", "close-button", "active", "background");
			SetColor(window_button_icon                     , "window", "button", "icon");
			SetColor(window_button_idle_background          , "window", "button", "idle", "background");
			SetColor(window_button_hovering_background      , "window", "button", "hovering", "background");
			SetColor(window_button_active_background        ,"window", "button", "active", "background");

			SetColor(menu_background, "menu", "background");
			SetColor(menu_border    , "menu", "border");
			SetColor(menu_divider   , "menu", "divider");

			SetColor(text_selection, "window", "selection");

			LoadFont();
		}
		catch (...)
		{
			LOG("Error loading theme");
		}
	}

	template<typename ...Args>
	void GetColor(Color& c, const char const* name, const Args&... args)
	{
		try
		{
			Color col = GetColor(args...);
			c = col;
		}
		catch (std::exception)
		{
			LOG("Error loading color : " << name);
		}
	}

	template<typename ...Args>
	Color GetColor(const Args&... args)
	{
		return GetColor(m_Json.at("components"), args...);
	};

	template<typename Arg, typename ...Args>
	Color GetColor(const nlohmann::json& json, const Arg& a, const Args&... args)
	{
		return GetColor(json.at(a), args...);
	};

	template<typename Arg>
	Color GetColor(const nlohmann::json& json, const Arg& arg)
	{
		return GetColor(json.at(arg));
	};

	Color GetColor(const nlohmann::json& json)
	{
		if (json.is_string())
			return m_Variables.at(json.get<std::string>());
		return Color{ json.at(0), json.at(1), json.at(2), json.at(3) };
	}

	std::string& Name() { return m_Name; }

	static inline ThemeT& Get()
	{
		static ThemeT theme;
		if (m_Theme == nullptr)
			return theme;
		return *m_Theme;
	}

	static inline void ReloadThemes()
	{
		m_Theme = nullptr;
		for (auto& i : m_Themes)
			i.second->FreeFonts();

		m_Themes.clear();
		std::filesystem::create_directory("./themes");
		for (auto& p : std::filesystem::directory_iterator("./themes"))
		{
			if (!p.is_regular_file())
				continue;

			std::filesystem::path _path = p;

			if (_path.filename().extension() != ".json")
				continue;

			try
			{
				std::ifstream _if{ p };
				nlohmann::json _json;
				_json = nlohmann::json::parse(_if, nullptr, false);

				if (_json.is_discarded())
					throw(nullptr);

				auto _theme = std::make_unique<ThemeT>(_json);
				auto _name = _theme->Name();
				m_Themes.emplace(_name, std::move(_theme));
				if (m_Theme == nullptr)
					SetTheme(_name);

				_if.close();
			}
			catch (...)
			{
				LOG("Failed to load theme : " << _path.filename());
			}
		}
	}

	static inline void SetTheme(const std::string& t)
	{
		m_Theme = m_Themes[t].get();
	}

	static inline std::unordered_map<std::string, std::unique_ptr<ThemeT>>& Themes()
	{
		return m_Themes;
	}

private:
	nlohmann::json m_Json;
	std::string m_Name;
	std::unordered_map<std::string, Color> m_Variables;
	std::string m_Font{ "" };

	void LoadFont()
	{
		if (m_Font.empty())
		{
			font = Graphics::Fonts::Gidole;
			return;
		}
		std::filesystem::path abspath = "./themes/" + m_Font;
		font = Graphics::LoadFont(abspath.string());
	}

	
	static inline std::unordered_map<std::string, std::unique_ptr<ThemeT>> m_Themes;
	static inline ThemeT* m_Theme = nullptr;
};

struct NOTHING
{
	static void Render(ButtonBase&, CommandCollection&) {}
};

class TitleText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _padding = 20;
		Color _c1 = ThemeT::Get().window_frame;
		Color _c2 = ThemeT::Get().text;
		
		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Font>(ThemeT::Get().font, 24.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class SmallText
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		Color _c2 = ThemeT::Get().text_small;

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<Fill>(_c2);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class RouteButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		double _div = 3;
		Color _c1 = ThemeT::Get().route_button_idle_background;
		Color _c2 = ThemeT::Get().route_button_idle_triangle;

		if (b.Active())
		{
			_c1 = ThemeT::Get().route_button_active_background;
			_c2 = ThemeT::Get().route_button_active_triangle;
			_div = 2.7;
		}
		else if ((b.Hovering()))
		{
			_c1 = ThemeT::Get().route_button_hovering_background;
			_c2 = ThemeT::Get().route_button_hovering_triangle;
		}

		int _w = 3 * b.Height() / _div;
		int _h = 2 * b.Height() / _div;
		d.Command<Fill>(_c1);
		d.Command<Quad>(b.Position(), b.Size());
		d.Command<Fill>(_c2);
		d.Command<Triangle>(Vec4<int>{b.Position() + (b.Size() / 2), _w, _h }, b.Name() == "in" ? -90.0f : 90.0f);
	}
};

class MonoButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		Color _c1 = ThemeT::Get().mute_mono_button_idle_text;
		Color _c2 = ThemeT::Get().mute_mono_button_idle_background;
		
		if (b.Active())
		{
			_c1 = ThemeT::Get().mute_mono_button_active_text;
			_c2 = ThemeT::Get().mute_mono_button_active_background;
		}
		else if ((b.Hovering()))
		{
			_c1 = ThemeT::Get().mute_mono_button_hovering_text;
			_c2 = ThemeT::Get().mute_mono_button_hovering_background;
		}

		int _w = b.Width();
		int _h = b.Height();
		d.Command<Fill>(_c2);
		d.Command<Quad>(Vec4<int>{b.Position(), _w, _h });
		d.Command<Font>(ThemeT::Get().font, 11.0f);
		d.Command<Fill>(_c1);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class MuteButton
{
public:
	static void Render(ButtonBase& b, CommandCollection& d)
	{
		if (b.Disabled())
			return;
		using namespace Graphics;
		Color _c1 = ThemeT::Get().mute_mono_button_idle_text;
		Color _c2 = ThemeT::Get().mute_mono_button_idle_background;

		if (b.Active())
		{
			_c1 = ThemeT::Get().mute_mono_button_active_text;
			_c2 = ThemeT::Get().mute_mono_button_active_background;
		}
		else if ((b.Hovering()))
		{
			_c1 = ThemeT::Get().mute_mono_button_hovering_text;
			_c2 = ThemeT::Get().mute_mono_button_hovering_background;
		}

		int _w = b.Width();
		int _h = b.Height();
		d.Command<Fill>(_c2);
		d.Command<Quad>(Vec4<int>{b.Position(), _w, _h });
		d.Command<Font>(ThemeT::Get().font, 11.0f);
		d.Command<Fill>(_c1);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
	}
};

class VolumeSliderGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		if (b.Vertical())
		{
			int _p = 6;
			int _h = 25;
			int _y = b.NormalizedValue() * b.Height() + b.Y();

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().volume_slider_disabled_handle);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().volume_slider_active_handle);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().volume_slider_hovering_handle);
			else
				d.Command<Fill>(ThemeT::Get().volume_slider_idle_handle);

			int _w = b.Width() - _p * 2;
			int _he = _h - _p * 2;
			d.Command<Triangle>(Vec4<int>{b.X() + _p, _y + _p, 8, _he}, 0.0f);
			d.Command<Triangle>(Vec4<int>{b.X() - _p + b.Width(), _y + _p, 8, _he}, 180.0f);
			d.Command<Quad>(Vec4<int>{b.X() + _p, _y + _p - 1, b.Width() - _p * 2, 3});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(theme->Get(C::TextSmall));
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X() + _p + _w / 2, b.Y()});
		}
		else
		{
			int _p = 6;
			int _w = 25;
			int _x = b.NormalizedValue() * (b.Width()) + b.X();

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().volume_slider_disabled_handle);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().volume_slider_active_handle);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().volume_slider_hovering_handle);
			else
				d.Command<Fill>(ThemeT::Get().volume_slider_idle_handle);

			int _h = b.Height() - _p * 2;
			int _we = _w - _p * 2;
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() + _p, _we, 8}, 90.0f);
			d.Command<Triangle>(Vec4<int>{_x + _p, b.Y() - _p + b.Height(), _we, 8}, 270.0f);
			d.Command<Quad>(Vec4<int>{_x + _p - 1, b.Y() + _p, 3, b.Height() - _p * 2});

			//d.Command<Font>(Fonts::Gidole14, 14.0f);
			//d.Command<Fill>(theme->Get(C::TextSmall));
			//d.Command<TextAlign>(Align::CENTER, Align::TOP);
			//d.Command<Text>(&b.ValueText(), Vec2<int>{b.X(), b.Y() + _p + _h / 2});
		}
	}
};

class PanSliderGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;
		int _w = (b.NormalizedValue() - 0.5) * (b.Width() - 1.0);

		int _h = b.Height() - _p * 2;
		int _we = _w - _p * 2;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().slider_disabled_border);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().slider_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().slider_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().slider_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});
		
		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().slider_disabled_background);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().slider_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().slider_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().slider_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});
		
		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().slider_disabled_value);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().slider_active_value);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().slider_hovering_value);
		else
			d.Command<Fill>(ThemeT::Get().slider_idle_value);

		d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2, b.Y() + 1, _w, b.Height() - 2});

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		//if (b.DisplayValue())
		//{
		//	if (b.Disabled())
		//		d.Command<Fill>(ThemeT::Get().slider_disabled_value_text);
		//	else if (b.Dragging())
		//		d.Command<Fill>(ThemeT::Get().slider_active_value_text);
		//	else if (b.Hovering())
		//		d.Command<Fill>(ThemeT::Get().slider_hovering_value_text);
		//	else
		//		d.Command<Fill>(ThemeT::Get().slider_idle_value_text);

		//	d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		//	d.Command<Text>(&b.ValueText(), b.Position() + (b.Size() / 2));
		//}
		if (b.DisplayName())
		{
			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_name_text);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_name_text);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_name_text);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_name_text);

			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + 5 });
		}
	}
};

class SliderGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Vertical())
		{
			int _p = 6;
			int _h = b.NormalizedValue() * (b.Height() - 2);

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_border);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_border);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_border);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_border);

			d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_background);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_background);

			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_value);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_value);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_value);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_value);

			d.Command<Quad>(Vec4<int>{b.X() + 1, b.Y() + 1, b.Width() - 2, _h});
		}
		else
		{
			int _p = 6;
			int _w = b.NormalizedValue() * (b.Width() - 2);

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_border);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_border);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_border);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_border);

			d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_background);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_background);

			d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_value);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_value);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_value);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_value);

			d.Command<Quad>(Vec4<int>{b.X() + 1, b.Y() + 1, _w, b.Height() - 2});
		}

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		//if (b.DisplayValue())
		//{
		//	if (b.Disabled())
		//		d.Command<Fill>(ThemeT::Get().slider_disabled_value_text);
		//	else if (b.Dragging())
		//		d.Command<Fill>(ThemeT::Get().slider_active_value_text);
		//	else if (b.Hovering())
		//		d.Command<Fill>(ThemeT::Get().slider_hovering_value_text);
		//	else
		//		d.Command<Fill>(ThemeT::Get().slider_idle_value_text);

		//	d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		//	d.Command<Text>(&b.ValueText(), Vec2<int>{ b.X() + (b.Width() / 2), b.Y() + b.Height() / 2});
		//}
		if (b.DisplayName())
		{
			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().slider_disabled_name_text);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().slider_active_name_text);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().slider_hovering_name_text);
			else
				d.Command<Fill>(ThemeT::Get().slider_idle_name_text);

			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + 5 });
		}
	}

};

class ToggleButtonGraphics
{
public:

	static void Render(ButtonBase& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().toggle_button_disabled_border);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().toggle_button_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().toggle_button_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().toggle_button_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().toggle_button_disabled_background);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().toggle_button_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().toggle_button_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().toggle_button_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().toggle_button_disabled_text);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().toggle_button_active_text);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().toggle_button_hovering_text);
		else
			d.Command<Fill>(ThemeT::Get().toggle_button_idle_text);

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() / 2 });
	}
};

class RadioButtonGraphics
{
public:

	static void Render(Button<RadioButtonGraphics, ButtonType::List>& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().radio_button_disabled_border);
		else if (b.Selected())
			d.Command<Fill>(ThemeT::Get().radio_button_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().radio_button_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().radio_button_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().radio_button_disabled_background);
		else if (b.Selected())
			d.Command<Fill>(ThemeT::Get().radio_button_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().radio_button_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().radio_button_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().radio_button_disabled_text);
		else if (b.Selected())
			d.Command<Fill>(ThemeT::Get().radio_button_active_text);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().radio_button_hovering_text);
		else
			d.Command<Fill>(ThemeT::Get().radio_button_idle_text);

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() / 2 });
	}

};

class NormalButtonGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().button_disabled_border);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().button_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().button_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().button_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().button_disabled_background);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().button_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().button_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().button_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().button_disabled_text);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().button_active_text);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().button_hovering_text);
		else
			d.Command<Fill>(ThemeT::Get().button_idle_text);

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() / 2 });
	}

};

class DropdownButton
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_border);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_background);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_text);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_text);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_text);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_text);

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<TextAlign>(Align::CENTER, Align::CENTER);
		d.Command<Text>(&b.Name(), Vec2<int>{ b.X() + b.Width() / 2, b.Y() + b.Height() / 2 });
	}

};

class DropdownButton2
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_border);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_border);

		d.Command<Quad>(Vec4<int>{b.Position(), b.Size()});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_background);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_background);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_background);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_background);

		d.Command<Quad>(Vec4<int>{b.Position() + 1, b.Size() - 2});

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().dropdown_button_disabled_text);
		else if (b.Active())
			d.Command<Fill>(ThemeT::Get().dropdown_button_active_text);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().dropdown_button_hovering_text);
		else
			d.Command<Fill>(ThemeT::Get().dropdown_button_idle_text);

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		d.Command<TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Text>(&b.Name(), Vec2<int>{ b.X() + 4, b.Y() + b.Height() / 2 });
		
		int _w = 8;
		int _padding = 4;
		d.Command<Triangle>(Vec4<int>{b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w, _w / 2}, -90.0f);
	}
};

class KnobGraphics
{
public:

	template<typename T>
	static void Render(T& b, CommandCollection& d)
	{
		using namespace Graphics;
		int _p = 6;

		bool _double = b.Range().start < 0 && b.Range().end > 0 && b.Range().start == - b.Range().end;

		double _v = 1.0 - b.NormalizedValue();
		double _a = _v * M_PI * 1.5 + M_PI * 0.25 - M_PI / 2.0;

		int _w = b.NormalizedValue() * (b.Width() * 0.5 - 1);

		int _h = b.Height() - _p * 2;
		int _we = _w - _p * 2;

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().knob_disabled_border);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().knob_active_border);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().knob_hovering_border);
		else
			d.Command<Fill>(ThemeT::Get().knob_idle_border);

		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size()}, Vec2<double>{ M_PI * 1.75 - M_PI / 2.0, M_PI * 0.25 - M_PI / 2.0 });

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().knob_disabled_value);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().knob_active_value);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().knob_hovering_value);
		else
			d.Command<Fill>(ThemeT::Get().knob_idle_value);

		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size()}, Vec2<double>{_double ? _a > M_PI / 2 ? _a : M_PI / 2 : -M_PI * 0.75f, _double ? _a > M_PI / 2 ? M_PI / 2 : _a : _a});
		
		d.Command<Fill>(ThemeT::Get().effect_background);
		d.Command<Graphics::Ellipse>(Vec4<int>{b.Position() + b.Size() / 2, b.Size() - 5}, Vec2<double>{ M_PI * 1.75 - M_PI / 2.0, M_PI * 0.25 - M_PI / 2.0 });

		float _x = std::cos(_a) * (b.Width() / 2.0); 
		float _y = std::sin(_a) * (b.Height() / 2.0);
		d.Command<Graphics::Line>(Vec4<float>{ b.X() + b.Width() / 2.0f, b.Y() + b.Height() / 2.0f, b.X() + b.Width() / 2.0f + _x, b.Y() + b.Height() / 2.0f + _y}, 6.0f);

		if (b.Disabled())
			d.Command<Fill>(ThemeT::Get().knob_disabled_handle);
		else if (b.Dragging())
			d.Command<Fill>(ThemeT::Get().knob_active_handle);
		else if (b.Hovering())
			d.Command<Fill>(ThemeT::Get().knob_hovering_handle);
		else
			d.Command<Fill>(ThemeT::Get().knob_idle_handle);
		d.Command<Graphics::Line>(Vec4<float>{ b.X() + b.Width() / 2.0f, b.Y() + b.Height() / 2.0f, b.X() + b.Width() / 2.0f + _x, b.Y() + b.Height() / 2.0f + _y}, 3.0f);

		if (_double)
		{
			if (b.Value() == 0)
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().knob_disabled_border);
				else if (b.Dragging())
					d.Command<Fill>(ThemeT::Get().knob_active_border);
				else if (b.Hovering())
					d.Command<Fill>(ThemeT::Get().knob_hovering_border);
				else
					d.Command<Fill>(ThemeT::Get().knob_idle_border);
			}
			else
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().knob_disabled_value);
				else if (b.Dragging())
					d.Command<Fill>(ThemeT::Get().knob_active_value);
				else if (b.Hovering())
					d.Command<Fill>(ThemeT::Get().knob_hovering_value);
				else
					d.Command<Fill>(ThemeT::Get().knob_idle_value);
			}
			d.Command<Graphics::Triangle>(Vec4<int>{ b.X() + b.Width() / 2, b.Y() + b.Height() + 2, 7, 4 }, -90.0f);
		}

		d.Command<Font>(ThemeT::Get().font, 14.0f);
		//if (b.DisplayValue())
		//{
		//	if (b.Disabled())
		//		d.Command<Fill>(ThemeT::Get().knob_disabled_value_text);
		//	else if (b.Dragging())
		//		d.Command<Fill>(ThemeT::Get().knob_active_value_text);
		//	else if (b.Hovering())
		//		d.Command<Fill>(ThemeT::Get().knob_hovering_value_text);
		//	else
		//		d.Command<Fill>(ThemeT::Get().knob_idle_value_text);

		//	d.Command<TextAlign>(Align::CENTER, Align::TOP);
		//	d.Command<Text>(&b.ValueText(), Vec2<int>{ b.X() + (b.Width() / 2), b.Y() + 3});
		//}		
		if (b.DisplayName())
		{
			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().knob_disabled_name_text);
			else if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().knob_active_name_text);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().knob_hovering_name_text);
			else
				d.Command<Fill>(ThemeT::Get().knob_idle_name_text);

			d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
			d.Command<Text>(&b.Name(), b.Position() + Vec2<int>{ b.Width() / 2, b.Height() + (_double ? 8 : 5) });
		}
	}
};

namespace SoundMixrGraphics
{
	using namespace Graphics;
	
	template<Align Hori = Align::CENTER, Align Vert = Align::CENTER>
	struct Normal
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			int _padding = 20;
			
			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().menu_button_disabled_background);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().menu_button_active_background);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().menu_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().menu_button_idle_background);

			d.Command<Quad>(b.Position(), b.Size());

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().menu_button_disabled_text);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().menu_button_active_text);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().menu_button_hovering_text);
			else
				d.Command<Fill>(ThemeT::Get().menu_button_idle_text);

			d.Command<Font>(ThemeT::Get().font, 14.0f);
			d.Command<TextAlign>(Hori, Vert);
			if (Hori == Align::CENTER)
				d.Command<Text>(&b.Name(), b.X() + b.Width() / 2, b.Y() + b.Height() / 2);
			else if (Hori == Align::RIGHT)
				d.Command<Text>(&b.Name(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			else if (Hori == Align::LEFT)
				d.Command<Text>(&b.Name(), b.X() + 6, b.Y() + b.Height() / 2);
		}
	};

	struct Menu
	{
		template<typename This = ButtonBase>
		static void Render(This& b, CommandCollection& d)
		{
			int _padding = 20;

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().menu_button_disabled_background);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().menu_button_active_background);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().menu_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().menu_button_idle_background);

			d.Command<Quad>(b.Position(), b.Size());
			
			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().menu_button_disabled_text);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().menu_button_active_text);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().menu_button_hovering_text);
			else
				d.Command<Fill>(ThemeT::Get().menu_button_idle_text);

			d.Command<Font>(ThemeT::Get().font, 14.0f);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);

			if (b.KeyCombo() != Key::NONE)
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().menu_button_disabled_text_small);
				else if (b.Active())
					d.Command<Fill>(ThemeT::Get().menu_button_active_text_small);
				else if (b.Hovering() || b.Focused())
					d.Command<Fill>(ThemeT::Get().menu_button_hovering_text_small);
				else
					d.Command<Fill>(ThemeT::Get().menu_button_idle_text_small);
				
				d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			}
		};

		template<>
		static void Render<Button<Menu, ButtonType::List>>(Button<Menu, ButtonType::List>& b, CommandCollection& d)
		{
			int _padding = 20;

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_background);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_background);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_background);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_background);

			d.Command<Quad>(b.Position(), b.Size());

			if (b.Selected())
			{
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_square);
				int _p = 2;
				d.Command<Quad>(b.Position() + Vec2<int>{ _p, _p }, Vec2<int>{ b.Size().y - _p * 2, b.Size().y - _p * 2 });
			}

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text);

			d.Command<Font>(ThemeT::Get().font, 14.0f);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);

			if (b.KeyCombo() != Key::NONE)
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text_small);
				else if (b.Hovering() || b.Focused())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text_small);
				else if (b.Active())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text_small);
				else
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text_small);

				d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			}
		};

		template<>
		static void Render<Button<Menu, ButtonType::Toggle>>(Button<Menu, ButtonType::Toggle>& b, CommandCollection& d)
		{
			int _padding = 20;

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_background);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_background);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_background);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_background);

			d.Command<Quad>(b.Position(), b.Size());

			if (b.Active())
			{
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_square);
				int _p = 2;
				d.Command<Quad>(b.Position() + Vec2<int>{ _p, _p }, Vec2<int>{ b.Size().y - _p * 2, b.Size().y - _p * 2 });
			}

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text);

			d.Command<Font>(ThemeT::Get().font, 14.0f);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);

			if (b.KeyCombo() != Key::NONE)
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text_small);
				else if (b.Hovering() || b.Focused())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text_small);
				else if (b.Active())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text_small);
				else
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text_small);

				d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			}
		};

		template<>
		static void Render<Button<Menu, ButtonType::Hover>>(Button<Menu, ButtonType::Hover>& b, CommandCollection& d)
		{
			int _padding = 20;

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_background);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_background);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_background);

			d.Command<Quad>(b.Position(), b.Size());

			if (b.Disabled())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text);
			else if (b.Active())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text);
			else if (b.Hovering() || b.Focused())
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text);
			else
				d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text);

			d.Command<Font>(ThemeT::Get().font, 14.0f);
			d.Command<TextAlign>(Align::LEFT, Align::CENTER);
			d.Command<Text>(&b.Name(), _padding + b.X() + 6, b.Y() + b.Height() / 2);

			if (b.KeyCombo() != Key::NONE)
			{
				if (b.Disabled())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_disabled_text_small);
				else if (b.Active())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_active_text_small);
				else if (b.Hovering() || b.Focused())
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_hovering_text_small);
				else
					d.Command<Fill>(ThemeT::Get().toggle_menu_button_idle_text_small);

				d.Command<TextAlign>(Align::RIGHT, Align::CENTER);
				d.Command<Text>(&b.KeyCombo().ToString(), b.X() + b.Width() - 6, b.Y() + b.Height() / 2);
			}
		};
	};

	struct SubMenu
	{
		template<typename This = ButtonBase>
		static void Render(This& b, CommandCollection& d)
		{
			Menu::Render(b, d);
			int _w = 8;
			int _padding = 4;
			d.Command<Triangle>(b.X() + b.Width() - _w / 2 - _padding, b.Y() + b.Height() / 2, _w / 2, _w, 0.0f);
		}
	};

	struct TitleMenu
	{ 
		static void TitleMenu::Render(ButtonBase& b, CommandCollection& d)
		 {
			 Color _c1 = b.Active() ? ThemeT::Get().menu_background : b.Hovering() ? ThemeT::Get().menu_button_hovering_background : Color{ 0, 0, 0, 0 };
			 Color _c2 = b.Active() ? ThemeT::Get().window_menu_text : b.Hovering() ? ThemeT::Get().menu_button_hovering_text : ThemeT::Get().window_menu_text;

			 int _padding = 20;
			 if (b.Active())
			 {
				 d.Command<Fill>(ThemeT::Get().menu_border);
				 d.Command<Quad>(b.X() - 1, b.Y(), b.Width() + 2, b.Height());
			 }
			 d.Command<Fill>(_c1);
			 d.Command<Quad>(b.X(), b.Y(), b.Width(), b.Height() - 1);
			 d.Command<Fill>(_c2);
			 d.Command<Font>(ThemeT::Get().font, 14.0f);
			 d.Command<TextAlign>(Align::CENTER, Align::CENTER);
			 d.Command<Text>(&b.Name(), Vec2<int>{b.X() + b.Width() / 2, b.Y() + b.Height() / 2});
		 }
	}; 

	struct Horizontal
	{
		static void Render(MenuBase& b, CommandCollection& d) 
		{
			auto& _menuItems = b.Components();

			int _x = b.X();
			int _y = b.Y();
			int _ix = b.X();
			int _mh = 0;

			for (auto& _item : _menuItems)
			{
				if (!_item->Visible())
					continue;

				_item->Position({ _x, _y });
				_x += _item->Width();
				if (_item->Height() > _mh)
					_mh = _item->Height();
			}

			b.Size({ _x - _ix, _mh });
		}
	};

	struct Vertical
	{
		static void Render(MenuBase& b, CommandCollection& d) 
		{
			auto& _menuItems = b.Components();

			int _padding = 2;
			int _x = b.X();
			int _y = b.Y() + b.Height();
			int _iy = _y;
			int _mw = 0;

			d.Command<Fill>(ThemeT::Get().menu_border);
			d.Command<Quad>(b.X() - 1, b.Y() - 1, b.Width() + 2, b.Height() + 2);

			d.Command<Fill>(ThemeT::Get().menu_background);
			d.Command<Quad>(b.X(), b.Y(), b.Width(), b.Height());

			for (auto& _item : _menuItems)
			{
				if (!_item->Visible())
					continue;

				_y -= _item->Height();
				_item->Position({ _x + _padding, _y - _padding });
				if (_item->Width() > _mw)
					_mw = _item->Width();
			}

			b.Size({ _mw + _padding * 2, -_y + _iy + _padding * 2 });
		}
	};

	struct ScrollbarNormal
	{
		template<typename ScrollbarType>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType>& b, CommandCollection& d)
		{}

		template<>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType::Horizontal>& b, CommandCollection& d)
		{
			int _p = 6;
			int _w = std::max((b.VisibleRange() - b.Range().start) / (float)(b.Range().end - b.Range().start) * b.Width(), (float)b.MinBarSize());
			int _x = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start - b.VisibleRange()) * (b.Width() - _w) + b.X();

			if (b.Dragging() || b.Hovering())
				_p = 4;

			if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().scrollbar_active_bar);
			else if (b.Hovering() && b.Mouse() >= _x && b.Mouse() <= _x + _w)
				d.Command<Fill>(ThemeT::Get().scrollbar_hovering_bar);
			else
				d.Command<Fill>(ThemeT::Get().scrollbar_idle_bar);

			d.Command<Quad>(b.Position().x, b.Position().y + b.Size().height / 2 - 1, b.Size().width, 2);

			if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().scrollbar_active_handle);
			else if (b.Hovering() && b.Mouse() >= _x && b.Mouse() <= _x + _w)
				d.Command<Fill>(ThemeT::Get().scrollbar_hovering_handle);
			else
				d.Command<Fill>(ThemeT::Get().scrollbar_idle_handle);

			d.Command<Quad>(_x + _p, b.Position().y + _p, _w - _p * 2, b.Size().height - _p * 2);
		}

		template<>
		static void Render(Scrollbar<ScrollbarNormal, ScrollbarType::Vertical>& b, CommandCollection& d)
		{
			int _p = 6;
			int _h = std::max((b.VisibleRange() - b.Range().start) / (float)(b.Range().end - b.Range().start) * b.Height(), (float)b.MinBarSize());
			int _y = (b.Value() - b.Range().start) / (float)(b.Range().end - b.Range().start - b.VisibleRange()) * -(b.Height() - _h) + b.Y() + b.Height() - _h;

			if (b.Dragging() || b.Hovering())
				_p = 4;

			if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().scrollbar_active_bar);
			else if (b.Hovering() && b.Mouse() >= _y && b.Mouse() <= _y + _h)
				d.Command<Fill>(ThemeT::Get().scrollbar_hovering_bar);
			else
				d.Command<Fill>(ThemeT::Get().scrollbar_idle_bar);

			d.Command<Quad>(b.Position().x + b.Size().width / 2 - 1, b.Position().y, 2, b.Size().height);

			if (b.Dragging())
				d.Command<Fill>(ThemeT::Get().scrollbar_active_handle);
			else if (b.Hovering() && b.Mouse() >= _y && b.Mouse() <= _y + _h)
				d.Command<Fill>(ThemeT::Get().scrollbar_hovering_handle);
			else
				d.Command<Fill>(ThemeT::Get().scrollbar_idle_handle);

			d.Command<Quad>(b.Position().x + _p, _y + _p, b.Size().width - _p * 2, _h - _p * 2);
		}
	};

	struct CloseButton
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			if (b.Active())
				d.Command<Fill>(ThemeT::Get().window_close_button_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().window_close_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().window_close_button_idle_background);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Fill>(ThemeT::Get().window_close_button_icon);
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 7, b.Y() + b.Height() / 2, 14, 1}, 45.0f);
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 7, b.Y() + b.Height() / 2, 14, 1}, -45.0f);

		}
	};

	struct Maxi1Button
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			if (b.Active())
				d.Command<Fill>(ThemeT::Get().window_button_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().window_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().window_button_idle_background);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Fill>(ThemeT::Get().window_button_icon);
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 + 2, 8, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 + 2, b.Y() + b.Height() / 2 - 5, 1, 8});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 - 5, 8, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 - 5, 1, 8});

			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 3, b.Y() + b.Height() / 2 + 3, 1, 2});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 3, b.Y() + b.Height() / 2 + 4, 8, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 + 3, b.Y() + b.Height() / 2 - 3, 2, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 + 4, b.Y() + b.Height() / 2 - 3, 1, 8});

		}
	};

	struct Maxi2Button
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			if (b.Active())
				d.Command<Fill>(ThemeT::Get().window_button_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().window_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().window_button_idle_background);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Fill>(ThemeT::Get().window_button_icon);
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 + 4, 10, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 + 4, b.Y() + b.Height() / 2 - 5, 1, 10});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 - 5, 10, 1});
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2 - 5, 1, 10});
		}
	};

	struct MiniButton
	{
		static void Render(ButtonBase& b, CommandCollection& d)
		{
			if (b.Active())
				d.Command<Fill>(ThemeT::Get().window_button_active_background);
			else if (b.Hovering())
				d.Command<Fill>(ThemeT::Get().window_button_hovering_background);
			else
				d.Command<Fill>(ThemeT::Get().window_button_idle_background);
			d.Command<Quad>(b.Position(), b.Size());
			d.Command<Fill>(ThemeT::Get().window_button_icon);
			d.Command<Quad>(Vec4<int>{b.X() + b.Width() / 2 - 5, b.Y() + b.Height() / 2, 10, 1});

		}
	};
}

class SMXRScrollPanel : public ScrollPanel
{
public:

	Vec2<bool> ScrollbarNotNecessary()
	{
		return { m_ScrollbarX->NotNecessary(), m_ScrollbarY->NotNecessary() };
	}

	// Overwrite these methods from ScrollPanel for custom ScrollbarGraphics	
	::Panel& Panel() const { return *m_Panel; }
	template<typename T, typename ...Args>
	T& Panel(Args&&... args)
	{
		if (m_Panel != nullptr)
			return dynamic_cast<T&>(*m_Panel);

		auto& _t = Emplace<T>(std::forward<Args>(args)...);
		m_Panel = &_t;
		m_ScrollbarX = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Horizontal>>();
		m_ScrollbarY = &Emplace<Scrollbar<SoundMixrGraphics::ScrollbarNormal, ScrollbarType::Vertical>>();
		return _t;
	}
};

class MenuDivider : public Component
{
public:
	MenuDivider(int size, int stroke, int paddingx, int paddingy)
		: m_Size(size), m_PaddingX(paddingx), m_PaddingY(paddingy), m_Stroke(stroke), Component()
	{
		Size({ size, stroke + paddingy * 2 });
	}

	void Render(CommandCollection& d) override 
	{
		using namespace Graphics;
		d.Command<Fill>(ThemeT::Get().menu_divider);
		d.Command<Quad>(X() + m_PaddingX, Y() + m_PaddingY, Width() - m_PaddingX * 2, m_Stroke);
	};

	bool WithinBounds(const Vec2<int>&) const override { return false; }

private:
	int m_Size,
		m_PaddingX,
		m_PaddingY,
		m_Stroke;
};

class VerticalMenuDivider : public Component
{
public:
	VerticalMenuDivider(int size, int stroke, int paddingx, int paddingy)
		: m_Size(size), m_PaddingX(paddingx), m_PaddingY(paddingy), m_Stroke(stroke), Component()
	{
		Size({ stroke + paddingx * 2, size });
	}

	void Render(CommandCollection& d) override
	{
		using namespace Graphics;
		d.Command<Fill>(ThemeT::Get().divider);
		d.Command<Quad>(X() + m_PaddingX, Y() + m_PaddingY, m_Stroke, Height() - m_PaddingY * 2);
	};

	bool WithinBounds(const Vec2<int>&) const override { return false; }

private:
	int m_Size,
		m_PaddingX,
		m_PaddingY,
		m_Stroke;
};

class SMXRTextBox : public TextBox
{

public:

	SMXRTextBox(bool autoresize = true)
		: m_AutoResize(autoresize)
	{}

	void Update(const Vec4<int>& v) override
	{
		if (FontSize() == 48) Font(ThemeT::Get().font, 48);
		else if (FontSize() == 24) Font(ThemeT::Get().font, 24);
		else if (FontSize() == 12) Font(ThemeT::Get().font, 12);
		else if (FontSize() == 14) Font(ThemeT::Get().font, 14);
		else if (FontSize() == 16) Font(ThemeT::Get().font, 16);
		else if (FontSize() == 8) Font(ThemeT::Get().font, 8);

		SelectColor(ThemeT::Get().text_selection);
		TextBox::Update(v);
	}

	void Font(int f, float size) 
	{
		int fold = m_Displayer.Font();
		if (f != fold)
		{
			m_Displayer.Font(f, size);

			if (m_AutoResize)
				Width(Graphics::StringWidth(m_Displayer.Content(), f, size) + 6);
			
			m_Displayer.RecalculateLines();
		}
	};
private:
	bool m_AutoResize = true;
};

class SMXRTextComponent : public SMXRTextBox
{

public:

	SMXRTextComponent(const std::string& t, float fsize = 14, bool autoresize = true)
		: SMXRTextBox(autoresize)
	{
		Font(0, fsize);

		Content(t);
		AlignLines(Align::LEFT);
		TextColor({ 255, 255, 255, 255 });
		Editable(false);
		Width(Graphics::StringWidth(t, Font(), fsize) + 6);
		Height(fsize + 4);
		Padding(2);
		Background({ 0, 0, 0, 0 });
	}

	/**
	 * Set the content.
	 * @param c content
	 */
	void Content(const std::string& c)
	{
		//Width(Graphics::StringWidth(c, Font(), FontSize()) + 4);
		//Height(FontSize() + 4);
		TextBox::Content(c);
	}

	void Update(const Vec4<int>& v) override
	{
		TextColor(ThemeT::Get().text);
		SMXRTextBox::Update(v);
	}

	/**
	 * Get the content.
	 * @return content
	 */
	auto Content() -> std::string& { return TextBox::Content(); }

	/**
	 * Set the font and font size.
	 * @param f font
	 * @param size font size
	 */
	void Font(int f, float fsize)
	{
		if (fsize == 48) TextBox::Font(ThemeT::Get().font, 48);
		else if (fsize == 24) TextBox::Font(ThemeT::Get().font, 24);
		else if (fsize == 12) TextBox::Font(ThemeT::Get().font, 12);
		else if (fsize == 14) TextBox::Font(ThemeT::Get().font, 14);
		else if (fsize == 16) TextBox::Font(ThemeT::Get().font, 16);
		else if (fsize == 8) TextBox::Font(ThemeT::Get().font, 8);

		Width(Graphics::StringWidth(Content(), TextBox::FontSize(), fsize) + 6);
		Height(fsize + 4);
	}

	/**
	 * Get the font.
	 * @return font
	 */
	int Font() const { return TextBox::Font(); }
};