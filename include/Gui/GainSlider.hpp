#pragma once
#include "pch.hpp"
#include "Gui/Parameter.hpp"
#include "Audio/Channel.hpp"

struct GainSlider : public Parameter
{
	static inline std::map<int, std::string> strings;

	GainSlider();
	GainSlider& operator=(GainSlider&&) = default;

	Pointer<Audio::Channel> channel;

	float meterWidth = 16;
	float meterPadding = 8;
	float b = 1.3;
	float step = 3;
	bool numbers = true;
	bool lines = true;
	int fontSize = 14;
	std::string font = GraphicsBase::DefaultFont;
	StateColors bar{ {.link = this, .base{ 100, 100, 100 } } };
	StateColors text{ {.link = this, .base{ 255, 255, 255 } } };
	StateColors meter{ {.link = this, .base{ 0, 255, 0 } } };
	StateColors line{ {.link = this, .base{ 128, 128, 128 } } };
	StateColors background{ {.link = this, .base{ 0, 0, 0 } } };

	float Gain();

	void Update() override;
	void Render(CommandCollection& d) const override;

private:
	std::string m_Value;
};