#include "Gui/GainSlider.hpp"
#include "Gui/Units.hpp"

GainSlider::GainSlider()
	: Parameter{ {
		.range{ -66, 12 },
		.value = 0,
		.reset = 0,
		.vertical = true,
		.scaling = [&](float v) {
			float _x = 1 + (std::atanh((v - 1) * std::tanh(b)) / b);
			return _x < 0.001 ? 0 : _x;
		},
		.inverse = [&](float v) {
			return 1 - std::tanh(b * (1 - v)) / std::tanh(b);
		}
	} }
{}

float GainSlider::Gain()
{
	return value == range.start ? 0 : db2lin(value);
}

void GainSlider::Update() 
{
	if (!channel)
		return;

	width = std::max(70, channel->lines * 16 + 8 + 30);

	m_Value = Units::units[Units::DECIBEL].Format(value, 1);
}

void GainSlider::Render(CommandCollection& d) const 
{
	if (!channel)
		return;
	float _height = height - 20;
	float _step = step;
	if (_height < 300)
		_step = step * 2;
	if (_height < 100)
		_step = step * 4;
	float _bx = 4;
	float _sidepadding = 26;
	float _a = 1 / std::tanh(b);
	float _padding = 2;
	float _ypadding = 2;
	float _x = _bx + x + _padding;
	float _w = ((width - (_bx + _sidepadding)) - _padding) / channel->lines;
	for (int i = 0; i < channel->lines; i++)
	{
		float _db = lin2db(channel->smoothed[i]);
		float _norm = std::max((_db - range.start) / (range.end - range.start), 0.f);
		float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
		float _0db = std::max((0 - range.start) / (range.end - range.start), 0.f);
		float _0scaled = 1 - _a * std::tanh(b * (1 - _0db));

		float _h = _scaled * (_height - 2 * _ypadding);
		float _0h = std::round(_0scaled * (_height - 2 * _padding));
		d.Fill(background.Current());
		d.Quad({ _x, y + _height - _ypadding, _w - _padding, -_0h + 1 });
		d.Quad({ _x, y + _ypadding, _w - _padding, _height - _0h - 2 * _ypadding });
		d.Fill(meter.Current());
		d.Quad({ _x, y + _height - _ypadding, _w - _padding, -_h });
		_x += _w;
	}
	_x -= _padding;

	if (lines)
	{
		d.Fill(line.Current());
		d.TextAlign(Align::Left | Align::Middle);
		for (int i = range.start + _step; i <= range.end; i += _step)
		{
			float _norm = std::max((i - range.start) / (range.end - range.start), 0.f);
			float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
			float _y = std::round(y + _height - _ypadding - _scaled * (_height - 2 * _ypadding));
			d.Quad({ _x, _y, 5, 1 });
		}
	}

	if (numbers)
	{
		d.FontSize(fontSize);
		d.Font(font);
		d.Fill(text.Current());
		d.TextAlign(Align::Right | Align::Middle);
		for (int i = range.start + step * 2; i <= range.end; i += _step * 2)
		{
			float _norm = std::max((i - range.start) / (range.end - range.start), 0.f);
			float _scaled = 1 - _a * std::tanh(b * (1 - _norm));
			float _y = std::round(y + _height - _ypadding - _scaled * (_height - 2 * _ypadding));
			if (lines)
				d.Quad({ _x, _y, 5, 1 });

			int abs = i < 0 ? -i : i;
			if (!strings.contains(abs))
				strings[abs] = std::to_string(abs);
			d.Text(strings[abs], { x + width - 4, _y });
		}

		d.TextAlign(Align::Left | Align::Middle);
		d.Text(m_Value, { x + _bx + _padding, y + _height + 10 });
	}

	_padding = 6;
	float _h = 25;
	float _he = _h - _padding * 2;
	float _norm = std::max((value - range.start) / (range.end - range.start), 0.f);
	float _scaled = _a * std::tanh(b * (1 - _norm));
	float _y = y + std::round(_scaled * (_height - 2 * _ypadding)) - _padding + 1;
	d.Fill(bar.Current());
	d.Triangle(Vec4<float>{x + _bx, _y + _padding, 8, _he}, 0.0f);
	d.Triangle(Vec4<float>{x + width - _sidepadding, _y + _padding, 8, _he}, 180.0f);
	d.Quad(Vec4<float>{x + _bx, _y + _padding - 1, width - _sidepadding, 3});
}
