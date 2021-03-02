#pragma once
#include "pch.hpp"
#include "audio/Effects.hpp"
#include "ui/Soundboard.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Device ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Device
{
public:
	Device(PaDeviceIndex id, const PaDeviceInfo& a)
		: id(id), info(a)
	{}

	PaDeviceIndex id;
	const PaDeviceInfo& info;
};

// -------------------------------------------------------------------------- \\
// -------------------------- Output Channel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class InputChannel;
class OutputChannel;
class SoundboardChannel;
class ChannelGroup;

class ChannelBase
{
public:
	ChannelBase(int id, const std::string& name, const bool IsInput)
		: m_Id(id), m_Name(name), m_IsInput(IsInput)
	{}

	auto Name() -> std::string& { return m_Name; }
	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void MonoLevel(float l) { m_MonoLevel = l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float v) { m_TPeak = v; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }
	void Group(ChannelGroup* p, int index) { m_Group = p; m_GroupIndex = index; }

	virtual void Level(float l) { m_Level = l; };
	virtual void CalcLevel() = 0;
	virtual float Level() const { return m_OutLevel; }

	int	  ID()               const { return m_Id; }
	float Volume()           const { return m_Volume; }
	float Pan()              const { return m_Pan; }
	float Peak()             const { return m_Peak; }
	bool  Muted()            const { return m_Muted; }
	bool  Mono()             const { return m_Mono; }
	bool  IsInput()			 const { return m_IsInput; }
	float TPeak()            const { return m_TPeak; }
	float UnprocessedLevel() const { return m_Level; }
	float MonoLevel()        const { return m_MonoLevel; }

protected:
	int m_Id,
		m_GroupIndex;

	bool m_Mono = false,
		m_Muted = false;
		
	const bool m_IsInput;

	float m_Level = 0,
		m_MonoLevel = 0,
		m_OutLevel = 0,
		m_Peak = 0,
		m_TPeak = 0,
		m_Pan = 1,
		m_Volume = 1;

	std::string m_Name;
	ChannelGroup* m_Group = nullptr;
};


class ChannelGroup
{
public:
	~ChannelGroup() { ClearChannels(); }

	auto  Connections() -> std::vector<::ChannelGroup*> const { return m_Connected; }
	auto  Channels() -> std::vector<ChannelBase*>& const { return m_Channels; }
	int   ChannelAmount()                   const { return m_ChannelAmount; }
	auto  EffectsGroup() -> EffectsGroup&   const { return m_EffectsGroup; }
	auto  Name() -> std::string& const { return m_Name; }
	int   ID()                   const { return m_Id; }
	float Volume()               const { return m_Volume; }
	bool  Muted()                const { return m_Muted; }
	bool  Mono()		         const { return m_Mono; }
	bool  IsInput()				 const { return m_IsInput; }
	float Pan()					 const { return m_Pan; }

	void  Volume(float v) { for (auto& c : m_Channels) c->Volume(v); m_Volume = v; }
	void  Mute(bool v) { for (auto& c : m_Channels) c->Mute(v); m_Muted = v; }
	void  Mono(bool v) { for (auto& c : m_Channels) c->Mono(v); m_Mono = v; }
	void  Pan(float p)
	{
		m_Pan = p;
		int _index = 0;
		double _p = -p / 50.0;
		double _a = 1.0 - std::abs((ChannelAmount() - 1) / 2.0 * _p);
		for (auto& c : m_Channels)
		{
			float _pan = constrain(_p * (_index - (ChannelAmount() - 1) / 2.0) + _a, 0.0, 1.0);
			c->Pan(_pan);
			_index++;
		}
	}

	void ClearConnections() { for (auto& i : m_Connected) Disconnect(i); }
	void ClearChannels() { for (auto& i : m_Channels) i->Group(nullptr, -1); }
	bool Connected(::ChannelGroup* other) const { return std::find(m_Connected.begin(), m_Connected.end(), other) != m_Connected.end() && other != nullptr; }
	void Connect(::ChannelGroup* other) { if (other != nullptr) other->ConnectDirect(this), ConnectDirect(other); }
	void ConnectDirect(::ChannelGroup* other) { if (!Connected(other) && other != nullptr) m_Connected.push_back(other); }
	void Disconnect(::ChannelGroup* other) { if (other != nullptr) other->DisconnectDirect(this), DisconnectDirect(other); }
	void DisconnectDirect(::ChannelGroup* other)
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), other);
		if (it != m_Connected.end() && other != nullptr)
			m_Connected.erase(it);
	}

	void AddChannel(ChannelBase* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a == m_Channels.end() && c != nullptr)
		{
			ClearConnections();
			if (ChannelAmount() == 0)
			{
				m_Mono = false;
				m_Muted = false;
				m_Name = c->Name();
				m_Id = c->ID();
				m_IsInput = c->IsInput();
				c->Mono(false);
				c->Mute(false);
			}
			m_Channels.push_back(c);
			c->Group(this, ChannelAmount());
			c->Mono(Mono()); 
			c->Mute(Muted());
			Pan(Pan());
			m_ChannelAmount++;
		}
	}

	void RemoveChannel(ChannelBase* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
		{
			m_ChannelAmount--;
			ClearConnections();
			m_Channels.erase(a);
			for (int i = 0; i < ChannelAmount(); i++)
				m_Channels[i]->Group(this, i);
			Pan(Pan());
		}

		if (ChannelAmount())
		{
			m_Name = m_Channels[0]->Name();
			m_Id = m_Channels[0]->ID();
		}
	}

	float DoEffects()
	{
		if (ChannelAmount())
			return m_EffectsGroup.NextSample(m_Channels[0]->Level());
		return 0;
	}

	float GetLevel(int id)
	{
		m_StartRound = true;
		float level = 0;

		int amt = ChannelAmount();
		for (auto& i : m_Connected)
		{
			int iamt = i->ChannelAmount();
			if (i->Mono())
				level += i->Channels()[id % iamt]->Level();
			
			else
				for (int j = id; j < iamt; j += amt)
					level += i->Channels()[j]->Level();
		}
		return level;
	}

	float GetMonoLevel()
	{
		if (m_StartRound)
		{
			m_MonoLevel = 0;
			for (auto& i : m_Channels)
				m_MonoLevel += i->UnprocessedLevel();

			m_MonoLevel /= ChannelAmount();
			m_StartRound = false;
		}

		return m_MonoLevel;
	}

private:
	bool m_StartRound = false;
	float m_Pan = 0;
	float m_MonoLevel = 0;

	int m_Id = -1, m_ChannelAmount = 0;
	float m_Volume = 1;
	bool m_Mono = false,
		m_Muted = false,
		m_IsInput = false;

	::EffectsGroup m_EffectsGroup;
	std::string m_Name = "APPLE";
	std::vector<ChannelBase*> m_Channels;
	std::vector<::ChannelGroup*> m_Connected;
};

class InputChannel : public ChannelBase
{
public:
	using ChannelBase::ChannelBase;

	void CalcLevel() override
	{
		if (m_Muted)
			m_OutLevel = 0;

		else if (m_Mono && m_Group != nullptr)
		{
			float level = 0;
			for (auto& i : m_Group->Channels())
				level += i->UnprocessedLevel();

			m_OutLevel = (level / m_Group->ChannelAmount());
		}
		else
			m_OutLevel = m_Level;

		if (m_Group != nullptr)
			m_OutLevel = m_Group->DoEffects() * m_Volume * m_Pan;
	}
};

class OutputChannel : public ChannelBase
{
public:
	using ChannelBase::ChannelBase;
	
	void  CalcLevel()    override { m_OutLevel = m_Muted || !m_Group ? 0 : m_Group->GetLevel(m_GroupIndex); }
	float Level()  const override { return (m_Mono && m_Group ? m_Group->GetMonoLevel() : m_OutLevel) * m_Volume * m_Pan; }
};

class SoundboardChannel : public ChannelBase
{
public:
	SoundboardChannel(Soundboard& soundBoard)
		: ChannelBase(0, "Soundboard", true), m_Soundboard(soundBoard)
	{ }

	void  CalcLevel()    override { m_OutLevel = m_Soundboard.GetLevel(); }
	
private:
	Soundboard& m_Soundboard;
};

// -------------------------------------------------------------------------- \\
// -------------------------- SAR ASIO Device ------------------------------- \\
// -------------------------------------------------------------------------- \\

class AsioDevice
{
public:
	AsioDevice();

	~AsioDevice();

	auto Devices() -> std::vector<::Device>& { return m_Devices; }
	void Device(::Device& d) { m_Device = &d; }
	auto Device() -> ::Device& { return *m_Device; }
	bool StreamRunning() { return Pa_IsStreamActive(stream); }
	bool OpenStream();
	void CloseStream();
	bool StartStream();
	bool StopStream();

	std::vector<InputChannel>& Inputs() { return m_Inputs; }
	std::vector<OutputChannel>& Outputs() { return m_Outputs; }

private:
	double m_Samplerate;
	int m_BufferSize;
	
	PaStream* stream = nullptr;
	::Device* m_Device = nullptr;

	std::vector<InputChannel> m_Inputs;
	std::vector<OutputChannel> m_Outputs;
	std::vector<::Device> m_Devices;

	static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};