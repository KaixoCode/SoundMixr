#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"
#include <ui/Soundboard.hpp>

class SoundboardChannel : public ChannelBase
{
public:
	SoundboardChannel(Soundboard& soundboard)
		: ChannelBase(ChannelBase::Type::Input | ChannelBase::Type::SoundBoard | ChannelBase::Type::Generator),
		m_Soundboard(soundboard)
	{
		Hide();
		Lines(2);
		name.Content("Soundboard");
	}

	/**
	 * Processing for the endpoint channel happens as follows:
	 *  - Get the levels from the endpoints
	 *  - Apply effectchain, panning, volume, mute, and mono.
	 *  - Forward to all connections.
	 */
	virtual void Process() override
	{
		m_Lock.lock();

		m_Levels[0] = m_Soundboard.GetLevel(0);
		m_Levels[1] = m_Soundboard.GetLevel(1);

		// Process main channel things
		ChannelBase::Process();
		
		// Reset levels
		for (auto& i : m_Levels)
			i = 0;

		m_Lock.unlock();
	};

	virtual operator nlohmann::json() override { return {}; };
	virtual void operator=(const nlohmann::json& json) override {};

private:
	Soundboard& m_Soundboard;
};