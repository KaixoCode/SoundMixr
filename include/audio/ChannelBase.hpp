#pragma once
#include "pch.hpp"
#include "audio/EffectChain.hpp"
#include "ui/Parameter.hpp"
#include "ui/Components.hpp"

// Me writing methods that are useful but std doesn't provide, in the namespace 
// std to make things really confusing but it looks like it's just std. Deal with it.
namespace std
{
	template<typename T>
	constexpr static inline bool contains(std::vector<T>& v, const T& t)
	{
		return std::find(v.begin(), v.end(), t) != v.end();
	};
}

class ChannelBase : public Panel
{
public:

	typedef int ChannelType;
	struct Type
	{
		constexpr static inline const int Input = 0x1;
		constexpr static inline const int Output = 0x2;
		constexpr static inline const int Generator = 0x4;
		constexpr static inline const int SoundBoard = 0x8;
		constexpr static inline const int Endpoint = 0x10;
	};

	ChannelBase(ChannelType type)
		: m_Type(type),
		volume(Emplace<VolumeSlider>(volumeParam)),
		pan(Emplace<PanSlider>()),
		name(Emplace<TextComponent>("Discord")),
		volumeVal(Emplace<TextComponent>("0.0dB")),
		mute(Emplace<Button<MuteButton, ButtonType::Toggle>>("MUTE")),
		mono(Emplace<Button<MonoButton, ButtonType::Toggle>>("MONO")),
		route(Emplace<Button<RouteButton, ButtonType::Toggle>>(type & Type::Input ? "in" : ""))
	{
		Background({0, 255, 255, 255});
		Width(70);

		Layout<Layout::Divs>();

		volumeParam.Channels(2);
		volumeVal.AlignLines(Align::LEFT);
		name.AlignLines(Align::CENTER);
		name.Editable(true);
		route.Disable();
		pan.Name("Pan");
		pan.DisplayName(false);

		Div() = { 7, Div::Alignment::Vertical, 0, false, Div::AUTO };
		Div()[6] = { name, Div::Alignment::Center, 28, true };
		Div()[5].DivSize(14);
		Div()[4] = { 2, Div::Alignment::Horizontal, 0, false, Div::AUTO };
		Div()[4][0] = { volume, Div::Alignment::Left, Div::AUTO, true };
		Div()[4][1].DivSize(24);
		Div()[3] = { 2, Div::Alignment::Horizontal, 0, false, 24 };
		Div()[3][0].DivSize(6);
		Div()[3][1] = { volumeVal, Div::Alignment::Center, Div::AUTO, true };
		Div()[2] = { 4, Div::Alignment::Horizontal, 3, false, 32 };
		Div()[2][0].DivSize(2);
		Div()[2][1] = { mono, Div::Alignment::Center, Div::AUTO, true };
		Div()[2][2] = { mute, Div::Alignment::Center, Div::AUTO, true };
		Div()[2][3].DivSize(2);
		Div()[1] = { 3, Div::Alignment::Horizontal, 2, false, 24 };
		Div()[1][0].DivSize(2);
		Div()[1][1] = { pan, Div::Alignment::Center, Div::AUTO, true };
		Div()[1][2].DivSize(2);
		Div()[0] = { route, Div::Alignment::Center, 25, true };

		m_Listener += [this](Event::MousePressed& e)
		{
			// Only select if not hovering over any button/parameter
			bool hovering = pan.Hovering() || 
				mute.Hovering() || mono.Hovering() || (route.Hovering() && !route.Disabled());

			if (!hovering && e.button == Event::MouseButton::LEFT)
				selected = this;
		};
	}

	/**
	 * Connect to a channel.
	 * @param c channel
	 */
	void Connect(ChannelBase* c)
	{
		if (!std::contains(m_Connections, c))
		{
			m_Lock.lock();
			m_Connections.push_back(c);
			m_Lock.unlock();
		}
	}

	/**
	 * Disconnect a channel.
	 * @param c channel
	 */
	void Disconnect(ChannelBase* c)
	{
		auto& it = std::find(m_Connections.begin(), m_Connections.end(), c);
		if (it != m_Connections.end())
		{
			m_Lock.lock();
			m_Connections.erase(it);
			m_Lock.unlock();
		}
	}

	/**
	 * Input sample at certain channel.
	 * @param c channel
	 * @param s sample
	 */
	virtual void Input(int c, float s)
	{
		m_Levels[c % m_Lines] += s;
	}

	/**
	 * Process the current samples.
	 */
	virtual void Process() = 0;

	int  Id() { return m_Id; }
	int  Lines() const { return m_Lines; }
	auto Type() const -> ChannelType { return m_Type; }
	auto Connections() -> std::vector<ChannelBase*>& { return m_Connections; }
	
	/**
	 * Returns true if the given ChannelBase is connected to this ChannelBase.
	 * @param c channel
	 * @return true when connected
	 */
	bool Connected(ChannelBase* c) const
	{ 
		// If it's an input it stores the connections
		if (Type() & Type::Input)
			return std::find(m_Connections.begin(), m_Connections.end(), c) != m_Connections.end();  
		
		// Otherwise it's an output, so check if param is input and is connected to this
		else if (c && (c->Type() & Type::Input))
			return std::find(c->Connections().begin(), c->Connections().end(), this) != c->Connections().end();
	}

	/**
	 * Set how many lines are in this channel. Reallocates buffers.
	 * @param c lines amount
	 */
	void Lines(int c) 
	{
		m_Levels.reserve(c);
		while (m_Levels.size() < c)
			m_Levels.push_back(0);
		m_Lines = c;
	}

	void Update(const Vec4<int>& v)
	{
		// If new channel was selected, make sure the state of the route button matches routing.
		if (m_Prev != selected)
		{
			m_Prev = selected;
			
			// Disable the route button if m_Selected is nullptr or
			// if the type of the channel is the same as this channel, but only for input/output
			if (!selected || (selected->Type() & Type() & (Type::Input | Type::Output)))
				route.Disable();
			else
				route.Enable(), route.Active(Connected(selected));
		}

		// Update connections given the state of the route button.
		if (selected && selected != this)
			if (!(Type() & Type::Input))
				if (route.Active())
					selected->Connect(this);
				else
					selected->Disconnect(this);
			else if (Type() & Type::Input)
				if (route.Active())
					Connect(selected);
				else
					Disconnect(selected);

		// Set volume value in text component.
		volumeVal.Content(volume.ValueText());

		// Background color of the panel using theme.
		Color c;
		if (selected == this)
			c = ThemeT::Get().channel_active_background;
		else if (Hovering())
			c = ThemeT::Get().channel_hovering_background;
		else
			c = ThemeT::Get().channel_idle_background;

		Background(c);
		Panel::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		Panel::Render(d);
	}

	virtual operator nlohmann::json() = 0;
	virtual void operator=(const nlohmann::json&) = 0;

	Effects::VolumeSlider volumeParam;
	VolumeSlider& volume;
	PanSlider& pan;
	TextComponent& name, &volumeVal;
	Button<MuteButton, ButtonType::Toggle>& mute;
	Button<MonoButton, ButtonType::Toggle>& mono;
	Button<RouteButton, ButtonType::Toggle>& route;

	static inline ChannelBase* selected = nullptr;
protected:
	int m_Id = 0;
	unsigned int m_Lines = 0;
	const ChannelType m_Type;
	std::vector<ChannelBase*> m_Connections;
	std::vector<float> m_Levels;

	EffectChain m_EffectChain;

	std::mutex m_Lock;

	ChannelBase* m_Prev = nullptr;
};