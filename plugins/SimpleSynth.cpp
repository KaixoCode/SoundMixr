#define GENERATOR_PLUGIN
#include "Base.hpp"

namespace SoundMixr
{

	class SimpleSynth : public GeneratorBase
	{
	public:
		SimpleSynth()
			: GeneratorBase("SimpleSynth")
		{}

		float Generate(int c) override
		{
			return 0;
		}


	};



}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::SimpleSynth;
}