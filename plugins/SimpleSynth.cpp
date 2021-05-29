#define GENERATOR_PLUGIN
#include "Base.hpp"
#include "Oscillator.hpp"

namespace SoundMixr
{

	class SimpleSynth : public GeneratorBase
	{
	public:
		SimpleSynth()
			: GeneratorBase("SimpleSynth")
		{
			voices.WaveTable(Wavetables::Saw);
		}

		float val = 0;
		float Generate(int c) override
		{
			if (c == 0)
			{
				val = voices.Process();
			}

			return val;
		}

		void ReceiveMidi(MidiData data)
		{
			if (data.type == MidiData::Type::NoteOn)
				voices.NotePress(data.noteon.note);
			if (data.type == MidiData::Type::NoteOff)
				voices.NoteRelease(data.noteoff.note);
		}

		VoiceBank<Oscillator> voices{ 64 };
	};



}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::SimpleSynth;
}