#pragma once
#include "pch.hpp"

struct ChannelBase
{
	struct Type
	{
		enum
		{
			Input = 0x1,
			Output = 0x2,
			Generator = 0x4,
			SoundBoard = 0x8,
			Endpoint = 0x10,
			Forward = 0x20,
		};
	};

	ChannelBase(int type) : type(type) { UpdatePans(); };
	~ChannelBase() { lock.lock(); lock.unlock(); };

	int counter = 0;
	int id = 0;
	int lines = 0;
	bool mono = false;
	bool mute = false;
	float pan = 0;
	float volume = 1;
	const int type;
	std::vector<Pointer<ChannelBase>> connections;
	mutable std::mutex lock;
	std::vector<float> levels;
	std::vector<float> peaks;
	std::vector<float> smoothed;
	std::vector<float> pans;

	virtual void Level(float s, int c)
	{
		if (lines == 0)
			return;

		std::lock_guard<std::mutex> _{ lock };
		levels[c % lines] += s;
	}

	virtual void NextCycle()
	{
		// Reset levels
		for (auto& i : levels)
			i = 0;
	}

	virtual void Process()
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

	void Connect(const Pointer<ChannelBase>& c)
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
			std::lock_guard<std::mutex> _{ lock };
			connections.push_back(c);
		}
	}

	void Disconnect(const Pointer<ChannelBase>& c)
	{
		// If connected, disconnect. 
		auto it = std::find(connections.begin(), connections.end(), c);
		if (it != connections.end())
		{
			std::lock_guard<std::mutex> _{ lock };
			connections.erase(it);
		}
	}

	bool Connected(const Pointer<ChannelBase>& c) const
	{
		// If it's an input it stores the connections
		if (type & Type::Input)
			return std::find(connections.begin(), connections.end(), c) != connections.end();

		// Otherwise it's an output, so check if param is input and is connected to this
		else if (c && (c->type & Type::Input))
			return std::find_if(c->connections.begin(), c->connections.end(), [this](const auto& a) { return &a == this; }) != c->connections.end();
	}

	void UpdatePans()
	{
		std::lock_guard<std::mutex> _{ lock };

		pans.reserve(lines);
		while (pans.size() < lines)
			pans.push_back(0);

		double _a = 1.0 - std::abs((lines - 1) / 2.0 * pan);
		for (int i = 0; i < lines; i++)
			pans[i] = constrain(pan * (i - (lines - 1) / 2.0) + _a, 0.0, 1.0);
	}
};

struct Endpoint
{
	const std::string name;
	int id;
	bool input;
	float sample = 0;
};

struct EndpointChannel : public ChannelBase
{
	std::vector<Pointer<Endpoint>> endpoints;

	EndpointChannel(bool input)
		: ChannelBase(Type::Endpoint | (input ? Type::Input : Type::Output))
	{}

	void NextCycle() override
	{
		if (type & ChannelBase::Type::Output)
			for (int i = 0; i < lines; i++)
				endpoints[i]->sample = 0;

		ChannelBase::NextCycle();
	}

	void Process() override
	{
		// Optimization when 0 lines
		if (lines == 0)
			return;

		std::lock_guard<std::mutex> _{ lock };

		// Input takes sample from endpoint and sends to connections
		if (type & ChannelBase::Type::Input)
		{
			// First get all levels from the endpoints
			for (int i = 0; i < lines; i++)
				levels[i] = endpoints[i]->sample;

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
				endpoints[i]->sample += levels[i];
		}
	}

	void Add(const Pointer<Endpoint>& e)
	{
		// Add if not already added.
		if (!contains(endpoints, e))
		{
			std::lock_guard<std::mutex> _{ lock };

			// Add and sort with new endpoint.
			endpoints.push_back(e);
			lines = lines + 1;
			levels.push_back(0); // Add new levels entry
			peaks.push_back(0); // Add new peaks entry
			smoothed.push_back(0); // Add new pans entry
			pans.push_back(1); // Add new pans entry

			// if Endpoint added, set id to first endpoint.
			id = endpoints[0]->id;
		}
		UpdatePans();
	};

	void Remove(const Pointer<Endpoint>& e)
	{
		auto it = std::find(endpoints.begin(), endpoints.end(), e);
		if (it != endpoints.end())
		{
			std::lock_guard<std::mutex> _{ lock };

			// If not added, add endpoint.
			endpoints.erase(it);
			lines = lines - 1;

			// if Endpoint removed, reset if to first endpoint.
			if (endpoints.size() != 0)
				id = endpoints[0]->id;
		}
	};

	bool Contains(const Pointer<Endpoint>& e)
	{
		return std::find(endpoints.begin(), endpoints.end(), e) != endpoints.end();
	}
};