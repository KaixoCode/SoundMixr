#include "ui/ChannelPanel.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

ChannelPanel::ChannelPanel(StereoInputChannel& c)
	: m_InputChannel(&c), m_IsInput(true),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{100, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&m_Routed, "", Vec2<int>{25, 25}))
{
	Init();
}

ChannelPanel::ChannelPanel(StereoOutputChannel& c)
	: m_OutputChannel(&c), m_IsInput(false),
	text(Emplace<Button<SmallText, ButtonType::Normal>>([]() {}, c.Name(), Vec2<int>{100, 24})),
	m_VolumeSlider(Emplace<Slider>()),
	routed(Emplace<Button<ButtonGraphics::Menu, ButtonType::Toggle>>(&m_Routed, "", Vec2<int>{25, 25}))
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
	m_VolumeSlider.Position(Vec2<int>{0, 0});
	text.Disable();

	Width(100);
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
	routed.Position(Vec2<int>{50, Height() - 50});
	m_VolumeSlider.Size(Vec2<int>{50, Height() - 24});

	if (Input())
		m_InputChannel->Volume(m_VolumeSlider.SliderValue());
	else
		m_OutputChannel->Volume(m_VolumeSlider.SliderValue());

	Panel::Update(viewport);
}
