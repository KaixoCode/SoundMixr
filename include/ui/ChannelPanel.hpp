#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- Channel Panel --------------------------------- \\
// -------------------------------------------------------------------------- \\

class ChannelPanel : public Panel
{
public:
	ChannelPanel(StereoInputChannel& c);
	ChannelPanel(StereoOutputChannel& c);

	void Select(StereoInputChannel* s);
	void Select(StereoOutputChannel* s);
	void Selected(bool v) { m_Selected = v; }
	bool Selected() { return m_Selected; }
	void Unselect() { m_HasSelect = false; routed.Disable(); }
	void Routed(bool v) { m_Routed = v; }
	bool Routed() { return m_Routed; }
	bool Input() { return m_IsInput; }
	auto InputChannel()  -> StereoInputChannel* { return m_InputChannel; }
	auto OutputChannel() -> StereoOutputChannel* { return m_OutputChannel; }
	void Transparency(bool t) { m_Transparency = t; }

	Button<SmallText, ButtonType::Normal>& text;
	Button<RouteButton, ButtonType::Toggle>& routed;
	Button<MuteButton, ButtonType::Toggle>& muted;
	Button<MonoButton, ButtonType::Toggle>& mono;
	PanSlider& pan;

private:
	bool m_Transparency = false;

	bool m_IsInput;
	union
	{
		StereoInputChannel* m_InputChannel;
		StereoOutputChannel* m_OutputChannel;
	};

	bool m_HasSelect = false;
	bool m_IsSelectedInput;
	union
	{
		StereoInputChannel* m_SelectedInputChannel;
		StereoOutputChannel* m_SelectedOutputChannel;
	};

	VolumeSlider& m_VolumeSlider;

	bool m_Selected = false,
		m_Routed = false;

	std::unordered_map<int, std::string> m_Numbers;
	std::string m_NegInf = "Inf";

	void Init();
	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;
};
