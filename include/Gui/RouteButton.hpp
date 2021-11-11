#pragma once
#include "pch.hpp"

struct RouteButtonGraphics : public Button::Graphics
{
	StateColors background{ {
		.base = { 100, 100, 100, 0 },
		.transition = 10.f,
	} };

	StateColors triangle{ {
		.base = { 50, 50, 50, 255 },
		.colors = {
			{ Hovering, { 70, 70, 70, 255 } },
			{ Selected, { 100, 100, 100, 255 } },
			{ Pressed, { 100, 100, 100, 255 } },
		},
		.transition = 10.f,
	} };

	bool input = false;

	void Link(Button* button) 
	{
		background.Link(button);
		triangle.Link(button);
		button->State(Disabled) = true;
	}

	void Render(CommandCollection& d) const override
	{
		if (button->State(Disabled))
			return;

		double _div = 3;
		float _w = 3 * button->height / _div;
		float _h = 2 * button->height / _div;
		d.Fill(background.Current());
		d.Quad(button->dimensions);
		d.Fill(triangle.Current());
		d.Triangle(Vec4<float>{ button->x + button->width / 2, button->y + button->height / 2, _w, _h }, input ? -90.0f : 90.0f);
	}
};