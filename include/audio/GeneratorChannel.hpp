#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

/**
 * Endpoint channel, for input and outputs provided by the opened asio device,
 * a channel contains 1 or more endpoints, the endpoints are always sorted on their
 * ids.
 */
class GeneratorChannel : public ChannelBase
{
public:

	GeneratorChannel(ChannelType type)
		: ChannelBase(type)
	{
		name.Content("Generator");
		Lines(2);

		m_Oscillator.WaveTable(Wavetables::Saw);
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

		float _generatedSample = 0;
		if (!mute.Active())
			_generatedSample = m_Oscillator.NextSample();

		// Go through all the output endpoints and submit samples
		for (int i = 0; i < m_Lines; i++)
		{
			float _sample = _generatedSample;
			m_Levels[i] = _sample;

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
	VoiceBank<Oscillator> m_Oscillator{ 64 };

	Midi::EventStorage _1{ Midi::Get() += [this](Midi::Event::NoteOn& e)
	{
		m_Oscillator.NotePress(e.note);
	} };

	Midi::EventStorage _2{ Midi::Get() += [this](Midi::Event::NoteOff& e)
	{
		m_Oscillator.NoteRelease(e.note);
	} };
};