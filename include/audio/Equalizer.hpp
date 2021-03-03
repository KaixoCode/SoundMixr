#pragma once
#include "audio/Effects.hpp"

enum class FilterType
{
	LowPass, HighPass, BandPass, Notch, AllPass, PeakingEQ, LowShelf, HighShelf
};

class BiquadParameters
{
public:


	// Parameters
	union { double Q, BW, S = 1; };
	double f0 = 400, dbgain = 0;
	FilterType type = FilterType::LowPass;

	void RecalculateParameters()
	{
		w0 = 2 * M_PI * (f0 / Effect::sampleRate);
		cosw0 = std::cos(w0), sinw0 = std::sin(w0);

		switch (type) {
		case FilterType::LowPass:
		{
			alpha = sinw0 / (2.0 * Q);
			//alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
			b0 = (1.0 - cosw0) / 2.0, b1 = 1.0 - cosw0, b2 = b0;
			a0 = 1.0 + alpha, a1 = -2.0 * alpha, a2 = 1.0 - alpha;
		} break;
		case FilterType::HighPass:
		{
			alpha = sinw0 / (2.0 * Q);
			alpha = sinw0 * std::sinh((log2 / 2.0) * BW * (w0 / sinw0));
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
			alpha = (sinw0 / 2.0) * std::sqrt((A + 1.0 / A) * (1.0 / S - 1.0) + 2);
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

class BiquadFilter
{
public:
	using Param = BiquadParameters;

	float Apply(float s, BiquadParameters& p)
	{
		x[0] = s;
		y[0] = constrain(p.b0a0 * x[0] + p.b1a0 * x[1] + p.b2a0 * x[2] - p.a1a0 * y[1] - p.a2a0 * y[2], -2, 2);

		for (int i = sizeof(y) / sizeof(double) - 2; i >= 0; i--)
			y[i + 1] = y[i];

		for (int i = sizeof(x) / sizeof(double) - 2; i >= 0; i--)
			x[i + 1] = x[i];

		return y[0];
	}

private:
	double y[3]{ 1, 1, 1 }, x[3]{ 0, 0, 0 };
};

template<size_t M>
class KaiserBesselParameters
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
	double H[M];				// Kaiser-Bessel window
	double A[M];				// Ideal impulse response
};

template<size_t M>
class KaiserBesselFilter
{
public:
	using Param = KaiserBesselParameters<M>;

	KaiserBesselFilter()
	{
		std::fill(std::begin(x), std::end(x), 0);
	}

	float Apply(float s, Param& p)
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

template<size_t COUNT, typename Filter, typename Params = Filter::Param>
class ChannelEqualizer
{
public:
	ChannelEqualizer(Params(&a)[COUNT])
		: m_Params(a), m_Filters()
	{}

	float Apply(float s)
	{
		return m_Filters[0].Apply(s, m_Params[0]);
		
		for (int i = 0; i < COUNT; i++)
			s = m_Filters[i].Apply(s, m_Params[i]);

		return s;
	}

	Params(&m_Params)[COUNT];
	Filter m_Filters[COUNT];
};

// -------------------------------------------------------------------------- \\
// ------------------------- Equalizer Effect ------------------------------- \\
// -------------------------------------------------------------------------- \\

class Equalizer : public Effect
{
public:
	Equalizer()
		: Effect("Equalizer"), m_Parameters{ },
		m_Knob11(Emplace<KnobSlider>()), m_Knob12(Emplace<KnobSlider>()), m_Knob13(Emplace<KnobSlider>()),
		m_Knob21(Emplace<KnobSlider>()), m_Knob22(Emplace<KnobSlider>()), m_Knob23(Emplace<KnobSlider>()),
		m_Knob31(Emplace<KnobSlider>()), m_Knob32(Emplace<KnobSlider>()), m_Knob33(Emplace<KnobSlider>())
	{
		m_Knob11.SliderRange({ 20000, 10 });
		m_Knob11.ResetValue(15000);
		m_Knob11.ResetValue();
		m_Knob11.Unit("Hz");
		m_Knob11.Size({ 30, 30 });
		m_Knob11.SliderMult(0.2);

		m_Knob21.SliderRange({ 20000, 10 });
		m_Knob21.ResetValue(15000);
		m_Knob21.ResetValue();
		m_Knob21.Unit("Hz");
		m_Knob21.Size({ 30, 30 });
		m_Knob21.SliderMult(0.2);

		m_Knob31.SliderRange({ 20000, 10 });
		m_Knob31.ResetValue(15000);
		m_Knob31.ResetValue();
		m_Knob31.Unit("Hz");
		m_Knob31.Size({ 30, 30 });
		m_Knob31.SliderMult(0.2);

		m_Knob12.SliderRange({ 24, -24 });
		m_Knob12.ResetValue(0);
		m_Knob12.ResetValue();
		m_Knob12.Unit("dB");
		m_Knob12.Size({ 30, 30 });
		m_Knob12.SliderMult(0.4);
			   
		m_Knob22.SliderRange({ 24, -24 });
		m_Knob22.ResetValue(0);
		m_Knob22.ResetValue();
		m_Knob22.Unit("dB");
		m_Knob22.Size({ 30, 30 });
		m_Knob22.SliderMult(0.4);
			   
		m_Knob32.SliderRange({ 24, -24 });
		m_Knob32.ResetValue(0);
		m_Knob32.ResetValue();
		m_Knob32.Unit("dB");
		m_Knob32.Size({ 30, 30 });
		m_Knob32.SliderMult(0.4);

		m_Knob13.SliderRange({ 6, 0.10 });
		m_Knob13.ResetValue(1);
		m_Knob13.ResetValue();
		m_Knob13.Unit("");
		m_Knob13.Size({ 30, 30 });
		m_Knob13.SliderMult(0.4);
			   
		m_Knob23.SliderRange({ 6, 0.10 });
		m_Knob23.ResetValue(1);
		m_Knob23.ResetValue();
		m_Knob23.Unit("");
		m_Knob23.Size({ 30, 30 });
		m_Knob23.SliderMult(0.4);
			   
		m_Knob33.SliderRange({ 6, 0.10 });
		m_Knob33.ResetValue(1);
		m_Knob33.ResetValue();
		m_Knob33.Unit("");
		m_Knob33.Size({ 30, 30 });
		m_Knob33.SliderMult(0.4);
		UpdateParams();
		Height(245);
	};

	void Update(const Vec4<int>& v) override 
	{
		m_Knob11.Position({ Width() - 145, 160 });
		m_Knob12.Position({ Width() - 145, 90 });
		m_Knob13.Position({ Width() - 145, 20 });
		m_Knob21.Position({ Width() - 95, 160 });
		m_Knob22.Position({ Width() - 95, 90 });
		m_Knob23.Position({ Width() - 95, 20 });
		m_Knob31.Position({ Width() - 45, 160 });
		m_Knob32.Position({ Width() - 45, 90 });
		m_Knob33.Position({ Width() - 45, 20 });
		UpdateParams();
		Background(Theme<C::Channel>::Get());
		Effect::Update(v);
	}

	void Render(CommandCollection& d) override 
	{
		Effect::Render(d);
		using namespace Graphics;
		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Font>(Fonts::Gidole14, 14.0f);
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<TextAlign>(Align::CENTER, Align::BOTTOM);
		d.Command<Text>(&m_Knob1Name, m_Knob11.Position() + Vec2<int>{ m_Knob11.Width() / 2, m_Knob11.Height() + 5 });
		d.Command<Text>(&m_Knob2Name, m_Knob12.Position() + Vec2<int>{ m_Knob12.Width() / 2, m_Knob12.Height() + 5 });
		d.Command<Text>(&m_Knob3Name, m_Knob13.Position() + Vec2<int>{ m_Knob13.Width() / 2, m_Knob13.Height() + 5 });
		d.Command<Fill>(Theme<C::Divider>::Get());
		d.Command<Quad>(Vec4<int>{(m_Knob11.X() + m_Knob11.Width() + m_Knob21.X()) / 2, 10, 1, 50});
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<Text>(&m_Knob1Name, m_Knob21.Position() + Vec2<int>{ m_Knob21.Width() / 2, m_Knob21.Height() + 5 });
		d.Command<Text>(&m_Knob2Name, m_Knob22.Position() + Vec2<int>{ m_Knob22.Width() / 2, m_Knob22.Height() + 5 });
		d.Command<Text>(&m_Knob3Name, m_Knob23.Position() + Vec2<int>{ m_Knob23.Width() / 2, m_Knob23.Height() + 5 });
		d.Command<Fill>(Theme<C::Divider>::Get());
		d.Command<Quad>(Vec4<int>{(m_Knob21.X() + m_Knob21.Width() + m_Knob31.X()) / 2, 10, 1, 50});
		d.Command<Fill>(Theme<C::TextSmall>::Get());
		d.Command<Text>(&m_Knob1Name, m_Knob21.Position() + Vec2<int>{ m_Knob21.Width() / 2, m_Knob21.Height() + 5 });
		d.Command<Text>(&m_Knob2Name, m_Knob22.Position() + Vec2<int>{ m_Knob22.Width() / 2, m_Knob22.Height() + 5 });
		d.Command<Text>(&m_Knob3Name, m_Knob23.Position() + Vec2<int>{ m_Knob23.Width() / 2, m_Knob23.Height() + 5 });
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
		return m_Equalizers[c].Apply(sin);
	}

	void UpdateParams()
	{
		//m_Parameters[0].Q = (1.0 - m_Knob13.SliderValue()) * 5.9 + 0.10;
		//m_Parameters[1].Q = (1.0 - m_Knob23.SliderValue()) * 5.9 + 0.10;
		//m_Parameters[2].Q = (1.0 - m_Knob33.SliderValue()) * 5.9 + 0.10;
		//m_Parameters[0].dbgain = (1.0 - m_Knob12.SliderValue()) * 48 - 24;
		//m_Parameters[1].dbgain = (1.0 - m_Knob22.SliderValue()) * 48 - 24;
		//m_Parameters[2].dbgain = (1.0 - m_Knob32.SliderValue()) * 48 - 24;
		//m_Parameters[0].f0 = (1.0 - m_Knob11.SliderValue()) * 19990 + 10;
		//m_Parameters[1].f0 = (1.0 - m_Knob21.SliderValue()) * 19990 + 10;
		//m_Parameters[2].f0 = (1.0 - m_Knob31.SliderValue()) * 19990 + 10;
		m_Parameters[0].attenuation = (1.0 - m_Knob12.SliderValue()) * 48 + 21;
		m_Parameters[1].attenuation = (1.0 - m_Knob22.SliderValue()) * 48 + 21;
		m_Parameters[2].attenuation = (1.0 - m_Knob32.SliderValue()) * 48 + 21;
		m_Parameters[0].Fb = (1.0 - m_Knob11.SliderValue()) * 19990 + 10;
		m_Parameters[1].Fb = (1.0 - m_Knob21.SliderValue()) * 19990 + 10;
		m_Parameters[2].Fb = (1.0 - m_Knob31.SliderValue()) * 19990 + 10;
		m_Parameters[0].RecalculateParameters();
		m_Parameters[1].RecalculateParameters();
		m_Parameters[2].RecalculateParameters();
	}

	operator json() override
	{
		json _json = json::object();
		_json["type"] = "Equalizer";

		return _json;
	}

	void operator=(const json& json)
	{
		
		UpdateParams();
	}

private:
	static inline std::string
		m_Knob1Name = "Freq",
		m_Knob2Name = "Gain",
		m_Knob3Name = "Q";

	KnobSlider
		& m_Knob11, & m_Knob12, & m_Knob13,
		& m_Knob21, & m_Knob22, & m_Knob23,
		& m_Knob31, & m_Knob32, & m_Knob33;

	KaiserBesselParameters<107> m_Parameters[3];
	std::vector<ChannelEqualizer<3, KaiserBesselFilter<107>>> m_Equalizers;
};