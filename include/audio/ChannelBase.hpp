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

/**
 * Basis for any channel, Endpoint/Generator/SoundBoard etc.
 */
class ChannelBase : public Panel
{
public:

	/**
	 * The channel type, can be stacked, channel is either Input or Output and one of
	 * Generator, Soundboard and Endpoint.
	 */
	struct Type
	{
		constexpr static inline const int Input = 0x1;
		constexpr static inline const int Output = 0x2;
		constexpr static inline const int Generator = 0x4;
		constexpr static inline const int SoundBoard = 0x8;
		constexpr static inline const int Endpoint = 0x10;
	};
	typedef int ChannelType;

	/**
	 * Constructor.
	 * @param type channeltype
	 */
	ChannelBase(ChannelType type);

	/**
	 * Destructor with lock to make sure it isn't being deleted
	 * when the process method is still busy in another thread.
	 */
	~ChannelBase() { m_Lock.lock(); m_Lock.unlock(); };

	/**
	 * Connect to a channel.
	 * @param c channel
	 */
	void Connect(ChannelBase* c);

	/**
	 * Disconnect a channel.
	 * @param c channel
	 */
	void Disconnect(ChannelBase* c);

	/**
	 * Input sample at certain channel.
	 * @param c channel
	 * @param s sample
	 */
	virtual void Input(float s, int c);

	/**
	 * Process the current samples. Also call this when overriding!
	 */
	virtual void Process();

	/**
	 * Get the channel id.
	 * @return id
	 */
	int Id() const { return m_Id; }

	/**
	 * Get the amount of lines in this channel.
	 * @return lines
	 */
	int Lines() const { return m_Lines; }

	/**
	 * Get the type of this channel.
	 * @return type
	 */
	auto Type() const -> ChannelType { return m_Type; }

	/**
	 * Get the effect chain of this channel.
	 * @return effect chain
	 */
	auto EffectChain() -> ::EffectChain& { return m_EffectChain; }

	/**
	 * Get all the connections this channel has.
	 * @return connections
	 */
	auto Connections() -> std::vector<ChannelBase*>& { return m_Connections; }
	
	/**
	 * Returns true if the given ChannelBase is connected to this ChannelBase.
	 * @param c channel
	 * @return true when connected
	 */
	bool Connected(ChannelBase* c) const;

	/**
	 * Set how many lines are in this channel. Reallocates buffers.
	 * @param c lines amount
	 */
	void Lines(int c);

	void Update(const Vec4<int>& v);

	virtual operator nlohmann::json() = 0;
	virtual void operator=(const nlohmann::json&) = 0;

	Effects::VolumeSlider volumeParam;
	Effects::Parameter panParam{ "Pan", Effects::ParameterType::Slider };
	VolumeSlider& volume;
	PanSlider& pan;
	SMXRTextComponent& name, &volumeVal;
	Button<MuteButton, ButtonType::Toggle>& mute;
	Button<MonoButton, ButtonType::Toggle>& mono;
	Button<RouteButton, ButtonType::Toggle>& route;

	// Static selected channel, so all channels have access to it.
	static inline ChannelBase* selected = nullptr;

protected:
	int m_Id = 0,
		m_Counter = 0,
		m_Lines = 0;

	const ChannelType m_Type;

	std::vector<ChannelBase*> 
		m_Connections;
	
	std::vector<float> 
		m_Levels,
		m_Peaks,
		m_Pans;

	::EffectChain m_EffectChain;

	mutable std::mutex m_Lock;

	ChannelBase* m_Prev = nullptr;

	/**
	 * Update the pans for each line in this channel, so we don't have to calculate them
	 * each call of Process.
	 */
	void UpdatePans();

	friend class Audio;
};