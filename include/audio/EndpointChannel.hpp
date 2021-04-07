#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/AsioDevice.hpp"


class EndpointChannel : public ChannelBase
{
public:
	using ChannelBase::ChannelBase;

	/**
	 * Processing for the endpoint channel happens as follows:
	 *  - Get the levels from the endpoints
	 *  - Apply effectchain, panning, volume, mute, and mono.
	 *  - Forward to all connections.
	 */
	virtual void Process() override
	{
		m_Lock.lock();

		// Input takes sample from endpoint and sends to connections
		if (Type() & ChannelBase::Type::Input)
			for (int i = 0; i < m_Lines; i++)
			{
				float _sample = m_Endpoints[i]->sample;
			
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
				float _sample = m_Levels[i];

				_sample = m_EffectChain.Apply(_sample);
				_sample *= volume.Value();

				// TODO: Apply pan, mute, mono, levels for visuals.

				m_Endpoints[i]->sample = _sample;
			}
		
		// Reset levels
		for (auto& i : m_Levels)
			i = 0;

		m_Lock.unlock();
	}


	/**
	 * Add an endpoint to this channel. Increments the line 
	 * count of this Channel as well.
	 * @param e endpoint
	 */
	void AddEndpoint(Endpoint* e) 
	{ 
		// If first endpoint, set name of channel.
		if (m_Endpoints.size() == 0 && e)
			name.Content(e->name), m_Id = e->id;
		
		// Add if not already added.
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

	/**
	 * Saving operator, converts this channel to json.
	 * @return json
	 */
	virtual operator nlohmann::json() override
	{
		nlohmann::json _json = m_EffectChain;
		_json["id"] = Id();
		_json["volume"] = volume.Value();
		_json["muted"] = mute.Active();
		_json["mono"] = mono.Active();
		_json["pan"] = pan.Value();

		if (Type() & Type::Input)
		{
			std::vector<int> _connections{};
			for (auto& i : m_Connections)
				_connections.push_back(i->Id());

			_json["connections"] = _connections;
		}

		std::vector<int> _channels{};
		for (auto& i : m_Endpoints)
			_channels.push_back(i->id);

		_json["channels"] = _channels;

		return _json;
	};

	/**
	 * Loading operator, sets this channel's state using json.
	 * @param json
	 */
	virtual void operator=(const nlohmann::json& json) override
	{
		mono.Active(json.at("mono").get<bool>());
		mute.Active(json.at("muted").get<bool>());
		pan.Value(json.at("pan").get<double>());
		volume.Value(json.at("volume").get<double>());
		m_EffectChain = json;
	};

private:
	std::vector<Endpoint*> m_Endpoints;
};