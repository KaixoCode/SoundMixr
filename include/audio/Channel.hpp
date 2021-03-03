#pragma once
#include "pch.hpp"
#include "audio/Effects.hpp"
#include "ui/Soundboard.hpp"

class ChannelGroup;

// -------------------------------------------------------------------------- \\
// ------------------------------ Channel ----------------------------------- \\
// -------------------------------------------------------------------------- \\

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
	void Group(::ChannelGroup* p, int index);

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
	ChannelGroup* m_Group = nullptr;
};

// -------------------------------------------------------------------------- \\
// ---------------------------- ChannelGroup -------------------------------- \\
// -------------------------------------------------------------------------- \\

class ChannelGroup
{
public:
	~ChannelGroup();

	auto  Connections() -> std::vector<::ChannelGroup*>& const { return m_Connected; }
	auto  Channels()           -> std::vector<Channel*>& const { return m_Channels; }

	int   ChannelAmount()                 const { return m_ChannelAmount; }
	auto  EffectsGroup() -> EffectsGroup& const { return m_EffectsGroup; }
	auto  Name()          -> std::string& const { return m_Name; }

	int   ID()      const { return m_Id; }
	float Volume()  const { return m_Volume; }
	bool  Muted()   const { return m_Muted; }
	bool  Mono()	const { return m_Mono; }
	bool  IsInput()	const { return m_IsInput; }
	float Pan()		const { return m_Pan; }
	bool  Lock()	const { if (!m_Dead) m_Mutex.lock(); return !m_Dead; }
	void  Unlock()	const { m_Mutex.unlock(); }

	void  Volume(float v) { for (auto& c : m_Channels) c->Volume(v); m_Volume = v; }
	void  Mute(bool v) { for (auto& c : m_Channels) c->Mute(v); m_Muted = v; }
	void  Mono(bool v) { for (auto& c : m_Channels) c->Mono(v); m_Mono = v; }
	void  Pan(float p);

	void ClearConnections();
	void ClearChannels();

	bool Connected(ChannelGroup* other) const;
	void Connect(::ChannelGroup* other);
	void ConnectDirect(ChannelGroup* other);
	void Disconnect(::ChannelGroup* other);
	void DisconnectDirect(::ChannelGroup* other);
	void AddChannel(Channel* c);
	void RemoveChannel(Channel* c);

	float DoEffects(float lvl, int indx);
	float GetLevel(int id);
	float GetMonoLevel();

private:
	float m_Pan = 0,
		m_MonoLevel = 0,
		m_Volume = 1;

	int m_Id = -1, 
		m_ChannelAmount = 0;

	bool m_FirstMono = false,
		m_Mono = false,
		m_Muted = false,
		m_IsInput = false,
		m_Dead = false;

	mutable std::mutex m_Mutex;

	::EffectsGroup m_EffectsGroup;
	std::string m_Name = "";
	std::vector<Channel*> m_Channels;
	std::vector<::ChannelGroup*> m_Connected;
};

// -------------------------------------------------------------------------- \\
// ---------------------------- InputChannel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class InputChannel : public Channel
{
public:
	using Channel::Channel;

	void CalcLevel() override;
};

// -------------------------------------------------------------------------- \\
// ---------------------------- OutputChannel ------------------------------- \\
// -------------------------------------------------------------------------- \\

class OutputChannel : public Channel
{
public:
	using Channel::Channel;

	void  CalcLevel() override;
	float Level() const override;
};

// -------------------------------------------------------------------------- \\
// -------------------------- SoundboardChannel ----------------------------- \\
// -------------------------------------------------------------------------- \\

class SoundboardChannel : public Channel
{
public:
	SoundboardChannel(Soundboard& soundBoard)
		: Channel(m_Counter++, "Soundboard", true), m_Soundboard(soundBoard)
	{}

	void CalcLevel() override;

private:
	static inline int m_Counter = 0;
	Soundboard& m_Soundboard;
};