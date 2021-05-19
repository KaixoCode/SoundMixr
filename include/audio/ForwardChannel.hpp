#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

class ForwardChannel : public ChannelBase
{
public:
	ForwardChannel()
		: ChannelBase(ChannelBase::Type::Forward)
	{
		name.Content("Forward");
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



		// Process main channel things
		ChannelBase::Process();

		// Reset levels
		for (auto& i : m_Levels)
			i = 0;

		m_Lock.unlock();
	};

	virtual operator nlohmann::json() override { return {}; };
	virtual void operator=(const nlohmann::json& json) override {};
};