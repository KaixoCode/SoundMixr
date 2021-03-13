#include "audio/Effects.hpp"

class Flanger : public Effect
{
public:
	Flanger()
		:
		Effect("Flanger")
	{}

	void Init() override
	{
		Height(80);
	}

	void Update(const Vec4<int>& v) override
	{
		Background(theme->Get(C::Channel));
		Effect::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		Effect::Render(d);
	}

	void Channels(int c) override
	{
		Effect::Channels(c);
	}

	float NextSample(float sin, int c) override
	{
		if (!m_Enabled)
			return sin;

		return sin;
	}

	void UpdateParams()
	{

	}

	operator json()
	{
		json _json = json::object();
		_json["type"] = "Flanger";
		return _json;
	}

	void operator=(const json& json)
	{
		UpdateParams();
	}
};


extern "C"
{
	DLLDIR void* NewInstance()
	{
		return new Flanger();
	}
}