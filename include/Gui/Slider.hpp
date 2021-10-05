#pragma once
#include "pch.hpp"
#include "Gui/Parameter.hpp"
#include "Gui/Units.hpp"

struct SliderParser : public ParameterParser
{
	SliderParser();
	Pointer<Component> Create();
};

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
	Ref<float> m_BorderWidth = settings.border.width;
	Ref<StateColors> m_Color = settings.color;
	Ref<StateColors> m_BorderColor = settings.border.color;
	Ref<StateColors> m_Background = settings.background;
	Ref<StateColors> m_TextColor = settings.text.color;
	Ref<float> m_FontSize = settings.text.size;
	Ref<std::string> m_Font = settings.font;
	Ref<int> m_Unit = settings.unit;

	std::string m_Value;

	void Init();
	friend class SliderParser;
};
