#include "Gui/Slider.hpp"

SliderParser::SliderParser()
{
	Parser::Link<ParameterParser>();
	settings.name = "slider";
	Attribute("border-width", &Slider::m_BorderWidth);
	Attribute("border-color", &Slider::m_BorderColor);
	Attribute("color", &Slider::m_Color);
	Attribute("background", &Slider::m_Background);
	Attribute("text-color", &Slider::m_TextColor);
	Attribute("font-size", &Slider::m_FontSize);
	Attribute("font", &Slider::m_Font);
	Attribute("unit", &Slider::m_Unit);

	enumMap["no-unit"] = Units::NONE;
	enumMap["pan-unit"] = Units::PAN;
	enumMap["decibel-unit"] = Units::DECIBEL;
	enumMap["percent-unit"] = Units::PERCENT;
}

Pointer<Component> SliderParser::Create()
{
	return new Slider;
}

Slider::Slider(const Settings& settings)
	: settings(settings)
{
	Init();
}

void Slider::Update()
{
	m_Value = Units::units[settings.unit].Format(value, settings.decimals);
}

void Slider::Render(CommandCollection& d) const
{
	d.Fill(settings.border.color.Current());
	d.Quad(dimensions);

	d.Fill(settings.background.Current());
	d.Quad({ x + settings.border.width, y + settings.border.width, width - 2 * settings.border.width, height - 2 * settings.border.width });

	float _width = width - 2 * settings.border.width;
	float _start = x + settings.border.width;
	float _end = Normalize(value) * _width;
	if (range.start < 0 && range.end > 0)
	{
		_start += _width / 2;
		_end -= _width / 2;
	}
	d.Fill(settings.color.Current());
	d.Quad({ _start, y + settings.border.width, _end, height - 2 * settings.border.width });

	d.Fill(settings.text.color.Current());
	d.TextAlign(Align::Center);
	d.FontSize(settings.text.size);
	d.Font(settings.font);
	d.Text(m_Value, { x + width / 2, y + height / 2 });
}

void Slider::Init()
{
	Parameter::settings.vertical = false;
	settings.background.Link(this);
	settings.color.Link(this);
	settings.border.color.Link(this);
	settings.text.color.Link(this);
}
