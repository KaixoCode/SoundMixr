#include "audio/EndpointChannel.hpp"

void EndpointChannel::Process()
{
	m_Lock.lock();

	// Input takes sample from endpoint and sends to connections
	if (Type() & ChannelBase::Type::Input)
	{
		float _avg = 0;
		bool _mono = mono.Active();

		// Go through all the input endpoints and collect samples
		for (int i = 0; i < m_Lines; i++)
		{
			float _sample = m_Endpoints[i]->sample;

			// If muted set sample to 0
			if (!mute.Active())
			{
				_sample = m_EffectChain.NextSample(_sample, i);
				_sample *= volume.Value();
				_sample *= m_Pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly add to all connections
			if (!_mono)
			{
				for (auto& j : Connections())
					j->Input(_sample, i);

				// Set peak, for the volume slider meter.
				if (_sample > m_Peaks[i])
					m_Peaks[i] = _sample;
				if (-_sample > m_Peaks[i])
					m_Peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= Lines();

		// If mono, send the avg sample to all lines of all connected channels
		if (_mono)
		{
			// The connections will get averaged levels, but panning will still be applied
			// panning will be applied as if there were the same amount of channels (n), but 
			// any channel above n will receive the pan of x mod n channel. So example:
			// this has 2 channels, connection 4, channel 3 of connection will get pan of channel 1.
			for (auto& j : Connections())
				for (int i = 0; i < j->Lines(); i++)
				{
					float _level = _avg * m_Pans[i % Lines()];
					j->Input(_level, i);
				}

			// Set peaks for volume slider meter to mono, panning will be applied after monoing.
			for (int i = 0; i < Lines(); i++)
			{
				float _level = _avg * m_Pans[i];

				// Set peak, for the volume slider meter.
				if (_level > m_Peaks[i])
					m_Peaks[i] = _level;
				if (-_level > m_Peaks[i])
					m_Peaks[i] = -_level;
			}
		}
	}

	// Otherwise it's output and takes samples from m_Levels and sends to endpoints
	else
	{
		float _avg = 0;
		bool _mono = mono.Active();

		// Go through all the output endpoints and submit samples
		for (int i = 0; i < m_Lines; i++)
		{
			float _sample = m_Levels[i];

			// If muted set sample to 0
			if (!mute.Active())
			{
				_sample = m_EffectChain.NextSample(_sample, i);
				_sample *= volume.Value();
				if (!_mono)
					_sample *= m_Pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly send to endpoints
			if (!_mono)
			{
				m_Endpoints[i]->sample = _sample;

				// Set peak, for the volume slider meter.
				if (_sample > m_Peaks[i])
					m_Peaks[i] = _sample;
				if (-_sample > m_Peaks[i])
					m_Peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= Lines();

		// If mono, send the avg sample to all endpoints
		if (_mono)
		{
			for (int i = 0; i < Lines(); i++)
			{
				float _level = _avg * m_Pans[i];
				m_Endpoints[i]->sample = _level;

				// Set peak, for the volume slider meter.
				if (_level > m_Peaks[i])
					m_Peaks[i] = _level;
				if (-_level > m_Peaks[i])
					m_Peaks[i] = -_level;
			}
		}
	}

	// Process main channel things
	ChannelBase::Process();

	// Reset levels
	for (auto& i : m_Levels)
		i = 0;

	m_Lock.unlock();
}

void EndpointChannel::AddEndpoint(Endpoint* e)
{
	// If first endpoint, set name of channel.
	if (m_Endpoints.size() == 0 && e)
		name.Content(e->name), m_Id = e->id;

	// Add if not already added.
	if (!std::contains(m_Endpoints, e))
		m_Endpoints.push_back(e), Lines(Lines() + 1), SortEndpoints();
};

void EndpointChannel::RemoveEndpoint(Endpoint* e)
{
	auto& it = std::find(m_Endpoints.begin(), m_Endpoints.end(), e);
	if (it != m_Endpoints.end())
		m_Endpoints.erase(it), Lines(Lines() - 1), SortEndpoints();
};

void EndpointChannel::SortEndpoints()
{
	std::sort(m_Endpoints.begin(), m_Endpoints.end(),
		[](Endpoint* a, Endpoint* b) -> bool
		{
			return a->id < b->id;
		});

	CalcWidth();
}

void EndpointChannel::CalcWidth()
{
	if (Lines() <= 4)
		Width(70);
	else
		Width(Lines() * 7 + 42);
}

EndpointChannel::operator nlohmann::json()
{
	nlohmann::json _json = m_EffectChain;
	_json["id"] = Id();
	_json["volume"] = volume;
	_json["muted"] = mute.Active();
	_json["mono"] = mono.Active();
	_json["pan"] = pan;
	_json["name"] = name.Content();

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

void EndpointChannel::operator=(const nlohmann::json& json) 
{
	mono.Active(json.at("mono").get<bool>());
	mute.Active(json.at("muted").get<bool>());
	pan = json.at("pan");
	volume = json.at("volume");
	name.Content(json.at("name").get<std::string>());
	m_EffectChain = json;
};