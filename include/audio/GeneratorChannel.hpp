#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/GeneratorChain.hpp"
#include "audio/Asio.hpp"
#include "audio/GeneratorFrame.hpp"

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
	GeneratorChannel(SoundMixr::GeneratorBase* generator);

	virtual void Process() override;

private:
	GeneratorFrame& m_Generator;

	Midi::EventStorage _1{ Midi::Get() += [this](Midi::Event&e)
	{
		m_Generator.Generator().ReceiveMidi({ (int)e.type, e.data });
	} };
};