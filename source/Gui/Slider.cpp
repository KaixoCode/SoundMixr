#include "Gui/Slider.hpp"

Slider::Slider(const Parameter::Settings& settings)
	: Parameter(settings)
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

	float _height = height - 2 * settings.border.width;
	float _start = y + settings.border.width + _height;
	float _end = Normalize(value) * _height;
	if (range.start < 0 && range.end > 0)
	{
		_start += _height / 2;
		_end -= _height / 2;
	}
	d.Fill(settings.color.Current());
	d.Quad({ x + settings.border.width, _start, width - 2 * settings.border.width, -_end });

	d.Fill(settings.text.color.Current());
	d.TextAlign(Align::Center);
	d.FontSize(settings.text.size);
	d.Font(settings.font);
	d.Text(m_Value, { x + width / 2, y + height / 2 });
}

void Slider::Init()
{
	Parameter::settings.vertical = true;
	settings.background.Link(this);
	settings.color.Link(this);
	settings.border.color.Link(this);
	settings.text.color.Link(this);
}
