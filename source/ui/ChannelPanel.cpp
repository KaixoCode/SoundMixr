#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

ChannelPanel::ChannelPanel(StereoInputChannel& c)
	: m_InputChannel(&c), m_IsInput(true),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{70, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "in", Vec2<int>{70, 25})),
	muted(Emplace<Button<MuteButton, ButtonType::Toggle>>(&c.muted, "MUTE", Vec2<int>{27, 25})),
	mono(Emplace<Button<MonoButton, ButtonType::Toggle>>(&c.mono, "MONO", Vec2<int>{27, 25}))
{
	Init();
}

ChannelPanel::ChannelPanel(StereoOutputChannel& c)
	: m_OutputChannel(&c), m_IsInput(false),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{70, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "", Vec2<int>{70, 25})),
	muted(Emplace<Button<MuteButton, ButtonType::Toggle>>(&c.muted, "MUTE", Vec2<int>{27, 25})),
	mono(Emplace<Button<MonoButton, ButtonType::Toggle>>(&c.mono, "MONO", Vec2<int>{27, 25}))
{
	Init();
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
	m_VolumeSlider.Position(Vec2<int>{0, 95});
	muted.Position(Vec2<int>{5, 50});
	mono.Position(Vec2<int>{38, 50});
	routed.Position(Vec2<int>{0, 0});
	text.Disable();
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

	text.Position(Vec2<int>{0, Height() - 24});
	m_VolumeSlider.Size(Vec2<int>{50, Height() - 110});

	if (Input())
		m_InputChannel->Volume(m_VolumeSlider.SliderValue());
	else
		m_OutputChannel->Volume(m_VolumeSlider.SliderValue());

	Panel::Update(viewport);
}

void ChannelPanel::Render(CommandCollection& d)
{
	using namespace Graphics;
	d.Command<PushMatrix>();
	if (!m_TranslateBackground)
		Background(d);

	d.Command<Translate>(Vec2<int>{ X(), Y() });
	if (m_TranslateBackground)
		Background(d);

	float _levelLeft = 0, _levelRight = 0;
	if (Input())
		_levelLeft = m_InputChannel->level_left, _levelRight = m_InputChannel->level_right;
	else
		_levelLeft = m_OutputChannel->level_left, _levelRight = m_OutputChannel->level_right;

	_levelLeft = std::powf(_levelLeft, 0.25);
	_levelRight = std::powf(_levelRight, 0.25);

	int _x = 10;
	int _y = 100;
	int _rh = Height() - 35 - _y;
	int _h = (_levelLeft / 1.412536) * (_rh);
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
	_h = (_levelRight / 1.412536) * (_rh);

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
	d.Command<Graphics::Font>(Graphics::Fonts::Gidole14, 14);
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

	Container::Render(d);
	d.Command<PopMatrix>();
}