#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"
#include "ui/Soundboard.hpp"

/**
 * The Soundboard channel. Simply gets the levels from the soundboard.
 */
class SoundboardChannel : public ChannelBase
{
public:

	/**
	 * Constructor.
	 */
	SoundboardChannel()
		: ChannelBase(ChannelBase::Type::Input | ChannelBase::Type::SoundBoard | ChannelBase::Type::Generator)
	{
		// Always has 2 lines
		Lines(2);
		name.Content("Soundboard");

		// Id of the soundboard is -1
		m_Id = -1;
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

		if (Soundboard::Get())
		{
			m_Levels[0] = Soundboard::Get().GetLevel(0);
			m_Levels[1] = Soundboard::Get().GetLevel(1);
		}

		// Process main channel things
		ChannelBase::Process();
		
		// Reset levels
		for (auto& i : m_Levels)
			i = 0;

		m_Lock.unlock();
	};
};