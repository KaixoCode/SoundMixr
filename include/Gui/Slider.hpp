#pragma once
#include "pch.hpp"
#include "Gui/Parameter.hpp"
#include "Gui/Units.hpp"

class Slider : public Parameter
{
public:
	struct Settings
	{
		StateColors background{ {
			.base = { 26, 26, 26 },
		} };

		StateColors color{ {
			.base = { 36, 36, 36 },
		} };

		struct Border
		{
			StateColors color{ {
				.base = { 12, 12, 12 },
			} };
			float width = 1;
		} border;

		struct Text
		{
			StateColors color{ {
				.base = { 255, 255, 255 },
			} };
			float size = 14;
		} text;

		std::string font = GraphicsBase::DefaultFont;

		struct Display
		{
			bool name = false;
			bool value = true;
		} display;

		int decimals = 0;
		int unit = Units::NONE;
	} settings;

	Slider(const Settings& settings = {});
	Slider(Slider&& other) = delete;
	Slider(const Slider&) = delete;
	Slider& operator=(Slider&& other) = delete;
	Slider& operator=(const Slider& other) = delete;

	void Update() override;
	void Render(CommandCollection& d) const override;

private:
	std::string m_Value;

	void Init();
};
