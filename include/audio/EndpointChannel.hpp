#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/AsioDevice.hpp"


class EndpointChannel : public ChannelBase
{
public:
	EndpointChannel(ChannelType type)
		: ChannelBase(type)
	{}

	/**
	 * Processing for the endpoint channel happens as follows:
	 *  - Get the levels from the endpoints
	 *  - Apply effectchain, panning, volume, mute, and mono.
	 *  - Forward to all connections.
	 */
	virtual void Process() override
	{
		// Input takes sample from endpoint and sends to connections
		if (Type() == ChannelBase::ChannelType::Input)
			for (int i = 0; i < m_Lines; i++)
			{
				int _sample = m_Endpoints[i]->sample;
			
				_sample = m_EffectChain.Apply(_sample);
				_sample *= volume.Value();

				// TODO: Apply pan, mute, mono, levels for visuals.

				for (auto& j : Connections())
					j->Input(i, _sample);
			}
		
		// Otherwise it's output and takes samples from m_Levels and sends to endpoints
		else
			for (int i = 0; i < m_Lines; i++)
			{
				int _sample = m_Levels[i];

				_sample = m_EffectChain.Apply(_sample);
				_sample *= volume.Value();

				// TODO: Apply pan, mute, mono, levels for visuals.

				m_Endpoints[i]->sample = _sample;
			}
		
		// Reset levels
		for (auto& i : m_Levels)
			i = 0;
	}


	/**
	 * Add an endpoint to this channel. Increments the line 
	 * count of this Channel as well.
	 * @param e endpoint
	 */
	void AddEndpoint(Endpoint* e) 
	{ 
		if (!std::contains(m_Endpoints, e))
			m_Endpoints.push_back(e), Lines(Lines() + 1);
	};

	/**
	 * Remove an endpoint from this channel. Decrements the line
	 * count of this Channel as well.
	 * @param e endpoint
	 */
	void RemoveEndpoint(Endpoint* e)
	{
		auto& it = std::find(m_Endpoints.begin(), m_Endpoints.end(), e);
		if (it != m_Endpoints.end())
			m_Endpoints.erase(it), Lines(Lines() - 1);
	};

	virtual operator nlohmann::json() override
	{
		nlohmann::json _json = nlohmann::json::object();
		return _json;
	};

	virtual void operator=(const nlohmann::json& json) override
	{
	
	};

private:
	std::vector<Endpoint*> m_Endpoints;
};