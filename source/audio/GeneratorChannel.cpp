#include "audio/GeneratorChannel.hpp"
#include "Controller.hpp"

GeneratorChannel::GeneratorChannel(SoundMixr::GeneratorBase* generator)
	: m_Generator(Controller::Get().m_Gui.AddWindow<GeneratorFrame>(generator)), ChannelBase(ChannelBase::Type::Input | ChannelBase::Type::Generator)
{
	m_EffectChain = std::make_unique<::GeneratorChain>();
	name.Content("Generator");
	Lines(2);
	m_Generator.Generator().Channels(2);
}

void GeneratorChannel::Process() 
{
	std::lock_guard<std::mutex> _{ m_Lock };

	if (!mute.Active())
	{
		m_Levels[0] = m_Generator.Generator().Generate(0);
		m_Levels[1] = m_Generator.Generator().Generate(1);
	}

	// Process main channel things
	ChannelBase::Process();

	// Reset levels
	for (auto& i : m_Levels)
		i = 0;
}