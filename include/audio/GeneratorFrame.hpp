#pragma once
#include "ui/Frame.hpp"
#include "audio/Generator.hpp"


class GeneratorFrame : public SoundMixrFrame
{
public:
	GeneratorFrame(SoundMixr::GeneratorBase* generator);

	SoundMixr::GeneratorBase& Generator() { return *m_Generator.generator; }

	::Generator& m_Generator;
};