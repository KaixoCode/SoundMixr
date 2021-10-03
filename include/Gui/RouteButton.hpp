#pragma once
#include "pch.hpp"

enum State
{
	Input = LastState
};

struct RouteButton : public Button
{
	StateColors background{ {
		.link = this,
		.base = { 100, 100, 100, 0 },
		.transition = 10.f,
	} };

	StateColors triangle{ {
		.link = this,
		.base = { 50, 50, 50, 255 },
		.colors = {
			{ Hovering, { 70, 70, 70, 255 } },
			{ Selected, { 100, 100, 100, 255 } },
			{ Pressed, { 100, 100, 100, 255 } },
		},
		.transition = 10.f,
	} };

	bool input = false;

	RouteButton()
	{
		State<Disabled>(true);
		settings = {
			.type = Toggle
		};
	}

	void Render(CommandCollection& d) const override
	{
		if (State<Disabled>())
			return;

		double _div = 3;
		float _w = 3 * height / _div;
		float _h = 2 * height / _div;
		d.Fill(background.Current());
		d.Quad(dimensions);
		d.Fill(triangle.Current());
		d.Triangle(Vec4<float>{ x + width / 2, y + height / 2, _w, _h }, State<Input>() ? -90.0f : 90.0f);
	}
};

struct RouteButtonParser : public ButtonParser
{
	RouteButtonParser()
	{
		settings.name = "route-button";
		Attribute("triangle", &RouteButton::triangle);
		Attribute("background", &RouteButton::background);
	}

	Pointer<Component> Create()
	{
		return new RouteButton{};
	}
};