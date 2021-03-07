#pragma once
#include "audio/Effects.hpp"
#include "ui/Dropdown.hpp"

enum class FilterType
{
	Off, LowPass, HighPass, BandPass, Notch, AllPass, PeakingEQ, LowShelf, HighShelf, ITEMS
};

template<typename T>
class Filter
{
public:
	using Params = T;
	virtual float Apply(float s, Params& p) = 0;
};


class FilterParameters
{
public:
	virtual void RecalculateParameters() = 0;
};

template<size_t M>
class FIRFilterParameters : public FilterParameters
{
public:

	// Coefficients
	double H[M];
};

template<size_t M>
class IIRFilterParameters : public FilterParameters
{
public:

	// Coefficients
	double A[M + 1];
	double B[M + 1];
};

class BiquadParameters
{
public:

	// Parameters
	union { double Q, BW, S = 1; };
	double f0 = 22000, dbgain = 0;
	FilterType type = FilterType::Off;

	void RecalculateParameters()
	{
		w0 = 2 * M_PI * (f0 / Effect::sampleRate);
		cosw0 = std::cos(w0), sinw0 = std::sin(w0);

		switch (type) {
		case FilterType::Off:
		{
			b0 = 1, a0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
		} break;
		case FilterType::LowPass:
		{
			alpha = sinw0 / (2.0 * Q);
			//alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
			b0 = (1.0 - cosw0) / 2.0, b1 = 1.0 - cosw0, b2 = b0;
			a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
		} break;
		case FilterType::HighPass:
		{
			alpha = sinw0 / (2.0 * Q);
			b0 = (1.0 + cosw0) / 2.0, b1 = -(1.0 + cosw0), b2 = b0;
			a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
		} break;
		case FilterType::BandPass:
		{
			alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
			b0 = sinw0 / 2.0, b1 = 0.0, b2 = -b0;
			a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
		} break;
		case FilterType::Notch:
		{
			alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
			b0 = 1, b1 = -2.0 * cosw0, b2 = 1.0;
			a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
		} break;
		case FilterType::AllPass:
		{
			alpha = sinw0 / (2.0 * Q);
			b0 = 1.0 - alpha, b1 = -2.0 * cosw0, b2 = 1.0 + alpha;
			a0 = 1.0 + alpha, a1 = -2.0 * cosw0, a2 = 1.0 - alpha;
		} break;
		case FilterType::PeakingEQ:
		{
			A = std::pow(10, dbgain / 40.0);
			alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
			b0 = 1.0 + alpha * A, b1 = -2.0 * cosw0, b2 = 1.0 - alpha * A;
			a0 = 1.0 + alpha / A, a1 = -2.0 * cosw0, a2 = 1.0 - alpha / A;
		} break;
		case FilterType::LowShelf:
		{
			A = std::pow(10, dbgain / 40.0);
			double t = std::max((A + 1.0 / A) * (1.0 / S - 1.0) + 2, 0.0);
			alpha = (sinw0 / 2.0) * std::sqrt(t);
			double sqrtAa = std::sqrt(A) * alpha;
			b0 = A * ((A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtAa);
			b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosw0);
			b2 = A * ((A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtAa);
			a0 = (A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtAa;
			a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosw0);
			a2 = (A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtAa;
		} break;
		case FilterType::HighShelf:
		{
			A = std::pow(10, dbgain / 40.0);
			double t = std::max((A + 1.0 / A) * (1.0 / S - 1.0) + 2, 0.0);
			alpha = (sinw0 / 2.0) * std::sqrt(t);
			double sqrtAa = std::sqrt(A) * alpha;
			b0 = A * ((A + 1.0) + (A - 1.0) * cosw0 + 2.0 * sqrtAa);
			b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosw0);
			b2 = A * ((A + 1.0) + (A - 1.0) * cosw0 - 2.0 * sqrtAa);
			a0 = (A + 1.0) - (A - 1.0) * cosw0 + 2.0 * sqrtAa;
			a1 = 2.0 * ((A - 1.0) - (A + 1) * cosw0);
			a2 = (A + 1.0) - (A - 1.0) * cosw0 - 2.0 * sqrtAa;
		}
		}
		b0a0 = b0 / a0, b1a0 = b1 / a0, b2a0 = b2 / a0, a1a0 = a1 / a0, a2a0 = a2 / a0;
	}

	// Constants
	static inline const double log2 = std::log(2);

	// Coeficients
	double b0 = 1, b1 = 0, b2 = 0, a0 = 1, a1 = 0, a2 = 0;
	double b0a0 = 0, b1a0 = 0, b2a0 = 0, a1a0 = 0, a2a0 = 0;

	// Intermediate values
	double w0 = 0, cosw0 = 0, sinw0 = 0, A = 0, alpha = 0;
};

template<typename P = BiquadParameters>
class BiquadFilter : public Filter<P>
{
public:
	float Apply(float s, P& p) override
	{
		x[0] = s;
		y[0] = constrain(p.b0a0 * x[0] + p.b1a0 * x[1] + p.b2a0 * x[2] - p.a1a0 * y[1] - p.a2a0 * y[2], -10000000, 10000000);

		for (int i = sizeof(y) / sizeof(double) - 2; i >= 0; i--)
			y[i + 1] = y[i];

		for (int i = sizeof(x) / sizeof(double) - 2; i >= 0; i--)
			x[i + 1] = x[i];

		return y[0];
	}

private:
	double y[3]{ 0, 0, 0 }, x[3]{ 0, 0, 0 };
};

template<size_t M>
class KaiserBesselParameters : public FIRFilterParameters<M>
{
public:
	void RecalculateParameters()
	{
		// Calculate the impulse response
		A[0] = 2 * (Fb - Fa) / Effect::sampleRate;
		int _np = (M - 1) / 2;
		for (int j = 1; j <= _np; j++)
			A[j] = (std::sin(2.0 * j * M_PI * Fb / Effect::sampleRate) - std::sin(2.0 * j * M_PI * Fa / Effect::sampleRate)) / (j * M_PI);

		// Calculate alpha
		double _alpha;
		if (attenuation < 21)
			_alpha = 0;

		else if (attenuation > 50)
			_alpha = 0.1102 * (attenuation - 8.7);

		else
			_alpha = 0.5842 * std::pow((attenuation - 21), 0.4) + 0.07886 * (attenuation - 21);

		// Window the ideal response with the Kaiser-Bessel window
		double _i0alpha = I0(_alpha);
		for (int j = 0; j <= _np; j++)
			H[_np + j] = A[j] * I0(_alpha * std::sqrt(1.0 - ((double)j * (double)j / (_np * _np)))) / _i0alpha;

		// It is mirrored so other half is same
		for (int j = 0; j < _np; j++)
			H[j] = H[M - 1 - j];
	}

	// This function calculates the zeroth order Bessel function
	double I0(double x)
	{
		double d = 0, ds = 1, s = 1;
		do
		{
			d += 2;
			ds *= x * x / (d * d);
			s += ds;
		} while (ds > s * 1e-6);
		return s;
	}

	double Fa = 0, Fb = 7200;	// Frequencies a and b
	double attenuation = 48;	// Attenuation
	double A[M];				// Ideal impulse response
};

template<size_t M, typename P = KaiserBesselParameters<M>>
class FIRFilter : public Filter<P>
{
public:
	FIRFilter() { std::fill(std::begin(x), std::end(x), 0); }

	float Apply(float s, P& p) override
	{
		x[0] = s;
		
		double y = 0;
		for (int i = 0; i < M; i++)
			y += p.H[i] * x[i];

		for (int i = sizeof(x) / sizeof(double) - 2; i >= 0; i--)
			x[i + 1] = x[i];

		return y;
	}

private:
	double x[M];
};

template<size_t N, class F, class P = F::Params>
class ChannelEqualizer
{
public:
	ChannelEqualizer(P(&a)[N])
		: m_Params(a), m_Filters()
	{}

	float Apply(float s)
	{
		for (int i = 0; i < N; i++)
			if (m_Params[i].type != FilterType::Off)
				s = m_Filters[i].Apply(s, m_Params[i]);

		return s;
	}

	P (&m_Params)[N];
	F m_Filters[N];
};

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

			m_Dropdown[i] = &Emplace<Dropdown<FilterType, DropdownButton>>();
			m_Dropdown[i]->AddOption("Off", FilterType::Off);
			m_Dropdown[i]->AddOption("LP", FilterType::LowPass);
			m_Dropdown[i]->AddOption("HP", FilterType::HighPass);
			m_Dropdown[i]->AddOption("BP", FilterType::BandPass);
			m_Dropdown[i]->AddOption("Notch", FilterType::Notch);
			m_Dropdown[i]->AddOption("Peak", FilterType::PeakingEQ);
			m_Dropdown[i]->AddOption("LS", FilterType::LowShelf);
			m_Dropdown[i]->AddOption("HS", FilterType::HighShelf);
			m_Dropdown[i]->Size({40, 18});
			m_Dropdown[i]->Select(FilterType::Off);
		}

		UpdateParams();
		Height(265);
	};

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
		Background(Theme<C::Channel>::Get());
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
		d.Command<Fill>(Theme<C::Divider>::Get());
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