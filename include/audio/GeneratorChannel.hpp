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

		float _generatedSample = 0;
		if (!mute.Active())
			_generatedSample = m_Oscillator.NextSample();

		m_Levels[0] = _generatedSample;
		m_Levels[1] = _generatedSample;

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