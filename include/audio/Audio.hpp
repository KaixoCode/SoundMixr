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

class ChannelGroup;

class Channel
{
public:
	Channel(int id, const std::string& name, const bool IsInput)
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
	void Group(const std::shared_ptr<::ChannelGroup>& p, int index) { m_Group = p; m_GroupIndex = index; }

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
		m_GroupIndex = -1;

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
	std::weak_ptr<ChannelGroup> m_Group;
};

class ChannelGroup : public std::enable_shared_from_this<ChannelGroup>
{
public:
	~ChannelGroup() { }

	auto  Connections() -> std::vector<std::weak_ptr<::ChannelGroup>>& const { return m_Connected; }
	auto  Channels() -> std::vector<Channel*>& const { return m_Channels; }
	int   ChannelAmount()                   const { return m_ChannelAmount; }
	auto  EffectsGroup() -> EffectsGroup&   const { return m_EffectsGroup; }
	auto  Name() -> std::string& const { return m_Name; }
	int   ID()                   const { return m_Id; }
	float Volume()               const { return m_Volume; }
	bool  Muted()                const { return m_Muted; }
	bool  Mono()		         const { return m_Mono; }
	bool  IsInput()				 const { return m_IsInput; }
	float Pan()					 const { return m_Pan; }

	void  NextIter() { m_StartRound = true; }
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

	void ClearConnections() { 
		for (auto& i : m_Connected) 
			Disconnect(i); 
	}

	void ClearChannels() { 
		for (auto& i : m_Channels)
			i->Group(nullptr, -1);
	}

	bool Connected(const std::weak_ptr<::ChannelGroup>& other) const
	{
		auto& it = std::find_if(m_Connected.begin(), m_Connected.end(),
			[&](const std::weak_ptr<::ChannelGroup>& a)
			{
				if (auto c1 = a.lock())
					if (auto c2 = other.lock())
						return c1.get() == c2.get();
				return false;
			}
		);
		return it != m_Connected.end();
	}

	void Connect(const std::weak_ptr<::ChannelGroup>& other)
	{ 
		std::weak_ptr<::ChannelGroup> t = shared_from_this();
		if (auto c = other.lock())
			c->ConnectDirect(t), ConnectDirect(other);
	}

	void ConnectDirect(const std::weak_ptr<::ChannelGroup>& other)
	{
		if (auto c = other.lock())
			if (!Connected(other))
				m_Connected.push_back(other);
	}

	void Disconnect(const std::weak_ptr<::ChannelGroup>& other)
	{
		std::weak_ptr<::ChannelGroup> t = shared_from_this();
		if (auto c = other.lock()) 
			c->DisconnectDirect(t), DisconnectDirect(other);
	}

	void DisconnectDirect(const std::weak_ptr<::ChannelGroup>& other)
	{
		auto& it = std::find_if(m_Connected.begin(), m_Connected.end(), 
			[&] (const std::weak_ptr<::ChannelGroup>& a)
			{
				if (auto c1 = a.lock())
					if (auto c2 = other.lock())
						return c1.get() == c2.get();
				return false;
			}
		);

		if (it != m_Connected.end())
			m_Connected.erase(it);
	}

	void AddChannel(Channel* c)
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
			m_EffectsGroup.Channels(m_ChannelAmount + 1);
			m_Channels.push_back(c);
			c->Group(shared_from_this(), ChannelAmount());
			c->Mono(Mono()); 
			c->Mute(Muted());
			Pan(Pan());
			m_ChannelAmount++;
		}
	}

	void RemoveChannel(Channel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
		{
			m_ChannelAmount--;
			m_EffectsGroup.Channels(m_ChannelAmount);
			ClearConnections();
			m_Channels.erase(a);
			for (int i = 0; i < ChannelAmount(); i++)
				m_Channels[i]->Group(shared_from_this(), i);
			Pan(Pan());
		}

		if (ChannelAmount())
		{
			m_Name = m_Channels[0]->Name();
			m_Id = m_Channels[0]->ID();
		}
	}

	float DoEffects(float lvl, int indx)
	{
		return m_EffectsGroup.NextSample(lvl, indx);
	}

	float GetLevel(int id)
	{
		m_FirstMono = true;
		float level = 0;

		int amt = ChannelAmount();

		for (auto& i : m_Connected)
		{
			if (auto _group = i.lock())
			{
				int iamt = _group->ChannelAmount();
				if (_group->Mono())
					level += _group->Channels()[id % iamt]->Level();
			
				else
					for (int j = id; j < iamt; j += amt)
						level += _group->Channels()[j]->Level();
			}
		}
		
		return level;
	}

	float GetMonoLevel()
	{
		if (m_FirstMono)
		{
			m_MonoLevel = 0;
			for (auto& i : m_Channels)
				m_MonoLevel += i->UnprocessedLevel();

			m_MonoLevel /= ChannelAmount();
			m_FirstMono = false;
		}

		return m_MonoLevel;
	}

private:
	bool m_StartRound = false, 
		m_FirstMono = false;
	float m_Pan = 0;
	float m_MonoLevel = 0;

	int m_Id = -1, m_ChannelAmount = 0;
	float m_Volume = 1;
	bool m_Mono = false,
		m_Muted = false,
		m_IsInput = false;

	::EffectsGroup m_EffectsGroup;
	std::string m_Name = "APPLE";
	std::vector<Channel*> m_Channels;
	std::vector<std::weak_ptr<::ChannelGroup>> m_Connected;
};

class InputChannel : public Channel
{
public:
	using Channel::Channel;

	void Level(float l) override 
	{
		Channel::Level(l); 
		if (auto _group = m_Group.lock())
			_group->NextIter();
	}
	float Level() const override { return m_OutLevel; }

	void CalcLevel() override
	{
		if (auto _group = m_Group.lock())
		{
			if (m_Muted)
				m_OutLevel = 0;

			else if (m_Mono)
			{
				float _level = 0;
				for (auto& i : _group->Channels())
					_level += i->UnprocessedLevel();

				m_OutLevel = (_level / _group->ChannelAmount());
			}
			else
				m_OutLevel = m_Level;

			if (_group != nullptr)
				m_OutLevel = _group->DoEffects(m_OutLevel, m_GroupIndex) * m_Volume * m_Pan;
		
			return;
		}

		m_OutLevel = 0;
	}
};

class OutputChannel : public Channel
{
public:
	using Channel::Channel;
	
	void  CalcLevel()    override 
	{ 
		if (auto _group = m_Group.lock())
		{
			m_OutLevel = m_Muted ? 0 : _group->GetLevel(m_GroupIndex);
			return;
		}
		m_OutLevel = 0;
		return;
	}
	float Level()  const override 
	{
		if (auto _group = m_Group.lock())
		{
			float level = (m_Mono ? _group->GetMonoLevel() : m_OutLevel);

			level = _group->DoEffects(level, m_GroupIndex);

			return level * m_Volume * m_Pan;
		}
		return 0;
	}
};

class SoundboardChannel : public Channel
{
public:
	SoundboardChannel(Soundboard& soundBoard)
		: Channel(m_Counter++, "Soundboard", true), m_Soundboard(soundBoard)
	{ }

	void  CalcLevel() override 
	{
		m_OutLevel = m_Pan * m_Volume * m_Soundboard.GetLevel(ID()); 
	}
	
private:
	static inline int m_Counter = 0;
	Soundboard& m_Soundboard;
};

// -------------------------------------------------------------------------- \\
// -------------------------- SAR ASIO Device ------------------------------- \\
// -------------------------------------------------------------------------- \\

class AsioDevice
{
public:
	AsioDevice(Soundboard& soundboard);

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
	std::vector<SoundboardChannel>& SoundboardChannels() { return m_SoundboardChannels; }

private:
	double m_Samplerate;
	int m_BufferSize;
	
	PaStream* stream = nullptr;
	::Device* m_Device = nullptr;

	std::vector<InputChannel> m_Inputs;
	std::vector<OutputChannel> m_Outputs;
	std::vector<::SoundboardChannel> m_SoundboardChannels;
	std::vector<::Device> m_Devices;

	static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};