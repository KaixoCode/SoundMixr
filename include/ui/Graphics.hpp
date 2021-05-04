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
		text,
		text_small,
		textOff;

	Color // Button
		button_disabled_background,
		button_disabled_border,
		button_disabled_text,
		button_active_background,
		button_active_border,
		button_active_text,
		button_hovering_background,
		button_hovering_border,
		button_hovering_text,
		button_idle_background,
		button_idle_border,
		button_idle_text;


	Color // Toggle button
		toggle_button_disabled_background,
		toggle_button_disabled_border,
		toggle_button_disabled_text,
		toggle_button_active_background,
		toggle_button_active_border,
		toggle_button_active_text,
		toggle_button_hovering_background,
		toggle_button_hovering_border,
		toggle_button_hovering_text,
		toggle_button_idle_background,
		toggle_button_idle_border,
		toggle_button_idle_text;


	Color // DropDown button
		dropdown_button_disabled_background,
		dropdown_button_disabled_border,
		dropdown_button_disabled_text,
		dropdown_button_active_background,
		dropdown_button_active_border,
		dropdown_button_active_text,
		dropdown_button_hovering_background,
		dropdown_button_hovering_border,
		dropdown_button_hovering_text,
		dropdown_button_idle_background,
		dropdown_button_idle_border,
		dropdown_button_idle_text;

	Color // Radio button
		radio_button_disabled_background,
		radio_button_disabled_border,
		radio_button_disabled_text,
		radio_button_active_background,
		radio_button_active_border,
		radio_button_active_text,
		radio_button_hovering_background,
		radio_button_hovering_border,
		radio_button_hovering_text,
		radio_button_idle_background,
		radio_button_idle_border,
		radio_button_idle_text;

	Color // Menu button
		menu_button_disabled_background,
		menu_button_disabled_text,
		menu_button_disabled_text_small,
		menu_button_active_background,
		menu_button_active_text,
		menu_button_active_text_small,
		menu_button_hovering_background,
		menu_button_hovering_text,
		menu_button_hovering_text_small,
		menu_button_idle_background,
		menu_button_idle_text,
		menu_button_idle_text_small;

	Color // Toggle menu button
		toggle_menu_button_disabled_background,
		toggle_menu_button_disabled_text,
		toggle_menu_button_disabled_text_small,
		toggle_menu_button_active_background,
		toggle_menu_button_active_text,
		toggle_menu_button_active_text_small,
		toggle_menu_button_active_square,
		toggle_menu_button_hovering_background,
		toggle_menu_button_hovering_text,
		toggle_menu_button_hovering_text_small,
		toggle_menu_button_idle_background,
		toggle_menu_button_idle_text,
		toggle_menu_button_idle_text_small;

	Color // Scrollbar
		scrollbar_idle_handle,
		scrollbar_idle_bar,
		scrollbar_hovering_handle,
		scrollbar_hovering_bar,
		scrollbar_active_handle,
		scrollbar_active_bar;

	Color // Slider
		slider_disabled_background,
		slider_disabled_border,
		slider_disabled_value,
		slider_disabled_value_text,
		slider_disabled_name_text,
		slider_idle_background,
		slider_idle_border,
		slider_idle_value,
		slider_idle_value_text,
		slider_idle_name_text,
		slider_hovering_background,
		slider_hovering_border,
		slider_hovering_value,
		slider_hovering_value_text,
		slider_hovering_name_text,
		slider_active_background,
		slider_active_border,
		slider_active_value,
		slider_active_value_text,
		slider_active_name_text;

	Color // Knob
		knob_disabled_handle,
		knob_disabled_border,
		knob_disabled_value,
		knob_disabled_name_text,
		knob_disabled_value_text,
		knob_idle_handle,
		knob_idle_border,
		knob_idle_value,
		knob_idle_name_text,
		knob_idle_value_text,
		knob_hovering_handle,
		knob_hovering_border,
		knob_hovering_value,
		knob_hovering_name_text,
		knob_hovering_value_text,
		knob_active_handle,
		knob_active_border,
		knob_active_value,
		knob_active_name_text,
		knob_active_value_text;

	Color // Volume slider
		volume_slider_disabled_handle,
		volume_slider_disabled_db_text,
		volume_slider_disabled_value_text,
		volume_slider_disabled_line,
		volume_slider_disabled_line_highlight,
		volume_slider_disabled_meter,
		volume_slider_disabled_meter_value,
		volume_slider_disabled_meter_value_c1,
		volume_slider_disabled_meter_value_c2,
		volume_slider_disabled_meter_value_c3,
		volume_slider_idle_handle,
		volume_slider_idle_db_text,
		volume_slider_idle_value_text,
		volume_slider_idle_line,
		volume_slider_idle_line_highlight,
		volume_slider_idle_meter,
		volume_slider_idle_meter_value,
		volume_slider_idle_meter_value_c1,
		volume_slider_idle_meter_value_c2,
		volume_slider_idle_meter_value_c3,
		volume_slider_hovering_handle,
		volume_slider_hovering_db_text,
		volume_slider_hovering_value_text,
		volume_slider_hovering_line,
		volume_slider_hovering_line_highlight,
		volume_slider_hovering_meter,
		volume_slider_hovering_meter_value,
		volume_slider_hovering_meter_value_c1,
		volume_slider_hovering_meter_value_c2,
		volume_slider_hovering_meter_value_c3,
		volume_slider_active_handle,
		volume_slider_active_db_text,
		volume_slider_active_value_text,
		volume_slider_active_line,
		volume_slider_active_line_highlight,
		volume_slider_active_meter,
		volume_slider_active_meter_value,
		volume_slider_active_meter_value_c1,
		volume_slider_active_meter_value_c2,
		volume_slider_active_meter_value_c3;

	Color // XY-Controller
		effect_graph_background,
		effect_graph_graph_lines,
		effect_graph_graph_lines_highlight,
		effect_graph_disabled_circle,
		effect_graph_disabled_line,
		effect_graph_idle_circle,
		effect_graph_idle_line,
		effect_graph_hovering_circle,
		effect_graph_hovering_line,
		effect_graph_active_circle,
		effect_graph_active_line;

	Color // Dynamics
		dynamics_background,
		dynamics_border,
		dynamics_line,
		dynamics_line_highlight,
		dynamics_compressor,
		dynamics_compressor_line,
		dynamics_meter_value,
		dynamics_db_text,
		dynamics_value_text;

	Color // Route button
		route_button_idle_background,
		route_button_idle_triangle,
		route_button_hovering_background,
		route_button_hovering_triangle,
		route_button_active_background,
		route_button_active_triangle;

	Color // Channel
		channel_idle_name_text,
		channel_idle_background,
		channel_hovering_name_text,
		channel_hovering_background,
		channel_active_name_text,
		channel_active_background;

	Color // mute mono button
		mute_mono_button_idle_background,
		mute_mono_button_idle_text,
		mute_mono_button_hovering_background,
		mute_mono_button_hovering_text,
		mute_mono_button_active_background,
		mute_mono_button_active_text;

	Color // Effect
		effect_title_text,
		effect_title_text_off,
		effect_title_bar,
		effect_background,
		effect_minimize_button,
		effect_divider,
		effect_drag_divider;

	Color // Divider
		divider;

	Color // Window
		window_border,
		window_frame,
		window_panel,
		window_title_text,
		window_menu_text,
		window_close_button_icon,
		window_close_button_idle_background,
		window_close_button_hovering_background,
		window_close_button_active_background,
		window_button_icon,
		window_button_idle_background,
		window_button_hovering_background,
		window_button_active_background;

	Color // Menu
		menu_background,
		menu_border,
		menu_divider;

	ThemeT()
	{}

	ThemeT(nlohmann::json json)
		: m_Json(json)
	{
		LoadTheme();
	}

	void LoadTheme()
	{
		try
		{
			m_Name = m_Json.at("name").get<std::string>();
			for (auto& [key, v] : m_Json.at("variables").items())
				m_Variables.emplace(key, Color{ v.at(0), v.at(1), v.at(2), v.at(3) });

			text       = GetColor("text");
			text_small = GetColor("textsmall");
			textOff    = GetColor("textoff");

			button_disabled_background	= GetColor("button", "disabled", "background");
			button_disabled_border		= GetColor("button", "disabled", "border");
			button_disabled_text		= GetColor("button", "disabled", "text");
			button_active_background	= GetColor("button", "active", "background");
			button_active_border		= GetColor("button", "active", "border");
			button_active_text			= GetColor("button", "active", "text");
			button_hovering_background	= GetColor("button", "hovering", "background");
			button_hovering_border		= GetColor("button", "hovering", "border");
			button_hovering_text		= GetColor("button", "hovering", "text");
			button_idle_background		= GetColor("button", "idle", "background");
			button_idle_border			= GetColor("button", "idle", "border");
			button_idle_text			= GetColor("button", "idle", "text");

			toggle_button_disabled_background	 = GetColor("toggle-button", "disabled", "background");
			toggle_button_disabled_border		 = GetColor("toggle-button", "disabled", "border");
			toggle_button_disabled_text			 = GetColor("toggle-button", "disabled", "text");
			toggle_button_active_background		 = GetColor("toggle-button", "active", "background");
			toggle_button_active_border			 = GetColor("toggle-button", "active", "border");
			toggle_button_active_text			 = GetColor("toggle-button", "active", "text");
			toggle_button_hovering_background	 = GetColor("toggle-button", "hovering", "background");
			toggle_button_hovering_border		 = GetColor("toggle-button", "hovering", "border");
			toggle_button_hovering_text			 = GetColor("toggle-button", "hovering", "text");
			toggle_button_idle_background		 = GetColor("toggle-button", "idle", "background");
			toggle_button_idle_border			 = GetColor("toggle-button", "idle", "border");
			toggle_button_idle_text				 = GetColor("toggle-button", "idle", "text");

			dropdown_button_disabled_background	 = GetColor("dropdown-button", "disabled", "background");
			dropdown_button_disabled_border		 = GetColor("dropdown-button", "disabled", "border");
			dropdown_button_disabled_text		 = GetColor("dropdown-button", "disabled", "text");
			dropdown_button_active_background	 = GetColor("dropdown-button", "active", "background");
			dropdown_button_active_border		 = GetColor("dropdown-button", "active", "border");
			dropdown_button_active_text			 = GetColor("dropdown-button", "active", "text");
			dropdown_button_hovering_background	 = GetColor("dropdown-button", "hovering", "background");
			dropdown_button_hovering_border		 = GetColor("dropdown-button", "hovering", "border");
			dropdown_button_hovering_text		 = GetColor("dropdown-button", "hovering", "text");
			dropdown_button_idle_background		 = GetColor("dropdown-button", "idle", "background");
			dropdown_button_idle_border			 = GetColor("dropdown-button", "idle", "border");
			dropdown_button_idle_text			 = GetColor("dropdown-button", "idle", "text");

			radio_button_disabled_background	 = GetColor("radio-button", "disabled", "background");
			radio_button_disabled_border		 = GetColor("radio-button", "disabled", "border");
			radio_button_disabled_text			 = GetColor("radio-button", "disabled", "text");
			radio_button_active_background		 = GetColor("radio-button", "active", "background");
			radio_button_active_border			 = GetColor("radio-button", "active", "border");
			radio_button_active_text			 = GetColor("radio-button", "active", "text");
			radio_button_hovering_background	 = GetColor("radio-button", "hovering", "background");
			radio_button_hovering_border		 = GetColor("radio-button", "hovering", "border");
			radio_button_hovering_text			 = GetColor("radio-button", "hovering", "text");
			radio_button_idle_background		 = GetColor("radio-button", "idle", "background");
			radio_button_idle_border			 = GetColor("radio-button", "idle", "border");
			radio_button_idle_text				 = GetColor("radio-button", "idle", "text");

			menu_button_disabled_background		 = GetColor("menu-button", "disabled", "background");
			menu_button_disabled_text			 = GetColor("menu-button", "disabled", "text");
			menu_button_disabled_text_small		 = GetColor("menu-button", "disabled", "text-small");
			menu_button_active_background		 = GetColor("menu-button", "active", "background");
			menu_button_active_text				 = GetColor("menu-button", "active", "text");
			menu_button_active_text_small		 = GetColor("menu-button", "active", "text-small");
			menu_button_hovering_background		 = GetColor("menu-button", "hovering", "background");
			menu_button_hovering_text			 = GetColor("menu-button", "hovering", "text");
			menu_button_hovering_text_small		 = GetColor("menu-button", "hovering", "text-small");
			menu_button_idle_background			 = GetColor("menu-button", "idle", "background");
			menu_button_idle_text				 = GetColor("menu-button", "idle", "text");
			menu_button_idle_text_small			 = GetColor("menu-button", "idle", "text-small");

			toggle_menu_button_disabled_background = GetColor("toggle-menu-button", "disabled", "background");
			toggle_menu_button_disabled_text	   = GetColor("toggle-menu-button", "disabled", "text");
			toggle_menu_button_disabled_text_small = GetColor("toggle-menu-button", "disabled", "text-small");
			toggle_menu_button_active_background   = GetColor("toggle-menu-button", "active", "background");
			toggle_menu_button_active_text		   = GetColor("toggle-menu-button", "active", "text");
			toggle_menu_button_active_text_small   = GetColor("toggle-menu-button", "active", "text-small");
			toggle_menu_button_active_square       = GetColor("toggle-menu-button", "active", "square");
			toggle_menu_button_hovering_background = GetColor("toggle-menu-button", "hovering", "background");
			toggle_menu_button_hovering_text	   = GetColor("toggle-menu-button", "hovering", "text");
			toggle_menu_button_hovering_text_small = GetColor("toggle-menu-button", "hovering", "text-small");
			toggle_menu_button_idle_background	   = GetColor("toggle-menu-button", "idle", "background");
			toggle_menu_button_idle_text		   = GetColor("toggle-menu-button", "idle", "text");
			toggle_menu_button_idle_text_small	   = GetColor("toggle-menu-button", "idle", "text-small");

			scrollbar_idle_handle	   = GetColor("scrollbar", "idle", "handle");
			scrollbar_idle_bar		   = GetColor("scrollbar", "idle", "bar");
			scrollbar_hovering_handle  = GetColor("scrollbar", "hovering", "handle");
			scrollbar_hovering_bar	   = GetColor("scrollbar", "hovering", "bar");
			scrollbar_active_handle	   = GetColor("scrollbar", "active", "handle");
			scrollbar_active_bar	   = GetColor("scrollbar", "active", "bar");

			slider_disabled_background = GetColor("slider", "disabled", "background");
			slider_disabled_border	   = GetColor("slider", "disabled", "border");
			slider_disabled_value	   = GetColor("slider", "disabled", "value");
			slider_disabled_value_text = GetColor("slider", "disabled", "value-text");
			slider_disabled_name_text  = GetColor("slider", "disabled", "name-text");
			slider_idle_background	   = GetColor("slider", "idle", "background");
			slider_idle_border		   = GetColor("slider", "idle", "border");
			slider_idle_value		   = GetColor("slider", "idle", "value");			
			slider_idle_value_text	   = GetColor("slider", "idle", "value-text");
			slider_idle_name_text	   = GetColor("slider", "idle", "name-text");
			slider_hovering_background = GetColor("slider", "hovering", "background");
			slider_hovering_border	   = GetColor("slider", "hovering", "border");
			slider_hovering_value	   = GetColor("slider", "hovering", "value");
			slider_hovering_value_text = GetColor("slider", "hovering", "value-text");
			slider_hovering_name_text  = GetColor("slider", "hovering", "name-text");
			slider_active_background   = GetColor("slider", "active", "background");
			slider_active_border	   = GetColor("slider", "active", "border");
			slider_active_value		   = GetColor("slider", "active", "value");
			slider_active_value_text   = GetColor("slider", "active", "value-text");
			slider_active_name_text    = GetColor("slider", "active", "name-text");

			knob_disabled_handle	  = GetColor("knob", "disabled", "handle");
			knob_disabled_border	  = GetColor("knob", "disabled", "border");
			knob_disabled_value		  = GetColor("knob", "disabled", "value");
			knob_disabled_name_text	  = GetColor("knob", "disabled", "name-text");
			knob_disabled_value_text  = GetColor("knob", "disabled", "value-text");
			knob_idle_handle		  = GetColor("knob", "idle", "handle");
			knob_idle_border		  = GetColor("knob", "idle", "border");
			knob_idle_value			  = GetColor("knob", "idle", "value");
			knob_idle_name_text		  = GetColor("knob", "idle", "name-text");
			knob_idle_value_text	  = GetColor("knob", "idle", "value-text");
			knob_hovering_handle	  = GetColor("knob", "hovering", "handle");
			knob_hovering_border	  = GetColor("knob", "hovering", "border");
			knob_hovering_value		  = GetColor("knob", "hovering", "value");
			knob_hovering_name_text	  = GetColor("knob", "hovering", "name-text");
			knob_hovering_value_text  = GetColor("knob", "hovering", "value-text");
			knob_active_handle		  = GetColor("knob", "active", "handle");
			knob_active_border		  = GetColor("knob", "active", "border");
			knob_active_value		  = GetColor("knob", "active", "value");
			knob_active_name_text	  = GetColor("knob", "active", "name-text");
			knob_active_value_text	  = GetColor("knob", "active", "value-text");

			volume_slider_disabled_handle		   = GetColor("volume-slider", "disabled", "handle");
			volume_slider_disabled_db_text		   = GetColor("volume-slider", "disabled", "db-text");
			volume_slider_disabled_value_text	   = GetColor("volume-slider", "disabled", "value-text");
			volume_slider_disabled_line			   = GetColor("volume-slider", "disabled", "line");
			volume_slider_disabled_line_highlight  = GetColor("volume-slider", "disabled", "line-highlight");
			volume_slider_disabled_meter		   = GetColor("volume-slider", "disabled", "meter");
			volume_slider_disabled_meter_value	   = GetColor("volume-slider", "disabled", "meter-value");
			volume_slider_disabled_meter_value_c1  = GetColor("volume-slider", "disabled", "meter-value-c1");
			volume_slider_disabled_meter_value_c2  = GetColor("volume-slider", "disabled", "meter-value-c2");
			volume_slider_disabled_meter_value_c3  = GetColor("volume-slider", "disabled", "meter-value-c3");
			volume_slider_idle_handle			   = GetColor("volume-slider", "idle", "handle");
			volume_slider_idle_db_text			   = GetColor("volume-slider", "idle", "db-text");
			volume_slider_idle_value_text		   = GetColor("volume-slider", "idle", "value-text");
			volume_slider_idle_line				   = GetColor("volume-slider", "idle", "line");
			volume_slider_idle_line_highlight	   = GetColor("volume-slider", "idle", "line-highlight");
			volume_slider_idle_meter			   = GetColor("volume-slider", "idle", "meter");
			volume_slider_idle_meter_value		   = GetColor("volume-slider", "idle", "meter-value");
			volume_slider_idle_meter_value_c1	   = GetColor("volume-slider", "idle", "meter-value-c1");
			volume_slider_idle_meter_value_c2	   = GetColor("volume-slider", "idle", "meter-value-c2");
			volume_slider_idle_meter_value_c3	   = GetColor("volume-slider", "idle", "meter-value-c3");
			volume_slider_hovering_handle		   = GetColor("volume-slider", "hovering", "handle");
			volume_slider_hovering_db_text		   = GetColor("volume-slider", "hovering", "db-text");
			volume_slider_hovering_value_text	   = GetColor("volume-slider", "hovering", "value-text");
			volume_slider_hovering_line			   = GetColor("volume-slider", "hovering", "line");
			volume_slider_hovering_line_highlight  = GetColor("volume-slider", "hovering", "line-highlight");
			volume_slider_hovering_meter		   = GetColor("volume-slider", "hovering", "meter");
			volume_slider_hovering_meter_value	   = GetColor("volume-slider", "hovering", "meter-value");
			volume_slider_hovering_meter_value_c1  = GetColor("volume-slider", "hovering", "meter-value-c1");
			volume_slider_hovering_meter_value_c2  = GetColor("volume-slider", "hovering", "meter-value-c2");
			volume_slider_hovering_meter_value_c3  = GetColor("volume-slider", "hovering", "meter-value-c3");
			volume_slider_active_handle			   = GetColor("volume-slider", "active", "handle");
			volume_slider_active_db_text		   = GetColor("volume-slider", "active", "db-text");
			volume_slider_active_value_text		   = GetColor("volume-slider", "active", "value-text");
			volume_slider_active_line			   = GetColor("volume-slider", "active", "line");
			volume_slider_active_line_highlight	   = GetColor("volume-slider", "active", "line-highlight");
			volume_slider_active_meter			   = GetColor("volume-slider", "active", "meter");
			volume_slider_active_meter_value	   = GetColor("volume-slider", "active", "meter-value");
			volume_slider_active_meter_value_c1	   = GetColor("volume-slider", "active", "meter-value-c1");
			volume_slider_active_meter_value_c2	   = GetColor("volume-slider", "active", "meter-value-c2");
			volume_slider_active_meter_value_c3	   = GetColor("volume-slider", "active", "meter-value-c3");

			effect_graph_background			   = GetColor("effect-graph", "background");
			effect_graph_graph_lines		   = GetColor("effect-graph", "graph-lines");
			effect_graph_graph_lines_highlight = GetColor("effect-graph", "graph-lines-highlight");
			effect_graph_disabled_circle	   = GetColor("effect-graph", "disabled", "circle");
			effect_graph_disabled_line		   = GetColor("effect-graph", "disabled", "line");
			effect_graph_idle_circle		   = GetColor("effect-graph", "idle", "circle");
			effect_graph_idle_line   		   = GetColor("effect-graph", "idle", "line");
			effect_graph_hovering_circle	   = GetColor("effect-graph", "hovering", "circle");
			effect_graph_hovering_line		   = GetColor("effect-graph", "hovering", "line");
			effect_graph_active_circle		   = GetColor("effect-graph", "active", "circle");
			effect_graph_active_line		   = GetColor("effect-graph", "active", "line");

			dynamics_background		   = GetColor("dynamics", "background");
			dynamics_border			   = GetColor("dynamics", "border");
			dynamics_line			   = GetColor("dynamics", "line");
			dynamics_line_highlight	   = GetColor("dynamics", "line-highlight");
			dynamics_compressor		   = GetColor("dynamics", "compressor");
			dynamics_compressor_line   = GetColor("dynamics", "compressor-line");
			dynamics_meter_value	   = GetColor("dynamics", "meter-value");
			dynamics_db_text		   = GetColor("dynamics", "db-text");
			dynamics_value_text		   = GetColor("dynamics", "value-text");

			route_button_idle_background	 = GetColor("route-button", "idle", "background");
			route_button_idle_triangle		 = GetColor("route-button", "idle", "triangle");
			route_button_hovering_background = GetColor("route-button", "hovering", "background");
			route_button_hovering_triangle	 = GetColor("route-button", "hovering", "triangle");
			route_button_active_background	 = GetColor("route-button", "active", "background");
			route_button_active_triangle	 = GetColor("route-button", "active", "triangle");

			channel_idle_name_text			 = GetColor("channel", "idle", "name-text");
			channel_idle_background			 = GetColor("channel", "idle", "background");
			channel_hovering_name_text		 = GetColor("channel", "hovering", "name-text");
			channel_hovering_background		 = GetColor("channel", "hovering", "background");
			channel_active_name_text		 = GetColor("channel", "active", "name-text");
			channel_active_background		 = GetColor("channel", "active", "background");

			mute_mono_button_idle_background	 = GetColor("mute-mono-button", "idle", "background");
			mute_mono_button_idle_text			 = GetColor("mute-mono-button", "idle", "text");
			mute_mono_button_hovering_background = GetColor("mute-mono-button", "hovering", "background");
			mute_mono_button_hovering_text		 = GetColor("mute-mono-button", "hovering", "text");
			mute_mono_button_active_background	 = GetColor("mute-mono-button", "active", "background");
			mute_mono_button_active_text		 = GetColor("mute-mono-button", "active", "text");

			effect_title_text      = GetColor("effect", "title-text");
			effect_title_text_off  = GetColor("effect", "title-text-off");
			effect_title_bar	   = GetColor("effect", "title-bar");
			effect_background	   = GetColor("effect", "background");
			effect_minimize_button = GetColor("effect", "minimize-button");
			effect_divider		   = GetColor("effect", "divider");
			effect_drag_divider	   = GetColor("effect", "drag-divider");

			divider = GetColor("divider");

			window_border						    = GetColor("window", "border");
			window_frame						    = GetColor("window", "frame");
			window_panel						    = GetColor("window", "panel");
			window_title_text					    = GetColor("window", "title-text");
			window_menu_text					    = GetColor("window", "menu-text");
			window_close_button_icon			    = GetColor("window", "close-button", "icon");
			window_close_button_idle_background	    = GetColor("window", "close-button", "idle", "background");
			window_close_button_hovering_background = GetColor("window", "close-button", "hovering", "background");
			window_close_button_active_background   = GetColor("window", "close-button", "active", "background");
			window_button_icon					    = GetColor("window", "button", "icon");
			window_button_idle_background		    = GetColor("window", "button", "idle", "background");
			window_button_hovering_background	    = GetColor("window", "button", "hovering", "background");
			window_button_active_background		    = GetColor("window", "button", "active", "background");

			menu_background = GetColor("menu", "background");
			menu_border = GetColor("menu", "border");
			menu_divider = GetColor("menu", "divider");
		}
		catch (...)
		{
			LOG("Error loading theme");
		}
	}

	template<typename ...Args>
	Color GetColor(Args... args)
	{
		return GetColor(m_Json.at("components"), args...);
	};

	template<typename Arg, typename ...Args>
	Color GetColor(const nlohmann::json& json, Arg a, Args... args)
	{
		try
		{
			return GetColor(json.at(a), args...);
		}
		catch (...)
		{
			LOG("Error loading color : " << a);
			return Color{ 0, 0, 0, 255 };
		}
	};

	template<typename Arg>
	Color GetColor(const nlohmann::json& json, Arg arg)
	{
		try
		{
			return GetColor(json.at(arg));
		}
		catch (...)
		{
			LOG("Error loading color : " << arg);
			return Color{ 0, 0, 0, 255 };
		}
	};

	Color GetColor(const nlohmann::json& json)
	{
		try
		{
			if (json.is_string())
				return m_Variables.at(json.get<std::string>());
			return Color{ json.at(0), json.at(1), json.at(2), json.at(3) };
		}
		catch (...)
		{
			LOG("Error loading color : " << json);
			return Color{ 0, 0, 0, 255 };
		}
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
		d.Command<Font>(Fonts::Gidole, 24.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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
		d.Command<Font>(Fonts::Gidole16, 11.0f);
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
		d.Command<Font>(Fonts::Gidole16, 11.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);		
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

		d.Command<Font>(Fonts::Gidole14, 14.0f);
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

			d.Command<Font>(Fonts::Gidole14, 14.0f);
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

			d.Command<Font>(Fonts::Gidole14, 14.0f);
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

			d.Command<Font>(Fonts::Gidole14, 14.0f);
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

			d.Command<Font>(Fonts::Gidole14, 14.0f);
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

			d.Command<Font>(Fonts::Gidole14, 14.0f);
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
			 d.Command<Font>(Fonts::Gidole14, 14.0f);
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