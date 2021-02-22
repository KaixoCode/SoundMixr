#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

ChannelPanel::ChannelPanel(AsioDevice& sar, StereoInputChannel& c)
	: m_InputChannel(&c), m_IsInput(true), m_SarAsio(sar),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name())),
	volume(Emplace<VolumeSlider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "in")),
	muted(Emplace<Button<MuteButton, ButtonType::Toggle>>(&c.muted, "MUTE")),
	mono(Emplace<Button<MonoButton, ButtonType::Toggle>>(&c.mono, "MONO")),
	pan(Emplace<PanSlider>())
{
	Init();
	m_Menu.ButtonSize({ 150, 20 });
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(c.Name()).Disable();
	m_Menu.Emplace<MenuAccessories::Divider>(150, 1, 2, 2);
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Normal>>([&] { volume.SliderValue(1); }, "Reset Volume");
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Normal>>([&] { pan.SliderValue(0); }, "Reset Pan");
	m_Menu.Emplace<MenuAccessories::Divider>(150, 1, 2, 2);
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&c.muted, "Mute");
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&c.mono, "Mono");

	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT)
		{
			RightClickMenu::Get().Open(&m_Menu);
		}
	};
}

ChannelPanel::ChannelPanel(AsioDevice& sar, StereoOutputChannel& c)
	: m_OutputChannel(&c), m_IsInput(false), m_SarAsio(sar),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name())),
	volume(Emplace<VolumeSlider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "")),
	muted(Emplace<Button<MuteButton, ButtonType::Toggle>>(&c.muted, "MUTE")),
	mono(Emplace<Button<MonoButton, ButtonType::Toggle>>(&c.mono, "MONO")),
	pan(Emplace<PanSlider>())
{
	Init();
	m_Menu.ButtonSize({ 150, 20 });
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(c.Name()).Disable();
	m_Menu.Emplace<MenuAccessories::Divider>(150, 1, 2, 2);
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Normal>>([&] { volume.SliderValue(1); }, "Reset Volume");
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Normal>>([&] { pan.SliderValue(0); }, "Reset Pan");
	m_Menu.Emplace<MenuAccessories::Divider>(150, 1, 2, 2);
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&c.muted, "Mute");
	m_Menu.Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&c.mono, "Mono");
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::RIGHT)
		{
			RightClickMenu::Get().Open(&m_Menu);
		}
	};
}

void ChannelPanel::Select(StereoInputChannel* s)
{
	m_HasSelect = true;
	if (m_IsInput)
	{
		m_Routed = false;
		routed.Disable();
	}
	else
	{
		m_Routed = s->Connected(m_OutputChannel);
		routed.Enable();
	}
	m_IsSelectedInput = true;
	m_SelectedInputChannel = s;
}

void ChannelPanel::Select(StereoOutputChannel* s)
{
	m_HasSelect = true;
	if (m_IsInput)
	{
		m_Routed = m_InputChannel->Connected(s);
		routed.Enable();
	}
	else
	{
		m_Routed = false;
		routed.Disable();
	}
	m_IsSelectedInput = false;
	m_SelectedOutputChannel = s;
}

void ChannelPanel::Init()
{
	pan.Position(Vec2<int>{4, 25});
	volume.Position(Vec2<int>{0, 95});
	muted.Position(Vec2<int>{5, 50});
	muted.Size({ 27, 25 });
	mono.Position(Vec2<int>{38, 50});
	mono.Size({ 27, 25 });
	routed.Position(Vec2<int>{0, 0});
	routed.Size({ 70, 25 });
	text.Disable();
	text.Size({ 70, 24 });
	routed.Disable();

	Width(70);
}

void ChannelPanel::Update(const Vec4<int>& viewport)
{
	if (m_HasSelect)
		if (m_IsSelectedInput && !m_IsInput)
			if (m_Routed)
				if (!m_SelectedInputChannel->Connected(m_OutputChannel))
					m_SelectedInputChannel->Connect(m_OutputChannel);
				else;
			else
				if (m_SelectedInputChannel->Connected(m_OutputChannel))
					m_SelectedInputChannel->Disconnect(m_OutputChannel);
				else;
		else if (!m_IsSelectedInput && m_IsInput)
			if (m_Routed)
				if (!m_InputChannel->Connected(m_SelectedOutputChannel))
					m_InputChannel->Connect(m_SelectedOutputChannel);
				else;
			else
				if (m_InputChannel->Connected(m_SelectedOutputChannel))
					m_InputChannel->Disconnect(m_SelectedOutputChannel);
				else;

	if (Selected())
		Background(Color{ 26, 26, 26, m_Transparency ? 245.0f : 255.0f });
	else
		Background(Color{ 33, 33, 33, m_Transparency ? 245.0f : 255.0f });

	if (m_Vertical)
	{
		volume.Vertical(false);
		pan.Position(Vec2<int>{4, 4});
		volume.Position(Vec2<int>{95, 15});
		muted.Position(Vec2<int>{38, 29});
		mono.Position(Vec2<int>{5, 29});
		routed.Position(Vec2<int>{0, 0});
		text.Position(Vec2<int>{0, Height() - 24});
		volume.Size(Vec2<int>{Width() - 135, 50});
		pan.Size(Vec2<int>{62, 19});
		Height(70);
	} 
	else
	{
		volume.Vertical(true);
		pan.Position(Vec2<int>{4, 25});
		volume.Position(Vec2<int>{0, 95});
		muted.Position(Vec2<int>{5, 50});
		mono.Position(Vec2<int>{38, 50});
		routed.Position(Vec2<int>{0, 0});
		text.Position(Vec2<int>{0, Height() - 24});
		volume.Size(Vec2<int>{50, Height() - 135});
		pan.Size(Vec2<int>{62, 19});
		Width(70);
	}

	if (Input())
	{
		m_InputChannel->volume = volume.SliderValue();
		m_InputChannel->pan = pan.SliderValue();
	}
	else
	{
		m_OutputChannel->volume = volume.SliderValue();
		m_OutputChannel->pan = pan.SliderValue();
	}

	Panel::Update(viewport);
}

void ChannelPanel::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();
	d.Command<Translate>(Position());
	Background(d);

	float _levelLeft = 0, _levelRight = 0;
	if (Input())
		_levelLeft = m_InputChannel->level_left, _levelRight = m_InputChannel->level_right;
	else
		_levelLeft = m_OutputChannel->level_left, _levelRight = m_OutputChannel->level_right;

	_levelLeft = std::powf(_levelLeft, 0.25);
	_levelRight = std::powf(_levelRight, 0.25);

	if (m_Vertical)
	{ 
		int _x = 100;
		int _y = 25;
		int _rw = Width() - 35 - _x;
		int _w = (std::min(_levelLeft, 1.412536f) / 1.412536) * (_rw);
		int _0db = ((1.0 / 1.412536) * (_rw)) + _x;
		int _3db = ((std::powf(std::powf(10, 3 / 20.0), 0.25) / 1.412536) * (_rw)) + _x;
		int _6db = ((std::powf(std::powf(10, 6 / 20.0), 0.25) / 1.412536) * (_rw)) + _x;
		int _h = 14;

		d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _rw, _h});
		d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_0db, _y, 1, _h});
		d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		if (_levelLeft > 1.0)
		{
			d.Command<Graphics::Fill>(Color{ 255, 255, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_0db, _y, _w - _0db + _x, _h});
		}
		if (_levelLeft > std::powf(std::powf(10, 3 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 126, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_3db, _y, _w - _3db + _x, _h});
		}
		if (_levelLeft > std::powf(std::powf(10, 6 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 0, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_6db, _y, _w - _6db + _x, _h});
		}

		_y = 25 + 16;
		_w = (std::min(_levelRight, 1.412536f) / 1.412536) * (_rw);

		d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _rw, _h});
		d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_0db, _y, 1, _h});
		d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		if (_levelRight > 1.0)
		{
			d.Command<Graphics::Fill>(Color{ 255, 255, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_0db, _y, _w - _0db + _x, _h});
		}
		if (_levelRight > std::powf(std::powf(10, 3 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 126, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_3db, _y, _w - _3db + _x, _h});
		}
		if (_levelRight > std::powf(std::powf(10, 6 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 0, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_6db, _y, _w - _6db + _x, _h});
		}

		int _d = 3;
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::CENTER, Align::BOTTOM);
		for (int i = 12; i > -120; i -= _d)
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;

			if (_b)
				d.Command<Graphics::Fill>(Color{ 128, 128, 128, 255 });
			else
				d.Command<Graphics::Fill>(Color{ 64, 64, 64, 255 });

			int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rw)) + _x;
			d.Command<Graphics::Quad>(Vec4<int>{_mdb, 20, 1, 5});
			if (_b)
			{
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i, std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(Color{ 200, 200, 200, 255 });
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_mdb, 4 });
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(Color{ 128, 128, 128, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, 20, 1, 5});
		d.Command<Graphics::Fill>(Color{ 200, 200, 200, 255 });
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x, 4});
	}
	else
	{
		int _x = 10;
		int _y = 100;
		int _rh = Height() - 35 - _y;
		int _h = (std::min(_levelLeft, 1.412536f) / 1.412536) * (_rh);
		int _0db = ((1.0 / 1.412536) * (_rh)) + _y;
		int _3db = ((std::powf(std::powf(10, 3 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		int _6db = ((std::powf(std::powf(10, 6 / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
		int _w = 14;

		d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
		d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
		d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});
		if (_levelLeft > 1.0)
		{
			d.Command<Graphics::Fill>(Color{ 255, 255, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, _h - _0db + _y});
		}	
		if (_levelLeft > std::powf(std::powf(10, 3 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 126, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _3db, _w, _h - _3db + _y});
		}
		if (_levelLeft > std::powf(std::powf(10, 6 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 0, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _6db, _w, _h - _6db + _y});
		}
	
		_x = 10 + 16;
		_h = (std::min(_levelRight, 1.412536f) / 1.412536)* (_rh);

		d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _rh});
		d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
		d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});	
		if (_levelRight > 1.0)
		{
			d.Command<Graphics::Fill>(Color{ 255, 255, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, _h - _0db + _y});
		}
		if (_levelRight > std::powf(std::powf(10, 3 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 126, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _3db, _w, _h - _3db + _y});
		}
		if (_levelRight > std::powf(std::powf(10, 6 / 20.0), 0.25))
		{
			d.Command<Graphics::Fill>(Color{ 255, 0, 0, 255 });
			d.Command<Graphics::Quad>(Vec4<int>{_x, _6db, _w, _h - _6db + _y});
		}

		int _d = 3;
		bool _b = true;
		d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14.0f);
		d.Command<Graphics::TextAlign>(Align::RIGHT, Align::CENTER);
		for (int i = 12; i > -120; i-= _d)
		{
			if (i < -11)
				_d = 6;
			if (i < -47)
				_d = 12;
			if (i < -71)
				_d = 24;

			if (_b)
				d.Command<Graphics::Fill>(Color{ 128, 128, 128, 255 });
			else
				d.Command<Graphics::Fill>(Color{ 64, 64, 64, 255 });
		
			int _mdb = ((std::powf(std::powf(10, i / 20.0), 0.25) / 1.412536) * (_rh)) + _y;
			d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _mdb, 5, 1});
			if (_b)
			{	
				if (m_Numbers.find(i) == m_Numbers.end())
				{
					m_Numbers.emplace(i,std::to_string(std::abs(i)));
				}
				d.Command<Graphics::Fill>(Color{ 200, 200, 200, 255 });
				d.Command<Graphics::Text>(&m_Numbers[i], Vec2<int>{_x + _w + 25, _mdb});
			}
			_b ^= true;
		}
		d.Command<Graphics::Fill>(Color{ 128, 128, 128, 255 });
		d.Command<Graphics::Quad>(Vec4<int>{_x + _w, _y, 5, 1});
		d.Command<Graphics::Fill>(Color{ 200, 200, 200, 255 });
		d.Command<Graphics::Text>(&m_NegInf, Vec2<int>{_x + _w + 25, _y});
	}
	Container::Render(d);
	d.Command<PopMatrix>();
}