#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

/*
 * TODO:
 *  - Add GeneratorBase repo
 *  - Generator base class pointer in this channel.
 *  - GeneratorLoader singleton, just like PluginLoader
 */

/**
 * Little test generator channel. TODO
 */
class GeneratorChannel : public ChannelBase
{
public:

	GeneratorChannel()
		: ChannelBase(ChannelBase::Type::Input | ChannelBase::Type::Generator)
	{
		name.Content("Generator");
		Lines(2);

		m_Oscillator.WaveTable(Wavetables::Saw);
	}

	virtual void Process() override
	{
		std::lock_guard<std::mutex> _{ m_Lock };

		float _generatedSample = 0;
		if (!mute.Active())
			_generatedSample = m_Oscillator.Process();

		m_Levels[0] = _generatedSample;
		m_Levels[1] = _generatedSample;

		// Process main channel things
		ChannelBase::Process();

		// Reset levels
		for (auto& i : m_Levels)
			i = 0;
	};

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