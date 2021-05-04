#include "ui/Components.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Radio Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

RadioButton::RadioButton(Effects::RadioButton& t, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons)
	: m_RadioButton(t), Button<RadioButtonGraphics, ButtonType::List>([&]
		{
			// Get the ButtonType::List id using the radio button id and the lookup.
			auto _listId = m_Keys[m_RadioButton.Id()];

			// If this button is pressed it unselects all button in
			// the buttongroup with the id _listId
			for (auto& i : m_RButtons)

				// If id found
				if (i.first == _listId)
				{
					// Unselect all buttons in group
					for (auto& c : i.second)
						c->Selected(false);

					// Break out of the loop
					break;
				}

			// After unselecting all buttons, select this button
			m_RadioButton.Selected(true);
		}, t.Name(), GetKey(t, keys, buttons)), m_Keys(keys), m_RButtons(buttons)
{}

void RadioButton::Update(const Vec4<int>& v)
{
	m_Size = { m_RadioButton.Size().width, m_RadioButton.Size().height };
	m_Pos = { m_RadioButton.Position().x, m_RadioButton.Position().y };

	if (!Selected() && m_RadioButton.Selected())
	{
		Selected(true);
	}

	Button<RadioButtonGraphics, ButtonType::List>::Update(v);
}

int RadioButton::GetKey(Effects::RadioButton& k, std::unordered_map<int, int>& keys, std::unordered_map<int, std::vector<Effects::RadioButton*>>& buttons)
{
	int id = 0;
	auto& _it = keys.find(k.Id());
	if (_it == keys.end())
	{
		id = keys.emplace(k.Id(), ButtonType::List::NewKey()).first->second;
		auto& i = buttons.emplace(id, std::vector<Effects::RadioButton*>{});
		i.first->second.push_back(&k);
	}
	else
	{
		id = _it->second;
		buttons[id].push_back(&k);
	}
	return id;
}

// -------------------------------------------------------------------------- \\
// --------------------------- Toggle Button -------------------------------- \\
// -------------------------------------------------------------------------- \\

ToggleButton::ToggleButton(Effects::ToggleButton& t)
	: m_Toggle(t), Button<ToggleButtonGraphics, ButtonType::Toggle>(&t.state, t.Name())
{}

void ToggleButton::Update(const Vec4<int>& v)
{
	// Update the size and position
	m_Size = { m_Toggle.Size().width, m_Toggle.Size().height };
	m_Pos = { m_Toggle.Position().x, m_Toggle.Position().y };

	Button<ToggleButtonGraphics, ButtonType::Toggle>::Update(v);
}

// -------------------------------------------------------------------------- \\
// --------------------------- XY Controller -------------------------------- \\
// -------------------------------------------------------------------------- \\

XYController::XYController(Effects::XYController& c)
	: controller(c)
{
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT && e.x > X() && e.x < X() + Width() && e.y > Y() && e.y < Y() + Height())
			m_Dragging = true;
	};

	m_Listener += [this](Event::MouseClicked& e)
	{
		if (e.button == Event::MouseButton::LEFT)
		{
			if (m_Click > 0)
				controller.Param1().ResetValue(), controller.Param2().ResetValue();
			m_Click = 20;
		}
	};

	m_Listener += [this](Event::MouseEntered& e)
	{
		m_Hovering = true;
	};

	m_Listener += [this](Event::MouseExited& e)
	{
		m_Hovering = false;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		if (m_Dragging)
		{
			double _x = constrain((e.x - X() - 8) / (Width() - 8 * 2.0), 0, 1);
			double _y = constrain((e.y - Y() - 8) / (Height() - 8 * 2.0), 0, 1);
			controller.Param1().NormalizedValue(_x);
			controller.Param2().NormalizedValue(_y);
		}
	};

	m_Listener += [this](Event::MouseReleased& e)
	{
		m_Dragging = false;
	};

	m_Listener += [this](Event::KeyPressed& e)
	{
		if (!Focused())
			return;

		double amt = 0.01;
		if (e.keymod & Event::Mod::CONTROL)
			amt *= 4;
		else if (e.keymod & Event::Mod::SHIFT)
			amt *= 0.1;

		if (e.key == Key::LEFT)
			controller.Param1().NormalizedValue(controller.Param1().NormalizedValue() - amt);

		if (e.key == Key::RIGHT)
			controller.Param1().NormalizedValue(controller.Param1().NormalizedValue() + amt);

		if (e.key == Key::UP)
			controller.Param2().NormalizedValue(controller.Param2().NormalizedValue() + amt);

		if (e.key == Key::DOWN)
			controller.Param2().NormalizedValue(controller.Param2().NormalizedValue() - amt);

		m_Dragging = false;
	};


}

void XYController::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	d.Command<Fill>(ThemeT::Get().effect_graph_background);
	d.Command<Quad>(Vec4<int>{ 0, 0, Width(), Height() });

	int _p = 8;
	int _x = controller.Param1().NormalizedValue() * (Width() - 2 * _p) + _p;
	int _y = controller.Param2().NormalizedValue() * (Height() - 2 * _p) + _p;
	if (controller.Param1().Disabled() && controller.Param2().Disabled())
		d.Command<Fill>(ThemeT::Get().effect_graph_disabled_circle);
	else if (m_Dragging)
		d.Command<Fill>(ThemeT::Get().effect_graph_active_circle);
	else if (m_Hovering)
		d.Command<Fill>(ThemeT::Get().effect_graph_hovering_circle);
	else
		d.Command<Fill>(ThemeT::Get().effect_graph_idle_circle);
	d.Command<Graphics::Ellipse>(Vec4<int>{ _x, _y, _p * 2, _p * 2 });
	d.Command<PopMatrix>();
}

void XYController::Update(const Vec4<int>& v)
{
	if (m_Click)
		m_Click--;
	m_Size = { controller.Size().width, controller.Size().height };
	m_Pos = { controller.Position().x, controller.Position().y };

	Container::Update(v);
}

// -------------------------------------------------------------------------- \\
// --------------------------- Volume Slider -------------------------------- \\
// -------------------------------------------------------------------------- \\

VolumeSlider::VolumeSlider(Effects::VolumeSlider& s)
	: Parameter<VolumeSliderGraphics>(s), m_Slider(s)
{
	m_Parameter.Range({ 0.0, 3.98107 });
	m_Parameter.Power(4);
	m_Parameter.Value(1);
	m_Parameter.ResetValue(1);
	m_Parameter.Vertical(true);
}

void VolumeSlider::Update(const Vec4<int>& v)
{
	m_Size = { m_Parameter.Size().width, m_Parameter.Size().height };
	m_Pos = { m_Parameter.Position().x, m_Parameter.Position().y };

	Component::Update(v);

	char s[10];
	if (Decibels() < -100)
		m_ValueText = "-inf";
	else {
		std::sprintf(s, "%.1f", Decibels());
		m_ValueText = s;
	}
	m_ValueText += "dB";
}

void VolumeSlider::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	int _channels = m_Slider.Channels();

	int _width = m_Slider.Size().width - 16;
	int _x = 6;
	int _y = 5;
	int _rh = Height();
	int _w = (_width / _channels) - 2;
	int _0db = ((std::powf(1, 0.25) / 1.412536) * (_rh)) + _y;

	// Draw all audio meters
	for (int i = 0; i < _channels; i++)
	{
		_x = 9 + i * (_w + 2);
		float _level1 = std::powf(m_Slider.Values()[i], 0.25);
		float _level2 = std::powf(m_Slider.Reduces()[i], 0.25);

		int _h1 = (std::min(_level1, 1.412536f) / 1.412536) * (_rh);
		int _h2 = (std::min(_level2, 1.412536f) / 1.412536) * (_rh);
		
		if (Disabled())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_meter);
		else if (Dragging())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_meter);
		else if (Hovering())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_meter);
		else
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_meter);

		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
		d.Command<Graphics::Fill>(ThemeT::Get().channel_idle_background);
		d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});

		if (Disabled())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_meter_value_c1);
		else if (Dragging())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_meter_value_c1);
		else if (Hovering())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_meter_value_c1);
		else
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_meter_value_c1);

		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h1});

		if (Disabled())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_meter_value);
		else if (Dragging())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_meter_value);
		else if (Hovering())
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_meter_value);
		else
			d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_meter_value);

		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h2});

	}

	// db numbers besides volume meter
	int _d = 3;
	if (Height() < 200)
		_d = 6;

	bool _b = true;
	d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
	d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
	for (int i = 12; i > -120; i -= _d)
	{
		if (Height() < 200)
		{
			if (i < -11)
				_d = 12;
			if (i < -35)
				_d = 36;
			if (i < -86)
				break;
		}
		else
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;
		}

		if (Disabled())
			d.Command<Graphics::Fill>(_b ? ThemeT::Get().volume_slider_disabled_line_highlight : ThemeT::Get().volume_slider_disabled_line);
		else if (Dragging())
			d.Command<Graphics::Fill>(_b ? ThemeT::Get().volume_slider_active_line_highlight : ThemeT::Get().volume_slider_active_line);
		else if (Hovering())
			d.Command<Graphics::Fill>(_b ? ThemeT::Get().volume_slider_hovering_line_highlight : ThemeT::Get().volume_slider_hovering_line);
		else
			d.Command<Graphics::Fill>(_b ? ThemeT::Get().volume_slider_idle_line_highlight : ThemeT::Get().volume_slider_idle_line);

		int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
		if (_b)
		{
			if (m_Numbers.find(i) == m_Numbers.end())
			{
				m_Numbers.emplace(i, std::to_string(std::abs(i)));
			}

			if (Disabled())
				d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_db_text);
			else if (Dragging())
				d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_db_text);
			else if (Hovering())
				d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_db_text);
			else
				d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_db_text);

			d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
		}
		_b ^= true;
	}

	if (Disabled())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_line_highlight);
	else if (Dragging())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_line_highlight);
	else if (Hovering())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_line_highlight);
	else
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_line_highlight);

	d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});

	if (Disabled())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_disabled_db_text);
	else if (Dragging())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_active_db_text);
	else if (Hovering())
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_hovering_db_text);
	else
		d.Command<Graphics::Fill>(ThemeT::Get().volume_slider_idle_db_text);

	d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y});
	d.Command<PopMatrix>();
	Parameter<VolumeSliderGraphics>::Render(d);
}

// -------------------------------------------------------------------------- \\
// ----------------------------- Pan Slider --------------------------------- \\
// -------------------------------------------------------------------------- \\

PanSlider::PanSlider(Effects::Parameter& param)
	: Parameter<PanSliderGraphics>(param)
{
	Name("pan");
	Range({ -50, 50 });
	Power(1);
	Decimals(-1);
	ResetValue(0);
	Vertical(false);
	ResetValue();
	Unit("L", -1);
	Unit("C", INT_MAX);
	Unit("R", 0);
	Multiplier(1);
}

// -------------------------------------------------------------------------- \\
// -------------------------- Dynamics Slider ------------------------------- \\
// -------------------------------------------------------------------------- \\

DynamicsSlider::DynamicsSlider(Effects::DynamicsSlider& o)
	: m_Object(o)
{
	UpdateStrings();

	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button != Event::MouseButton::LEFT || e.x < X() || e.x > X() + Width())
			return;

		m_PVal = constrain(e.x - X(), 0, Width());
		if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
			m_Dragging = RT2, m_PressVal = m_Object.ExpanderRatio();
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
			m_Dragging = RT1, m_PressVal = m_Object.CompressorRatio();
		else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
			m_Dragging = TH2, m_PressVal = DbToPixel(m_Object.ExpanderThreshhold());
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
			m_Dragging = TH1, m_PressVal = DbToPixel(m_Object.CompressorThreshhold());
	};
	m_Listener += [this](Event::MouseClicked& e)
	{
		int p = m_Click;
		m_Click = 20;

		if (!p)
			return;

		m_PVal = constrain(e.x - X(), 0, Width());
		if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
			m_Object.ExpanderRatio(0);
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
			m_Object.CompressorRatio(0);
		else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
			m_Object.ExpanderThreshhold(-50), m_Object.CompressorThreshhold(std::max(m_Object.CompressorThreshhold(), -50.0));
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
			m_Object.CompressorThreshhold(-10), m_Object.ExpanderThreshhold(std::min(m_Object.ExpanderThreshhold(), -10.0));

		UpdateStrings();
	};
	m_Listener += [this](Event::MouseMoved& e)
	{
		m_PVal = constrain(e.x - X(), 0, Width());
		if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) - m_DragRange)
			m_Cursor = GLFW_RESIZE_EW_CURSOR;
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) + m_DragRange)
			m_Cursor = GLFW_RESIZE_EW_CURSOR;
		else if (m_PVal < DbToPixel(m_Object.ExpanderThreshhold()) + m_DragRange)
			m_Cursor = GLFW_HAND_CURSOR;
		else if (m_PVal > DbToPixel(m_Object.CompressorThreshhold()) - m_DragRange)
			m_Cursor = GLFW_HAND_CURSOR;
		else
			m_Cursor = GLFW_CURSOR_NORMAL;
	};
	m_Listener += [this](Event::MouseDragged& e)
	{
		int cval = e.x - X();

		if (m_Dragging == TH2)
		{
			double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Width()));
			m_Cursor = GLFW_HAND_CURSOR;
			m_Object.ExpanderThreshhold(db1);

			if (m_Object.ExpanderThreshhold() > m_Object.CompressorThreshhold())
				m_Object.CompressorThreshhold(m_Object.ExpanderThreshhold());

			UpdateStrings();
		}
		else if (m_Dragging == TH1)
		{
			double db1 = PixelToDb(constrain(cval - m_PVal + m_PressVal, 0, Width()));
			m_Cursor = GLFW_HAND_CURSOR;
			m_Object.CompressorThreshhold(db1);

			if (m_Object.ExpanderThreshhold() > m_Object.CompressorThreshhold())
				m_Object.ExpanderThreshhold(m_Object.CompressorThreshhold());

			UpdateStrings();
		}
		else if (m_Dragging == RT1)
		{
			double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
			m_PVal = cval;
			m_Cursor = GLFW_RESIZE_EW_CURSOR;
			m_Object.CompressorRatio(constrain(db1 + m_Object.CompressorRatio(), -31, 32));
			UpdateStrings();
		}
		else if (m_Dragging == RT2)
		{
			double db1 = (cval - m_PVal) * (m_Shift ? 0.1 : 0.2);
			m_PVal = cval;
			m_Cursor = GLFW_RESIZE_EW_CURSOR;
			m_Object.ExpanderRatio(constrain(m_Object.ExpanderRatio() - db1, -31, 32));
			UpdateStrings();
		}
	};

	m_Listener += [this](Event::KeyPressed& e)
	{
		if (e.key == VK_SHIFT)
			m_Shift = true;
	};

	m_Listener += [this](Event::KeyReleased& e)
	{
		if (e.key == VK_SHIFT)
			m_Shift = false;
	};

	m_Listener += [this](Event::MouseReleased& e)
	{
		m_Dragging = 0;
	};
}

void DynamicsSlider::Update(const Vec4<int>& v)
{
	m_Pos.x = m_Object.Position().x;
	m_Pos.y = m_Object.Position().y;
	m_Size.width = m_Object.Size().width;
	m_Size.height = m_Object.Size().height;

	if (m_Click)
		m_Click--;

	UpdateStrings();

	Container::Update(v);
}

void DynamicsSlider::Render(CommandCollection& d)
{
	Container::Render(d);
	using namespace Graphics;
	d.Command<Fill>(ThemeT::Get().dynamics_background);
	d.Command<Quad>(Vec4<int>{ X() - 7, Y() - 20, Width() + 14, Height() + 20 });

	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	int _y = Height() - 22;
	int _d = 3;
	bool _b = true;
	d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
	d.Command<Graphics::TextAlign>(Align::CENTER, Align::BOTTOM);
	for (int i = 0; i > -120; i -= _d)
	{
		if (i < -11)
			_d = 6;
		if (i < -47)
			_d = 12;
		if (i < -71)
			_d = 24;

		if (_b)
			d.Command<Graphics::Fill>(ThemeT::Get().dynamics_line);
		else
			d.Command<Graphics::Fill>(ThemeT::Get().dynamics_line_highlight);

		int _mdb = DbToPixel(i) - 1;
		d.Command<Quad>(Vec4<int>{ _mdb, _y, 1, 5});
		if (_b)
		{
			if (m_Numbers.find(i) == m_Numbers.end())
			{
				m_Numbers.emplace(i, std::to_string(std::abs(i)));
			}
			d.Command<Graphics::Fill>(ThemeT::Get().dynamics_db_text);
			d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_mdb, _y + 7});
		}
		_b ^= true;
	}
	d.Command<Graphics::Fill>(ThemeT::Get().dynamics_line_highlight);
	d.Command<Graphics::Quad>(Vec4<int>{0, _y, 1, 5});
	d.Command<Graphics::Fill>(ThemeT::Get().dynamics_db_text);
	d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{5, _y + 7});

	int _y2 = 5;
	d.Command<Graphics::Fill>(ThemeT::Get().dynamics_border);
	d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, Width(), 1 });
	d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, 1, _y });
	d.Command<Graphics::Quad>(Vec4<int>{ 0, _y, Width(), 1 });
	d.Command<Graphics::Quad>(Vec4<int>{ Width() - 1, 0, 1, _y });
	int p1 = DbToPixel(m_Object.CompressorThreshhold());
	d.Command<Graphics::Fill>(ThemeT::Get().dynamics_compressor);
	d.Command<Graphics::Quad>(Vec4<int>{ p1, _y2, Width() - p1 - 1, _y - _y2 - 5 });
	double _xp = p1;
	for (int i = 0; i < 8; i++)
	{
		auto c = ThemeT::Get().dynamics_compressor_line;
		c.a *= (8 - i) / 8.0f;
		d.Command<Graphics::Fill>(c);
		d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
		_xp += m_Object.CompressorRatio() * 0.5 + 17;
		if (_xp > Width())
			break;
	}

	int p2 = DbToPixel(m_Object.ExpanderThreshhold());
	d.Command<Graphics::Fill>(ThemeT::Get().dynamics_compressor);
	d.Command<Graphics::Quad>(Vec4<int>{ 1, _y2, p2 - 1, _y - _y2 - 5 });
	_xp = p2;
	for (int i = 0; i < 8; i++)
	{
		auto c = ThemeT::Get().dynamics_compressor_line;
		c.a *= (8 - i) / 8.0f;
		d.Command<Graphics::Fill>(c);
		d.Command<Graphics::Quad>(Vec4<int>{ (int)_xp, _y2, 1, _y - _y2 - 5 });
		_xp -= m_Object.ExpanderRatio() * 0.5 + 17;
		if (_xp < 0)
			break;
	}
	d.Command<Fill>(ThemeT::Get().dynamics_value_text);
	d.Command<Font>(Fonts::Gidole14, 14.0f);
	d.Command<TextAlign>(Align::LEFT, Align::TOP);
	d.Command<Text>(&m_TH2Str, Vec2<int>{0, -3});
	d.Command<Text>(&m_RT2Str, Vec2<int>{60, -3});
	d.Command<Text>(&m_RT1Str, Vec2<int>{Width() - 100, -3});
	d.Command<TextAlign>(Align::RIGHT, Align::TOP);
	d.Command<Text>(&m_TH1Str, Vec2<int>{Width(), -3});

	int _channels = m_Object.Channels();
	if (_channels)
	{
		int _x = 0;
		int _rw = m_Object.Size().width;
		int _p = 16;
		int _h = ((m_Object.Size().height - 21 - _p * 2) / _channels) - (_channels > 4 ? 1 : 2);
		_y = _p;

		// Draw all audio meters
		for (int i = 0; i < _channels; i++)
		{
			_y = _p + i * (_h + (_channels > 4 ? 1 : 2));
			float _level = std::powf(m_Object.Levels()[i], 0.25);

			int _w = (std::min(_level, 1.0f) / 1.0f) * (_rw);
			d.Command<Graphics::Fill>(ThemeT::Get().dynamics_meter_value);
			d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		}
	}
	d.Command<PopMatrix>();
}

double DynamicsSlider::PixelToDb(int p)
{
	double v = ((double)p) / (double)(Width());
	return 20 * std::log10(std::max((double)std::powf(v, 4), 0.000001));
}

int DynamicsSlider::DbToPixel(double p)
{
	return std::pow(std::pow(10, p / 20.0), 0.25) * (Width());
}

void DynamicsSlider::UpdateStrings()
{
	char s[10];
	std::sprintf(s, "%.1f", m_Object.CompressorThreshhold());
	m_TH1Str = s;
	m_TH1Str += "dB";
	std::sprintf(s, "%.1f", m_Object.ExpanderThreshhold());
	m_TH2Str = s;
	m_TH2Str += "dB";
	double rt1 = m_Object.CompressorRatio();
	if (rt1 >= 0)
	{
		rt1 = 1.0 / ((rt1 / 32.0) + 1);
		std::sprintf(s, "%.2f", rt1);
		m_RT1Str = "1 : ";
		m_RT1Str += s;
	}
	else
	{
		rt1 = std::abs(rt1 - 1);
		std::sprintf(s, "%.2f", rt1);
		m_RT1Str = "1 : ";
		m_RT1Str += s;
	}
	double rt2 = m_Object.ExpanderRatio();
	if (rt2 >= 0)
	{
		rt2 = 1.0 / ((rt2 / 8.0) + 1);
		std::sprintf(s, "%.2f", rt2);
		m_RT2Str = "1 : ";
		m_RT2Str += s;
	}
	else
	{
		rt2 = std::abs(rt2 - 1);
		std::sprintf(s, "%.2f", rt2);
		m_RT2Str = "1 : ";
		m_RT2Str += s;
	}
}

// -------------------------------------------------------------------------- \\
// ---------------------------- Filter Curve -------------------------------- \\
// -------------------------------------------------------------------------- \\

float FilterCurve::Magnitude(float freq)
{
	float wf = /*6.28318530718*/ 3.1415926 * (freq / 22000);
	float avg = 0;

	float e0jw = std::exp(0 * wf);
	float e1jw = std::exp(-1 * wf);
	float e2jw = std::exp(-2 * wf);

	int amount = 0;

	for (auto& p : m_Curve.Parameters())
	{
		if (p.type == FilterType::Off)
			continue;

		amount++;

		// Transfer function:

		float c1 = p.a0 * e0jw;
		float c2 = p.a1 * e1jw;
		float c3 = p.a2 * e2jw;

		float d1 = p.b0 * e0jw;
		float d2 = p.b1 * e1jw;
		float d3 = p.b2 * e2jw;

		float Hw = (c1 + c2 + c3) / (d1 + d2 + d3);
		float mag = std::abs(Hw);

		if (mag == 0)
			mag = 0.00000000001;

		avg += 20 * std::log10(mag);
	}
	if (amount)
		return avg / amount;
	return 0;
}


// -------------------------------------------------------------------------- \\
// ------------------------- Simple Filter Curve ---------------------------- \\
// -------------------------------------------------------------------------- \\

SimpleFilterCurve::SimpleFilterCurve(Effects::SimpleFilterCurve& params)
	: m_Curve(params)
{
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT && e.x > X() && e.x < X() + Width() && e.y > Y() && e.y < Y() + Height())
			m_Dragging = true;

	};

	m_Listener += [this](Event::MouseClicked& e)
	{
		if (e.button == Event::MouseButton::LEFT)
		{
			if (m_Click > 0)
				m_Curve.freq.ResetValue(), m_Curve.width.ResetValue();
			m_Click = 20;
		}
	};

	m_Listener += [this](Event::MouseEntered& e)
	{
		m_Hovering = true;
	};

	m_Listener += [this](Event::MouseExited& e)
	{
		m_Hovering = false;
	};

	m_Listener += [this](Event::MouseDragged& e)
	{
		if (m_Dragging)
		{
			double _x = constrain((e.x - X() - 8) / (Width() - 8 * 2.0), 0, 1);
			double _y = constrain((e.y - Y() - 8) / (Height() - 8 * 2.0), 0, 1);
			if (!m_Curve.freq.Disabled())
				m_Curve.freq.NormalizedValue(_x);
			if (!m_Curve.width.Disabled())
				m_Curve.width.NormalizedValue(_y);
		}
	};

	m_Listener += [this](Event::MouseReleased& e)
	{
		m_Dragging = false;
	};
	m_Listener += [this](Event::KeyPressed& e)
	{
		if (!Focused())
			return;

		double amt = 0.01;
		if (e.keymod & Event::Mod::CONTROL)
			amt *= 4;
		else if (e.keymod & Event::Mod::SHIFT)
			amt *= 0.1;

		if (e.key == Key::LEFT)
			m_Curve.freq.NormalizedValue(m_Curve.freq.NormalizedValue() - amt);

		if (e.key == Key::RIGHT)
			m_Curve.freq.NormalizedValue(m_Curve.freq.NormalizedValue() + amt);

		if (e.key == Key::UP)
			m_Curve.width.NormalizedValue(m_Curve.width.NormalizedValue() + amt);

		if (e.key == Key::DOWN)
			m_Curve.width.NormalizedValue(m_Curve.width.NormalizedValue() - amt);

	};
}


void SimpleFilterCurve::UpdateMags()
{
	if (m_Dragging) m_Update = true;


	int size = std::ceil(Width() / m_Scale) + m_Scale;
	while (m_Mags.size() < size)
		m_Mags.push_back(0), m_Update = true;

	if (m_Update)
	{
		for (int i = 0; i < size; i++)
		{
			float ma = Magnitude(i * m_Scale);

			m_Mags[i] = ma;
		}
		m_Update = false;
	}

	if (m_PrevFreq != m_Curve.freq.Value())
		m_PrevFreq = m_Curve.freq.Value(), m_Update = true;

	if (m_PrevWidth != m_Curve.width.Value())
		m_PrevWidth = m_Curve.width.Value(), m_Update = true;
}

void SimpleFilterCurve::Update(const Vec4<int>& v)
{
	m_Click--;
	m_Size = { m_Curve.Size().width, m_Curve.Size().height };
	m_Pos = { m_Curve.Position().width, m_Curve.Position().height };
	UpdateMags();
	Component::Update(v);
}

void SimpleFilterCurve::Render(CommandCollection& d)
{
	Component::Render(d);

	d.Command<Graphics::Clip>(Vec4<int>{ Position(), Size() });
	d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_background);
	d.Command<Graphics::Quad>(Vec4<int>{ Position(), Size() });

	for (int j = 1; j < 5; j++)
	{
		d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_graph_lines_highlight);
		for (int i = 0; i < 10; i++)
		{
			double f = std::pow(10, j) * i;
			int x = 8 + X() + (Width() - 8 * 2) * FreqToPos(f);
			if (x < X() || x > X() + Width())
				continue;

			d.Command<Graphics::Quad>(Vec4<int>{ x, Y(), 1, Height() });
			d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_graph_lines);
		}
	}
	int size = std::ceil(Width() / m_Scale) + m_Scale;
	if (m_Curve.width.Disabled())
		d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_disabled_line);
	else
		d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_active_line);

	for (int i = 0; i < size - 1; i++)
	{
		float y1 = Y() + m_Mags[i] * (Height() * 0.66);
		float y2 = Y() + m_Mags[i + 1] * (Height() * 0.66);
		float x1 = X() + i * m_Scale;
		float x2 = X() + (i + 1) * m_Scale;
		d.Command<Graphics::Line>(Vec4<float>{ x1, y1, x2, y2 }, 2.0f);
	}

	if (m_Curve.width.Disabled())
		d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_disabled_circle);
	else
		d.Command<Graphics::Fill>(ThemeT::Get().effect_graph_active_circle);

	int p = 8;
	int x = p + X() + (Width() - p * 2) * FreqToPos(m_Curve.freq.Value());
	int y = p + Y() + m_Curve.width.Value() * (Height() - 2 * p);
	d.Command<Graphics::Ellipse>(Vec4<int>{ x, y, p * 2, p * 2 });
	d.Command<Graphics::PopClip>();
}

float SimpleFilterCurve::Magnitude(float a)
{
	auto& hp = m_Curve.Parameters()[0];
	auto& lp = m_Curve.Parameters()[1];

	auto fx = 8 + (Width() - 16) * FreqToPos(m_Curve.m_Parameters.freq);
	auto width = m_Curve.m_Parameters.width + 0.02;
	auto width2 = std::pow(m_Curve.m_Parameters.width + 0.02, 0.25);
	auto hpfx = 8 + (Width() - 16) * FreqToPos(hp.f0) - Width() * 0.1;
	auto lpfx = 8 + (Width() - 16) * FreqToPos(lp.f0) + Width() * 0.1;

	double m = 1 - (std::max((1 - width) * 4, 2.0) - 2);
	double m2 = 2 - (std::max((1 - width) * 4, 3.0) - 3);

	static auto myf2 = [](double a) { return a / std::sqrt(1.0 + a * a); };
	static auto myf = [](double a) { return std::tanh(a); };

	if (a < fx)
		return (std::pow(10, (-myf((hpfx - a - m2 * 30) * 0.05 + 0.9) * 0.5 + 0.5) - 1) - 0.1) * (0.9 + 0.1 * (m));

	else
		return (std::pow(10, (myf((lpfx - a + m2 * 30) * 0.05 - 0.9) * 0.5 + 0.5) - 1) - 0.1) * (0.9 + 0.1 * (m));
}

float SimpleFilterCurve::FreqToPos(float freq)
{
	static const auto mylog = [](double v, double b) { return std::log(v) / b; };

	auto log = freq > 0 ? mylog(freq, m_Logg) : mylog(-freq, m_Logg);

	auto norm1 = (log - mylog(10, m_Logg)) / (mylog(22000, m_Logg) - mylog(10, m_Logg));
	return freq > 0 ? norm1 : -norm1;
}