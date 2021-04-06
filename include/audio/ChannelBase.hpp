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

	enum class ChannelType : int
	{
		Input = 0x1,
		Output = 0x2,
		Generator = 0x4,
		SoundBoard = 0x8
	};

	ChannelBase(ChannelType type)
		: m_Type(type),
		volume(Emplace<OldVolumeSlider>()),
		pan(Emplace<PanSlider>()),
		mute(Emplace<Button<MuteButton, ButtonType::Toggle>>("MUTE")),
		mono(Emplace<Button<MonoButton, ButtonType::Toggle>>("MONO"))
	{
		Background({0, 255, 255, 255});
		Width(70);
	}

	/**
	 * Connect to a channel.
	 * @param c channel
	 */
	void Connect(ChannelBase* c)
	{
		if (!std::contains(m_Connections, c))
			m_Connections.push_back(c);
	}

	/**
	 * Disconnect a channel.
	 * @param c channel
	 */
	void Disconnect(ChannelBase* c)
	{
		auto& it = std::find(m_Connections.begin(), m_Connections.end(), c);
		if (it != m_Connections.end())
			m_Connections.erase(it);
	}

	/**
	 * Input sample at certain channel.
	 * @param c channel
	 * @param s sample
	 */
	virtual void Input(int c, float s)
	{
		m_Levels[c] += s;
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
		Color c;
		//if (Selected())
		//	c = ThemeT::Get().channel_active_background;
		if (Hovering())
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

	OldVolumeSlider& volume;
	PanSlider& pan;
	Button<MuteButton, ButtonType::Toggle>& mute;
	Button<MonoButton, ButtonType::Toggle>& mono;

protected:
	int m_Id = 0;
	unsigned int m_Lines = 0;
	const ChannelType m_Type;
	std::vector<ChannelBase*> m_Connections;
	std::vector<float> m_Levels;

	EffectChain m_EffectChain;
};