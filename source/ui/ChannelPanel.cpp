#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

ChannelPanel::ChannelPanel(StereoInputChannel& c)
	: m_InputChannel(&c), m_IsInput(true),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{50, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "in", Vec2<int>{25, 25}))
{
	Init();
}

ChannelPanel::ChannelPanel(StereoOutputChannel& c)
	: m_OutputChannel(&c), m_IsInput(false),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{50, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<RouteButton, ButtonType::Toggle>>(&m_Routed, "", Vec2<int>{25, 25}))
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
	m_VolumeSlider.Position(Vec2<int>{0, 35});
	text.Disable();
	routed.Disable();

	Width(50);
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
		Background(Color{ 26, 26, 26, 255 });
	else
		Background(Color{ 33, 33, 33, 255 });

	text.Position(Vec2<int>{0, Height() - 24});
	routed.Position(Vec2<int>{12, 8});
	m_VolumeSlider.Size(Vec2<int>{50, Height() - 60});

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
	int _y = 40;
	int _h = (_levelLeft / 1.412536) * (Height() - 85);
	int _0db = ((1.0 / 1.412536) * (Height() - 85)) + _y;
	int _w = 14;

	d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, Height() - 85});
	d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});

	d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});
	
	_x = 10 + 16;
	_y = 40;
	_h = (_levelRight / 1.412536) * (Height() - 85);
	_w = 14;

	d.Command<Graphics::Fill>(Color{ 0, 0, 0, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, Height() - 85});
	d.Command<Graphics::Fill>(Color{ 0, 255, 0, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _y, _w, _h});

	d.Command<Graphics::Fill>(Color{ 33, 33, 33, 255 });
	d.Command<Graphics::Quad>(Vec4<int>{_x, _0db, _w, 1});

	Container::Render(d);
	d.Command<PopMatrix>();
}