#include "audio/Effects.hpp"
#include "audio/Filters.hpp"
#include "ui/Dropdown.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------- Equalizer Effect ------------------------------- \\
// -------------------------------------------------------------------------- \\

template<size_t N = 5, typename Type = BiquadFilter<>, typename P = Type::Params>
class Equalizer : public Effect
{
public:

	using Params = P;
	using Filter = Type;

	Equalizer()
		: Effect("Equalizer"), m_Parameters{ }
	{

	};

	void Init() override
	{
		for (int i = 0; i < N; i++)
		{
			m_Knob1[i] = &Emplace<KnobSlider>("Freq");
			m_Knob1[i]->Range({ 10, 22000 });
			m_Knob1[i]->Power(3);
			m_Knob1[i]->ResetValue(22000);
			m_Knob1[i]->ResetValue();
			m_Knob1[i]->Unit("Hz");
			m_Knob1[i]->Unit("kHz", 3);
			m_Knob1[i]->Size({ 30, 30 });
			m_Knob1[i]->Multiplier(0.4);

			m_Knob2[i] = &Emplace<KnobSlider>("Gain");
			m_Knob2[i]->Range({ -24, 24 });
			m_Knob2[i]->ResetValue(0);
			m_Knob2[i]->ResetValue();
			m_Knob2[i]->Unit("dB");
			m_Knob2[i]->Size({ 30, 30 });
			m_Knob2[i]->Multiplier(0.4);
			m_Knob2[i]->Decimals(2);

			m_Knob3[i] = &Emplace<KnobSlider>("Q");
			m_Knob3[i]->Range({ 0.10, 6 });
			m_Knob3[i]->Power(2);
			m_Knob3[i]->ResetValue(1);
			m_Knob3[i]->ResetValue();
			m_Knob3[i]->Unit("");
			m_Knob3[i]->Size({ 30, 30 });
			m_Knob3[i]->Multiplier(0.4);
			m_Knob3[i]->Decimals(2);

			m_Dropdown[i] = &Emplace<Dropdown<FilterType, DropdownButton>>(m_RightClickMenu);
			m_Dropdown[i]->AddOption("Off", FilterType::Off);
			m_Dropdown[i]->AddOption("LP", FilterType::LowPass);
			m_Dropdown[i]->AddOption("HP", FilterType::HighPass);
			m_Dropdown[i]->AddOption("BP", FilterType::BandPass);
			m_Dropdown[i]->AddOption("Notch", FilterType::Notch);
			m_Dropdown[i]->AddOption("Peak", FilterType::PeakingEQ);
			m_Dropdown[i]->AddOption("LS", FilterType::LowShelf);
			m_Dropdown[i]->AddOption("HS", FilterType::HighShelf);
			m_Dropdown[i]->Size({ 40, 18 });
			m_Dropdown[i]->Select(FilterType::Off);
		}

		UpdateParams();
		Height(265);
	}

	void Update(const Vec4<int>& v) override 
	{
		for (int i = 0; i < N; i++)
		{
			int x = Width() - 46 - i * 59;
			m_Dropdown[i]->Position({ x - 5, 212 });
			m_Knob1[i]->Position({ x, Height() - 109 });
			m_Knob2[i]->Position({ x, Height() - 177 });
			m_Knob3[i]->Position({ x, Height() - 245 });
		}
		UpdateParams();
		Background(theme->Get(C::Channel));
		Effect::Update(v);
	}

	void Render(CommandCollection& d) override 
	{
		Effect::Render(d);
		if (m_Small)
			return;

		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Fill>(theme->Get(C::Divider));
		for (int i = 0; i < N - 1; i++)
			d.Command<Quad>(Vec4<int>{(m_Knob1[i]->X() + m_Knob1[i]->Width() + m_Knob1[i+1]->X()) / 2, 10, 1, Height() - 45});
		d.Command<PopMatrix>();
	}
	
	void Channels(int channels) override
	{
		while (m_Equalizers.size() < channels)
			m_Equalizers.emplace_back(m_Parameters);

		Effect::Channels(channels);
	}

	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		return m_Equalizers[c].Apply(sin);
	}

	void UpdateParams()
	{
		for (int i = 0; i < N; i++)
		{
			if (m_Dropdown[i]->Value() == FilterType::Off)
			{
				m_Knob1[i]->Disable();
				m_Knob2[i]->Disable();
				m_Knob3[i]->Disable();
			}
			else
			{
				switch (m_Dropdown[i]->Value())
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
		}

		if constexpr (std::is_same_v<Filter, BiquadFilter<>>)
			for (int i = 0; i < N; i++)
			{
				m_Parameters[i].type = m_Dropdown[i]->Value();
				m_Parameters[i].f0 = m_Knob1[i]->Value();
				m_Parameters[i].dbgain = m_Knob2[i]->Value();
				m_Parameters[i].Q = m_Knob3[i]->Value();
				m_Parameters[i].RecalculateParameters();
			}
		
		else if constexpr (std::is_same_v<Filter, FIRFilter<107>>)
			for (int i = 0; i < N; i++)
			{
				m_Parameters[i].type = m_Dropdown[i]->Value();
				m_Parameters[i].Fb = m_Knob1[i]->Value();
				m_Parameters[i].attenuation = m_Knob2[i]->Value();
				m_Parameters[i].RecalculateParameters();
			}

		else if constexpr (std::is_same_v<Filter, IIR12dBOctFilter<>>)
			for (int i = 0; i < N; i++)
			{
				m_Parameters[i].type = m_Dropdown[i]->Value();
				m_Parameters[i].resofreq = m_Knob1[i]->Value();
				m_Parameters[i].amp = m_Knob3[i]->Value();
				m_Parameters[i].RecalculateParameters();
			}

	}

	operator json() override
	{
		json _json = json::object();
		_json["enabled"] = m_Enable.Active();
		_json["small"] = m_Minim->Active();
		_json["type"] = "Equalizer";
		_json["filters"] = json::array();
		for (int i = 0; i < N; i++)
		{
			json _filter = json::object();
			_filter["filter"] = i;
			_filter["type"] = m_Dropdown[i]->Value();
			_filter["freq"] = m_Knob1[i]->Value();
			_filter["gain"] = m_Knob2[i]->Value();
			_filter["q"] = m_Knob3[i]->Value();
			_json["filters"].push_back(_filter);
		}
		return _json;
	}

	void operator=(const json& json)
	{
		m_Enable.Active(json.at("enabled").get<bool>());
		m_Minim->Active(json.at("small").get<bool>());
		for (auto& i : json.at("filters"))
		{
			int index = i.at("filter").get<int>();
			m_Dropdown[index]->Select(i.at("type").get<FilterType>());
			m_Knob1[index]->Value(i.at("freq").get<double>());
			m_Knob2[index]->Value(i.at("gain").get<double>());
			m_Knob3[index]->Value(i.at("q").get<double>());
		}
		UpdateParams();
	}

private:
	static inline std::string
		m_Knob1Name = "Freq",
		m_Knob2Name = "Gain",
		m_Knob3Name = "Q";

	KnobSlider * m_Knob1[N], * m_Knob2[N], * m_Knob3[N];
	Dropdown<FilterType, DropdownButton>* m_Dropdown[N];

	Params m_Parameters[N];
	std::vector<ChannelEqualizer<N, Filter, Params>> m_Equalizers;
};


extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Equalizer<>();
	}
}