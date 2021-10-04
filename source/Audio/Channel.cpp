#include "Audio/Channel.hpp"
#include "Controller.hpp"

ChannelBase::ChannelBase(int type) 
	: type(type) 
{
	UpdatePans(); 
}

ChannelBase::~ChannelBase() 
{ 
	Controller::Get().audio.lock.lock(); Controller::Get().audio.lock.unlock();
}

void ChannelBase::Level(float s, int c)
{
	if (lines == 0)
		return;

	levels[c % lines] += s;
}

void ChannelBase::NextCycle()
{
	// Reset levels
	for (auto& i : levels)
		i = 0;
}

void ChannelBase::Process()
{
	float _avg = 0;

	// Input
	if (type & ChannelBase::Type::Input || type & ChannelBase::Type::Forward)
	{
		// Go through all the lines 
		for (int i = 0; i < lines; i++)
		{
			float _sample = levels[i];

			// If muted set sample to 0
			if (!mute)
			{
				//_sample = m_EffectChain->Process(_sample, i);
				_sample *= volume;
				_sample *= pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly add to all connections
			if (!mono)
			{
				for (auto& j : connections)
					j->Level(_sample, i);

				// Set peak, for the volume slider meter.
				if (_sample > peaks[i]) peaks[i] = _sample;
				if (-_sample > peaks[i]) peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= lines;

		// If mono, send the avg sample to all lines of all connected channels
		if (mono)
		{
			// The connections will get averaged levels, but panning will still be applied
			// panning will be applied as if there were the same amount of channels (n), but 
			// any channel above n will receive the pan of x mod n channel. So example:
			// this has 2 channels, connection 4, channel 3 of connection will get pan of channel 1.
			for (auto& j : connections)
				for (int i = 0; i < j->lines; i++)
				{
					float _level = _avg * pans[i % lines];
					j->Level(_level, i);
				}

			// Set peaks for volume slider meter to mono, panning will be applied after monoing.
			for (int i = 0; i < lines; i++)
			{
				float _level = _avg * pans[i];

				// Set peak, for the volume slider meter.
				if (_level > peaks[i]) peaks[i] = _level;
				if (-_level > peaks[i]) peaks[i] = -_level;
			}
		}
	}

	// Otherwise it's output
	if (type & ChannelBase::Type::Output || type & ChannelBase::Type::Forward)
	{
		float _avg = 0;

		// Go through all the lines
		for (int i = 0; i < lines; i++)
		{
			float _sample = levels[i];

			// If muted set sample to 0
			if (!mute)
			{
				//_sample = effectChain->Process(_sample, i);
				_sample *= volume;
				if (!mono)
					_sample *= pans[i];
			}
			else
				_sample = 0;

			// If it's not mono, directly send to endpoints
			if (!mono)
			{
				levels[i] = constrain(_sample, -1, 1);

				// Set peak, for the volume slider meter.
				if (_sample > peaks[i]) peaks[i] = _sample;
				if (-_sample > peaks[i]) peaks[i] = -_sample;
			}

			// Increase average if mono
			else
				_avg += _sample;
		}

		// Actually make it an average.
		_avg /= lines;

		// If mono, send the avg sample to all endpoints
		if (mono)
		{
			for (int i = 0; i < lines; i++)
			{
				float _level = _avg * pans[i];
				levels[i] = constrain(_level, -1, 1);

				// Set peak, for the volume slider meter.
				if (_level > peaks[i]) peaks[i] = _level;
				if (-_level > peaks[i]) peaks[i] = -_level;
			}
		}
	}

	// Every 512 samples, set the value for the volume slider meter.
	counter++;
	if (counter > 512)
	{
		// Set the levels of the volume slider
		for (int i = 0; i < lines; i++)
		{
			float r = smoothed[i];
			smoothed[i] = r * 0.8 + 0.2 * peaks[i];
			peaks[i] = 0;
		}

		counter = 0;
	}
}

void ChannelBase::Connect(const Pointer<ChannelBase>& c)
{
	// If connecting an output channel to a channel, something
	// went wrong, since only input channels store the connection
	// to an output channel.
	if (type & Type::Output)
	{
		CrashLog("Connect was called on an output channel");
	}

	// If not connected already, connect.
	if (!contains(connections, c))
	{
		std::lock_guard<std::mutex> _{ Controller::Get().audio.lock };
		connections.push_back(c);
	}
}

void ChannelBase::Disconnect(const Pointer<ChannelBase>& c)
{
	// If connected, disconnect. 
	auto it = std::find(connections.begin(), connections.end(), c);
	if (it != connections.end())
	{
		std::lock_guard<std::mutex> _{ Controller::Get().audio.lock };
		connections.erase(it);
	}
}

bool ChannelBase::Connected(const Pointer<ChannelBase>& c) const
{
	// If it's an input it stores the connections
	if (type & Type::Input)
		return std::find(connections.begin(), connections.end(), c) != connections.end();

	// Otherwise it's an output, so check if param is input and is connected to this
	else if (c && (c->type & Type::Input))
		return std::find_if(c->connections.begin(), c->connections.end(), [this](const auto& a) { return &a == this; }) != c->connections.end();
}

void ChannelBase::UpdatePans()
{
	std::lock_guard<std::mutex> _{ Controller::Get().audio.lock };

	pans.reserve(lines);
	while (pans.size() < lines)
		pans.push_back(0);

	double _a = 1.0 - std::abs((lines - 1) / 2.0 * pan);
	for (int i = 0; i < lines; i++)
		pans[i] = constrain(pan * (i - (lines - 1) / 2.0) + _a, 0.0, 1.0);
}

ChannelBase::operator nlohmann::json()
{
	nlohmann::json _json;
	_json["id"] = id;
	_json["type"] = type;
	_json["mono"] = mono;
	_json["mute"] = mute;
	_json["pan"] = pan;
	_json["volume"] = volume;
	_json["name"] = name;

	return _json;
}

void ChannelBase::operator=(const nlohmann::json& json)
{
	id = json.at("id").get<int>();
	mono = json.at("mono").get<bool>();
	mute = json.at("mute").get<bool>();
	pan = json.at("pan").get<float>();
	volume = json.at("volume").get<float>();
	name = json.at("name").get<std::string>();
}

EndpointChannel::operator nlohmann::json()
{
	nlohmann::json _json = ChannelBase::operator nlohmann::json();
	_json["endpoints"] = nlohmann::json::array();
	for (auto& i : endpoints)
		_json["endpoints"].push_back(i);

	if (type & Type::Input)
	{
		_json["connections"] = nlohmann::json::array();
		for (auto& i : connections)
			_json["connections"].push_back(i->id);
	}
	return _json;
}

void EndpointChannel::operator=(const nlohmann::json& json)
{
	ChannelBase::operator=(json);
	for (auto& i : json["endpoints"])
		Add(i);
}

EndpointChannel::EndpointChannel(bool input)
	: ChannelBase(Type::Endpoint | (input ? Type::Input : Type::Output))
{}

void EndpointChannel::NextCycle()
{
	if (type & ChannelBase::Type::Output)
		for (int i = 0; i < lines; i++)
			if (endpoints[i] < Controller::Get().audio.outputs.size())
				Controller::Get().audio.outputs[endpoints[i]]->sample = 0;

	ChannelBase::NextCycle();
}

void EndpointChannel::Process()
{
	// Optimization when 0 lines
	if (lines == 0)
		return;

	// Input takes sample from endpoint and sends to connections
	if (type & ChannelBase::Type::Input)
	{
		// First get all levels from the endpoints
		for (int i = 0; i < lines; i++)
			if (endpoints[i] < Controller::Get().audio.inputs.size())
				levels[i] = Controller::Get().audio.inputs[endpoints[i]]->sample;

		// Then process the samples
		ChannelBase::Process();
	}

	// Otherwise it's output and takes samples from m_Levels and sends to endpoints
	else
	{
		// First process the incoming samples
		ChannelBase::Process();

		// Then forward them to the endpoints
		for (int i = 0; i < lines; i++)
			if (endpoints[i] < Controller::Get().audio.outputs.size())
				Controller::Get().audio.outputs[endpoints[i]]->sample += levels[i];
	}
}

void EndpointChannel::Add(int id)
{
	// Add if not already added.
	if (!contains(endpoints, id))
	{
		std::lock_guard<std::mutex> _{ Controller::Get().audio.lock };

		// Add and sort with new endpoint.
		if (endpoints.size() == 0 && name.empty())
		{
			if (type & ChannelBase::Type::Input)
			{
				if (id < Controller::Get().audio.inputs.size())
					name = Controller::Get().audio.inputs[id]->name;
			}
			else
				if (id < Controller::Get().audio.outputs.size())
					name = Controller::Get().audio.outputs[id]->name;
		}
		endpoints.push_back(id);
		lines = lines + 1;
		levels.push_back(0); // Add new levels entry
		peaks.push_back(0); // Add new peaks entry
		smoothed.push_back(0); // Add new pans entry
		pans.push_back(1); // Add new pans entry
	}
	UpdatePans();
};

void EndpointChannel::Remove(int id)
{
	auto it = std::find(endpoints.begin(), endpoints.end(), id);
	if (it != endpoints.end())
	{
		std::lock_guard<std::mutex> _{ Controller::Get().audio.lock };

		// If not added, add endpoint.
		endpoints.erase(it);
		lines = lines - 1;
	}
};

bool EndpointChannel::Contains(int id)
{
	return std::find(endpoints.begin(), endpoints.end(), id) != endpoints.end();
}