#include "EffectBase.hpp"
#include "Filters.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Equalizer Effect ------------------------------- \\
// -------------------------------------------------------------------------- \\

namespace Effects
{
	class Equalizer : public EffectBase
	{
	public:
		using Params = BiquadParameters;
		using Filter = BiquadFilter<>;

		Equalizer()
			: EffectBase("Equalizer"), m_Parameters{ }
		{
			for (int i = 0; i < N; i++)
			{
				m_Knob1[i] = &Parameter("Freq", ParameterType::Knob);
				m_Knob1[i]->Range({ 10, 22000 });
				m_Knob1[i]->Power(3);
				m_Knob1[i]->ResetValue(22000);
				m_Knob1[i]->ResetValue();
				m_Knob1[i]->Unit("Hz");
				m_Knob1[i]->Unit("kHz", 3);
				m_Knob1[i]->Size({ 30, 30 });
				m_Knob1[i]->Multiplier(0.4);

				m_Knob2[i] = &Parameter("Gain", ParameterType::Knob);
				m_Knob2[i]->Range({ -24, 24 });
				m_Knob2[i]->ResetValue(0);
				m_Knob2[i]->ResetValue();
				m_Knob2[i]->Unit("dB");
				m_Knob2[i]->Size({ 30, 30 });
				m_Knob2[i]->Multiplier(0.4);
				m_Knob2[i]->Decimals(2);

				m_Knob3[i] = &Parameter("Q", ParameterType::Knob);
				m_Knob3[i]->Range({ 0.10, 6 });
				m_Knob3[i]->Power(2);
				m_Knob3[i]->ResetValue(1);
				m_Knob3[i]->ResetValue();
				m_Knob3[i]->Unit("");
				m_Knob3[i]->Size({ 30, 30 });
				m_Knob3[i]->Multiplier(0.4);
				m_Knob3[i]->Decimals(2);

				m_Dropdown[i] = &DropDown();
				m_Dropdown[i]->AddOption("Off", (int)FilterType::Off);
				m_Dropdown[i]->AddOption("LP", (int)FilterType::LowPass);
				m_Dropdown[i]->AddOption("HP", (int)FilterType::HighPass);
				m_Dropdown[i]->AddOption("BP", (int)FilterType::BandPass);
				m_Dropdown[i]->AddOption("Notch", (int)FilterType::Notch);
				m_Dropdown[i]->AddOption("Peak", (int)FilterType::PeakingEQ);
				m_Dropdown[i]->AddOption("LS", (int)FilterType::LowShelf);
				m_Dropdown[i]->AddOption("HS", (int)FilterType::HighShelf);
				m_Dropdown[i]->Size({ 40, 18 });
				m_Dropdown[i]->Select((int)FilterType::Off);
			}

			Height(240);

			Div().Divs(N);
			Div().Dividers(true);
			Div().Align(Div::Alignment::Horizontal);

			for (int i = 0; i < N; i++)
			{
				Div()[i].Divs(4);
				Div()[i].Align(Div::Alignment::Vertical);
				Div()[i][3] = m_Dropdown[i];
				Div()[i][3].DivSize(32);
				Div()[i][2] = m_Knob1[i];
				Div()[i][1] = m_Knob2[i];
				Div()[i][0] = m_Knob3[i];
			}
		}

		void Update() override 
		{
			UpdateParams();
		}
	
		void Channels(int channels) override
		{
			while (m_Equalizers.size() < channels)
				m_Equalizers.emplace_back(m_Parameters);
		}

		float NextSample(float sin, int c) override
		{
			return m_Equalizers[c].Apply(sin);
		}

		void UpdateParams()
		{
			for (int i = 0; i < N; i++)
			{
				if (m_Dropdown[i]->Selected() == (int)FilterType::Off)
				{
					m_Knob1[i]->Disable();
					m_Knob2[i]->Disable();
					m_Knob3[i]->Disable();
				}
				else
				{
					switch ((FilterType)m_Dropdown[i]->Selected())
					{
					case FilterType::LowPass:
					case FilterType::HighPass:
					case FilterType::BandPass:
					case FilterType::Notch:
					case FilterType::AllPass:
						m_Knob2[i]->Disable();
						break;
					case FilterType::PeakingEQ:
					case FilterType::LowShelf:
					case FilterType::HighShelf:
						m_Knob2[i]->Enable();
						break;
					}
					m_Knob1[i]->Enable();
					m_Knob3[i]->Enable();

				}
		
				m_Parameters[i].type = (FilterType)m_Dropdown[i]->Selected();
				m_Parameters[i].f0 = m_Knob1[i]->Value();
				m_Parameters[i].dbgain = m_Knob2[i]->Value();
				m_Parameters[i].Q = m_Knob3[i]->Value();
				m_Parameters[i].RecalculateParameters();
			}

		}

	private:
		static inline constexpr int N = 5;

		Effects::Parameter * m_Knob1[N], * m_Knob2[N], * m_Knob3[N];
		Effects::DropDown * m_Dropdown[N];

		Params m_Parameters[N];
		std::vector<ChannelEqualizer<N, Filter, Params>> m_Equalizers;
	};
}

extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Effects::Equalizer();
	}
}
