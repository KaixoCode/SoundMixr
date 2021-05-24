#include "audio/SoundboardChannel.hpp"

SoundboardChannel::SoundboardChannel()
	: ChannelBase(ChannelBase::Type::Input | ChannelBase::Type::SoundBoard)
{
	// Always has 2 lines
	Lines(2);
	name.Content("Soundboard");

	// Id of the soundboard is -1
	m_Id = -1;
}

void SoundboardChannel::Process()
{
	std::lock_guard<std::mutex> _{ m_Lock };

	if (Soundboard::Get())
	{
		m_Levels[0] = Soundboard::Get().GetLevel(0);
		m_Levels[1] = Soundboard::Get().GetLevel(1);
	}

	// Process main channel things
	ChannelBase::Process();

	// Reset levels
	for (auto& i : m_Levels)
		i = 0;
}