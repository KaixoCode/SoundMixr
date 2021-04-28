#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"
#include <ui/Soundboard.hpp>

class SoundboardChannel : public ChannelBase
{
public:
	SoundboardChannel(Soundboard& soundboard)
		: ChannelBase(ChannelBase::Type::Input & ChannelBase::Type::SoundBoard),
		m_Soundboard(soundboard)
	{
		Lines(2);
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

		float _avg = 0;
		bool _mono = mono.Active();

		m_Levels[0] = m_Soundboard.GetLevel(0);
		m_Levels[1] = m_Soundboard.GetLevel(1);


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
				for (auto& c : m_Connections)
					c->Input(_sample, i);

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
				for (auto& c : m_Connections)
					c->Input(_level, i);


				// Set peak, for the volume slider meter.
				if (_level > m_Peaks[i])
					m_Peaks[i] = _level;
				if (-_level > m_Peaks[i])
					m_Peaks[i] = -_level;
			}
		}


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