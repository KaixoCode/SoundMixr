#pragma once
#include "pch.hpp"
#include "audio/Effects.hpp"

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

template<typename This, typename Other = std::conditional<std::is_same_v<This, InputChannel>, OutputChannel, InputChannel>::type>
class ChannelGroup;

template<typename This, typename Other>
class ChannelGroup
{
public:
	~ChannelGroup() { ClearChannels(); }

	auto  Connections() -> std::vector<::ChannelGroup<Other, This>*> const { return m_Connected; }
	auto  Channels() -> std::vector<This*>& const { return m_Channels; }
	int   ChannelAmount()                   const { return m_ChannelAmount; }
	auto  Name() -> std::string& const { return m_Name; }
	int   ID()                   const { return m_Id; }
	float Volume()               const { return m_Volume; }
	bool  Muted()                const { return m_Muted; }
	bool  Mono()		         const { return m_Mono; }
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
	bool Connected(::ChannelGroup<Other, This>* other) const { return std::find(m_Connected.begin(), m_Connected.end(), other) != m_Connected.end() && other != nullptr; }
	void Connect(::ChannelGroup<Other, This>* other) { if (other != nullptr) other->ConnectDirect(this), ConnectDirect(other); }
	void ConnectDirect(::ChannelGroup<Other, This>* other) { if (!Connected(other) && other != nullptr) m_Connected.push_back(other); }
	void Disconnect(::ChannelGroup<Other, This>* other) { if (other != nullptr) other->DisconnectDirect(this), DisconnectDirect(other); }
	void DisconnectDirect(::ChannelGroup<Other, This>* other)
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), other);
		if (it != m_Connected.end() && other != nullptr)
			m_Connected.erase(it);
	}

	void AddChannel(This* c)
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

	void RemoveChannel(This* c)
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
	bool m_Mono = false, m_Muted = false;

	std::string m_Name = "APPLE";
	std::vector<This*> m_Channels;
	std::vector<::ChannelGroup<Other, This>*> m_Connected;
};

using InputChannelGroup = ChannelGroup<InputChannel>;
using OutputChannelGroup = ChannelGroup<OutputChannel>;

template<typename GroupType>
class Channel
{
public:
	Channel(int id, const std::string& name)
		: m_Id(id), m_Name(name)
	{}

	auto Name() -> std::string& { return m_Name; }
	void Group(GroupType* p, int index) { m_Group = p; m_GroupIndex = index; }
	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void MonoLevel(float l) { m_MonoLevel = l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float v) { m_TPeak = v; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }

	virtual void Level(float l) { m_Level = l; };
	virtual void CalcLevel() = 0;
	virtual float Level() const { return m_OutLevel; }

	int	  ID()               const { return m_Id; }
	float Volume()           const { return m_Volume; }
	float Pan()              const { return m_Pan; }
	float Peak()             const { return m_Peak; }
	bool  Muted()            const { return m_Muted; }
	bool  Mono()             const { return m_Mono; }
	float TPeak()            const { return m_TPeak; }
	float UnprocessedLevel() const { return m_Level; }
	float MonoLevel()        const { return m_MonoLevel; }
	auto  EffectsGroup() -> EffectsGroup& const { return m_EffectsGroup; }
	
protected:
	int m_Id,
		m_GroupIndex;

	bool m_Mono = false,
		m_Muted = false;

	float m_Level = 0, 
		m_MonoLevel = 0, 
		m_OutLevel = 0,
		m_Peak = 0,
		m_TPeak = 0,
		m_Pan = 1,
		m_Volume = 1;

	GroupType* m_Group = nullptr;

	std::string m_Name;
	::EffectsGroup m_EffectsGroup;
};

class InputChannel : public Channel<InputChannelGroup>
{
public:
	using Channel::Channel;

	void CalcLevel() override
	{
		if (m_Muted)
			m_OutLevel = 0;

		else if (m_Mono && m_Group != nullptr)
		{
			float level = 0;
			for (auto& i : m_Group->Channels())
				level += i->UnprocessedLevel();

			m_OutLevel = (level / m_Group->ChannelAmount()) * m_Pan * m_Volume;
		}
		else
			m_OutLevel = m_Level * m_Volume * m_Pan;
	}
};

class OutputChannel : public Channel<OutputChannelGroup>
{
public:
	using Channel::Channel;
	
	void CalcLevel() override
	{
		if (m_Muted)
			m_Level = m_OutLevel = 0;

		else if (m_Group)
			m_Level = m_OutLevel = m_Group->GetLevel(m_GroupIndex);
	}

	float Level() const override
	{
		return (m_Mono && m_Group ? m_Group->GetMonoLevel() : m_OutLevel) * m_Volume * m_Pan;
	}

	void Level(float l) override { m_Level = l; };
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