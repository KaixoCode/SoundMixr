#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"
#include "ui/Soundboard.hpp"

/**
 * The Soundboard channel. Simply gets the levels from the soundboard.
 */
class SoundboardChannel : public ChannelBase
{
public:

	/**
	 * Constructor.
	 */
	SoundboardChannel();

	virtual void Process() override;
};