#include "audio/Channel.hpp"

// -------------------------------------------------------------------------- \\
// ------------------------------ Channel ----------------------------------- \\
// -------------------------------------------------------------------------- \\

void Channel::Group(::ChannelGroup* p, int index)
{
	if (!m_Group)
	{
		m_Group = p;
		m_GroupIndex = index;
	}
	else if (m_Group->Lock())
	{
		m_Group->Unlock();
		m_Group = p;
		m_GroupIndex = index;
	}
	else
	{
		m_Group = nullptr;
		m_GroupIndex = -1;
	}
}

// -------------------------------------------------------------------------- \\
// ---------------------------- ChannelGroup -------------------------------- \\
// -------------------------------------------------------------------------- \\

ChannelGroup::~ChannelGroup() 
{
	m_Dead = true; 
	m_Mutex.lock(); 
	m_Mutex.unlock();
}

void ChannelGroup::Pan(float p)
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

void ChannelGroup::ClearConnections() {
	for (auto& i : m_Connected)
		Disconnect(i);
}

void ChannelGroup::ClearChannels() {
	for (auto& i : m_Channels)
		i->Group(nullptr, -1);
}

bool ChannelGroup::Connected(ChannelGroup* other) const
{
	auto& it = std::find(m_Connected.begin(), m_Connected.end(), other);
	return it != m_Connected.end() && other != nullptr;
}

void ChannelGroup::Connect(::ChannelGroup* other)
{
	if (!Connected(other))
		other->ConnectDirect(this), ConnectDirect(other);
}

void ChannelGroup::ConnectDirect(ChannelGroup* other)
{
	if (!Connected(other))
		m_Connected.push_back(other);
}

void ChannelGroup::Disconnect(::ChannelGroup* other)
{
	if (Connected(other))
		other->DisconnectDirect(this), DisconnectDirect(other);
}

void ChannelGroup::DisconnectDirect(::ChannelGroup* other)
{
	auto& it = std::find(m_Connected.begin(), m_Connected.end(), other);
	if (it != m_Connected.end())
		m_Connected.erase(it);
}

void ChannelGroup::AddChannel(Channel* c)
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
		c->Group(this, ChannelAmount());
		c->Mono(Mono());
		c->Mute(Muted());
		Pan(Pan());
		m_ChannelAmount++;
	}
}

void ChannelGroup::RemoveChannel(Channel* c)
{
	auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
	if (a != m_Channels.end())
	{
		m_ChannelAmount--;
		m_EffectsGroup.Channels(m_ChannelAmount);
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

float ChannelGroup::DoEffects(float lvl, int indx)
{
	return m_EffectsGroup.NextSample(lvl, indx);
}

float ChannelGroup::GetLevel(int id)
{
	m_FirstMono = true;
	float level = 0;

	int amt = ChannelAmount();

	for (auto& i : m_Connected)
	{
		if (i->Lock())
		{
			int iamt = i->ChannelAmount();
			if (i->Mono())
				level += i->Channels()[id % iamt]->Level();

			else
				for (int j = id; j < iamt; j += amt)
					level += i->Channels()[j]->Level();
			i->Unlock();
		}
	}

	return level;
}

float ChannelGroup::GetMonoLevel()
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

// -------------------------------------------------------------------------- \\
// ---------------------------- InputChannel -------------------------------- \\
// -------------------------------------------------------------------------- \\

void InputChannel::CalcLevel() 
{
	if (m_Group != nullptr && m_Group->Lock())
	{
		if (m_Muted)
			m_OutLevel = 0;

		else if (m_Mono)
		{
			float _level = 0;
			for (auto& i : m_Group->Channels())
				_level += i->UnprocessedLevel();

			m_OutLevel = (_level / m_Group->ChannelAmount());
		}
		else
			m_OutLevel = m_Level;

		m_OutLevel = m_Group->DoEffects(m_OutLevel, m_GroupIndex) * m_Volume * m_Pan;

		m_Group->Unlock();
		return;
	}

	m_OutLevel = 0;
}

// -------------------------------------------------------------------------- \\
// ---------------------------- OutputChannel ------------------------------- \\
// -------------------------------------------------------------------------- \\

void OutputChannel::CalcLevel()
{
	if (m_Group != nullptr && m_Group->Lock())
	{
		m_OutLevel = m_Level = m_Muted ? 0 : m_Group->GetLevel(m_GroupIndex);
		m_Group->Unlock();
		return;
	}

	m_OutLevel = 0;
	return;
}

float OutputChannel::Level() const
{
	if (!m_Muted && m_Group != nullptr && m_Group->Lock())
	{
		float level = (m_Mono ? m_Group->GetMonoLevel() : m_OutLevel);
		level = m_Group->DoEffects(level, m_GroupIndex);
		m_Group->Unlock();
		return level * m_Volume * m_Pan;
	}

	return 0;
}

// -------------------------------------------------------------------------- \\
// -------------------------- SoundboardChannel ----------------------------- \\
// -------------------------------------------------------------------------- \\

void SoundboardChannel::CalcLevel() 
{ 
	m_OutLevel = m_Pan * m_Volume * m_Soundboard.GetLevel(ID()); 
}
