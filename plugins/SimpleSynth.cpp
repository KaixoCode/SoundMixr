#define GENERATOR_PLUGIN
#include "Base.hpp"
#include "Oscillator.hpp"

namespace SoundMixr
{

	class SimpleSynth : public GeneratorBase
	{
	public:

		SimpleSynth()
			: GeneratorBase("SimpleSynth"),
			freq(Parameter("Freq", ParameterType::Knob))
		{
			freq.Range({ 10, 22000 });
			freq.Log(10);
			freq.ResetValue(22000);
			freq.ResetValue();
			freq.Unit("Hz");
			freq.Unit("kHz", 3);
			freq.Size({ 30, 30 });
			freq.Multiplier(0.4);

			voices.WaveTable(Wavetables::Saw);
			Width(400);
			Height(500);

			Div() = {};
		}

		float val = 0;
		float Generate(int c) override
		{
			if (c == 0)
			{
				val = eq.Apply(voices.Process(), params);
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

		SoundMixr::Parameter& freq;
		VoiceBank<Oscillator> voices{ 64 };
		BiquadParameters params;
		BiquadFilter<> eq;
	};



}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::SimpleSynth;
}