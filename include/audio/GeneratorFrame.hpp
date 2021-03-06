#pragma once
#include "ui/Frame.hpp"
#include "audio/Generator.hpp"


class GeneratorFrame : public SoundMixrFrame
{
public:
	GeneratorFrame(SoundMixr::GeneratorBase* generator);
	virtual void Update(const Vec4<int>&) override;

	SoundMixr::GeneratorBase& Generator() { return *m_Generator.generator; }

	void Loop() override
	{
		m_Generator.UpdatePlugin();
		Window::Loop();
	}

	::Generator& m_Generator;
};