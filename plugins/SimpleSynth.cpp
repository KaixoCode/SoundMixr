#define GENERATOR_PLUGIN
#include "Base.hpp"
#include "Oscillator.hpp"

namespace SoundMixr
{

	class SimpleSynth : public GeneratorBase
	{
	public:

		class Voice : public SoundMixr::Voice
		{
		public:
			float Generate() override
			{
				double delta = (frequency * foffset) / sampleRate;
				phase = std::fmod(1 + phase + delta, 1);

				double envdata = env.Generate();
				double env2data = filterenv.Generate();

				params.f0 = filterfreq + env2data * filterenvamt;
				params.RecalculateParameters();



				return envdata * eq.Apply(wavetable(phase), params);
			}

			void Trigger() override
			{
				phase = 0;
				env.Trigger();
				filterenv.Trigger();
			}

			void Gate(bool g) override
			{
				env.Gate(g);
				filterenv.Gate(g);
			}

			void Frequency(double f)
			{
				frequency = f;
			}

			void Update()
			{
				env.sampleRate = sampleRate;
				filterenv.sampleRate = sampleRate;
				params.Q = 2;
				params.type = FilterType::LowPass;
			}

			bool Done() 
			{
				return env.Done();
			}

			double(*wavetable)(double) = Wavetables::Saw;

			BiquadParameters params;
			BiquadFilter<> eq;

			ADSR env;
			ADSR filterenv;

			double filterenvamt = 0.5;
			double phase = 0;
			double frequency = 0;
			double filterfreq = 0;
			double sampleRate = 48000;
			double foffset = 1;
		};


		SimpleSynth()
			: GeneratorBase("SimpleSynth"),
			freq(Parameter("Freq",   ParameterType::Knob)),
			famt(Parameter("Amount", ParameterType::Knob)),
			a(Parameter("Attack",    ParameterType::Slider)),
			ac(Parameter("Curve",    ParameterType::Slider)),
			d(Parameter("Decay",     ParameterType::Slider)),
			dc(Parameter("Curve",    ParameterType::Slider)),
			s(Parameter("Sustain",   ParameterType::Slider)),
			r(Parameter("Release",   ParameterType::Slider)),
			rc(Parameter("Curve",    ParameterType::Slider)),
			fa(Parameter("Attack",   ParameterType::Slider)),
			fac(Parameter("Curve",   ParameterType::Slider)),
			fd(Parameter("Decay",    ParameterType::Slider)),
			fdc(Parameter("Curve",   ParameterType::Slider)),
			fs(Parameter("Sustain",  ParameterType::Slider)),
			fr(Parameter("Release",  ParameterType::Slider)),
			frc(Parameter("Curve",   ParameterType::Slider))
		{
			freq.Range({ 10, 22000 });
			freq.Log(10);
			freq.ResetValue(22000);
			freq.ResetValue();
			freq.Unit("Hz");
			freq.Unit("kHz", 3);
			freq.Size({ 30, 30 });
			freq.Multiplier(0.4);

			famt.Range({ 10, 22000 });
			famt.Log(10);
			famt.ResetValue(22000);
			famt.ResetValue();
			famt.Unit("Hz");
			famt.Unit("kHz", 3);
			famt.Size({ 30, 30 });
			famt.Multiplier(0.4);

			a.Range({ 10, 2000 });
			a.Log(10);
			a.ResetValue(10);
			a.ResetValue();
			a.Unit("ms");
			a.Unit("s", 3);
			a.Size({ 48, 18 });		
			a.Vertical(false);

			ac.Range({ 0, 100 });
			ac.ResetValue(50);
			ac.ResetValue();
			ac.Unit("%");
			ac.Size({ 48, 18 });
			ac.Vertical(false);
			ac.DisplayName(false);

			d.Range({ 10, 2000 });
			d.Log(10);
			d.ResetValue(10);
			d.ResetValue();
			d.Unit("ms");
			d.Unit("s", 3);
			d.Size({ 48, 18 });
			d.Vertical(false);

			dc.Range({ 0, 100 });
			dc.ResetValue(50);
			dc.ResetValue();
			dc.Unit("%");
			dc.Size({ 48, 18 });
			dc.Vertical(false);
			dc.DisplayName(false);

			s.Range({ 0, 100 });
			s.ResetValue(50);
			s.ResetValue();
			s.Unit("%");
			s.Size({ 48, 18 });
			s.Vertical(false);

			r.Range({ 10, 2000 });
			r.Log(10);
			r.ResetValue(10);
			r.ResetValue();
			r.Unit("ms");
			r.Unit("s", 3);
			r.Size({ 48, 18 });
			r.Multiplier(0.4);
			r.Vertical(false);

			rc.Range({ 0, 100 });
			rc.ResetValue(50);
			rc.ResetValue();
			rc.Unit("%");
			rc.Size({ 48, 18 });
			rc.Vertical(false);
			rc.DisplayName(false);

			fa.Range({ 10, 2000 });
			fa.Log(10);
			fa.ResetValue(10);
			fa.ResetValue();
			fa.Unit("ms");
			fa.Unit("s", 3);
			fa.Size({ 48, 18 });
			fa.Vertical(false);
			
			fac.Range({ 0, 100 });
			fac.ResetValue(50);
			fac.ResetValue();
			fac.Unit("%");
			fac.Size({ 48, 18 });
			fac.Vertical(false);
			fac.DisplayName(false);

			fd.Range({ 10, 2000 });
			fd.Log(10);
			fd.ResetValue(10);
			fd.ResetValue();
			fd.Unit("ms");
			fd.Unit("s", 3);
			fd.Size({ 48, 18 });
			fd.Vertical(false);

			fdc.Range({ 0, 100 });
			fdc.ResetValue(50);
			fdc.ResetValue();
			fdc.Unit("%");
			fdc.Size({ 48, 18 });
			fdc.Vertical(false);
			fdc.DisplayName(false);

			fs.Range({ 0, 100 });
			fs.ResetValue(50);
			fs.ResetValue();
			fs.Unit("%");
			fs.Size({ 48, 18 });
			fs.Vertical(false);
			
			fr.Range({ 10, 2000 });
			fr.Log(10);
			fr.ResetValue(10);
			fr.ResetValue();
			fr.Unit("ms");
			fr.Unit("s", 3);
			fr.Size({ 48, 18 });
			fr.Vertical(false);

			frc.Range({ 0, 100 });
			frc.ResetValue(50);
			frc.ResetValue();
			frc.Unit("%");
			frc.Size({ 48, 18 });
			frc.Vertical(false);
			frc.DisplayName(false);

			Width(350);
			Height(166);

			Div() = { 2, Div::Alignment::Vertical, 4, false, Width() };
			Div()[1] = { 3, Div::Alignment::Horizontal, 0, false, 75 };
			Div()[1][0] = { freq, Div::Alignment::Center, 55 };
			Div()[1][1] = { famt, Div::Alignment::Center, 55 };
			Div()[1][2] = { 4, Div::Alignment::Horizontal, 0, false, Div::AUTO };
			Div()[1][2][0] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[1][2][0][1] = { fa, Div::Alignment::Bottom, 50 };
			Div()[1][2][0][0] = { fac, Div::Alignment::Center, 25 };
			Div()[1][2][1] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[1][2][1][1] = { fd, Div::Alignment::Bottom, 50 };
			Div()[1][2][1][0] = { fdc, Div::Alignment::Center, 25 };
			Div()[1][2][2] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[1][2][2][1] = { fs, Div::Alignment::Bottom, 50 };
			Div()[1][2][2][0].DivSize(25);
			Div()[1][2][3] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[1][2][3][1] = { fr, Div::Alignment::Bottom, 50 };
			Div()[1][2][3][0] = { frc, Div::Alignment::Center, 25 };
			Div()[0] = { 3, Div::Alignment::Horizontal, 0, false, 75 };
			Div()[0][0].DivSize(55);
			Div()[0][1].DivSize(55);
			Div()[0][2] = { 4, Div::Alignment::Horizontal, 0, false, Div::AUTO };
			Div()[0][2][0] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[0][2][0][1] = { a, Div::Alignment::Bottom, 50 };
			Div()[0][2][0][0] = { ac, Div::Alignment::Center, 25 };
			Div()[0][2][1] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[0][2][1][1] = { d, Div::Alignment::Bottom, 50 };
			Div()[0][2][1][0] = { dc, Div::Alignment::Center, 25 };
			Div()[0][2][2] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[0][2][2][1] = { s, Div::Alignment::Bottom, 50 };
			Div()[0][2][2][0].DivSize(25);
			Div()[0][2][3] = { 2, Div::Alignment::Vertical, 0, false, Div::AUTO };
			Div()[0][2][3][1] = { r, Div::Alignment::Bottom, 50 };
			Div()[0][2][3][0] = { rc, Div::Alignment::Center, 25 };


		}

		void Update() override
		{
			for (auto& v : voices.Voices())
			{
				v.filterfreq = freq.Value();
				v.sampleRate = SampleRate();
				v.env.a = a.Value() * 0.001;
				v.env.ac = ac.Value() * 0.01;
				v.env.d = d.Value() * 0.001;
				v.env.dc = dc.Value() * 0.01;
				v.env.s = s.Value() * 0.01;
				v.env.r = r.Value() * 0.001;
				v.env.rc = rc.Value() * 0.01;
				v.filterenv.a = fa.Value() * 0.001;
				v.filterenv.ac = fac.Value() * 0.01;
				v.filterenv.d = fd.Value() * 0.001;
				v.filterenv.dc = fdc.Value() * 0.01;
				v.filterenv.s = fs.Value() * 0.01;
				v.filterenv.r = fr.Value() * 0.001;
				v.filterenv.rc = frc.Value() * 0.01;
				v.filterenvamt = famt.Value();
				v.Update();
			}
		}

		float val = 0;
		float Generate(int c) override
		{
			if (c == 0)
			{
				val = voices.Generate();
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

		SoundMixr::Parameter& freq, & famt,
			& a, & ac, & d, & dc, & s, & r, & rc, 
			& fa, & fac, & fd, & fdc, & fs, & fr, & frc;
		SoundMixr::VoiceBank<Voice> voices{ 64 };
	};



}

extern "C" DLLDIR void* __cdecl NewInstance()
{
	return new SoundMixr::SimpleSynth;
}