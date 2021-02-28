#pragma once
#include "pch.hpp"

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
class InputChannelGroup;
class OutputChannelGroup;

class OutputChannel
{
public:
	OutputChannel(int id, const std::string& name)
		: m_Id(id), m_Name(name)
	{}
	
	template<typename T>
	void Connect(T* in) 
	{ 
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), in);
		if (it == m_Connected.end() && in != nullptr)
			m_Connected.push_back(in);
	}

	template<typename T>
	void Disconnect(T* in)
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), in);
		if (it != m_Connected.end())
			m_Connected.erase(it);
	}

	auto Name() -> std::string& { return m_Name; }
	void Group(void* p) { m_Group = p; }
	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void Level(float l) { m_Level = l; }
	void MonoLevel(float l) { m_MonoLevel = l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float v) { m_TPeak = v; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }

	int	  ID() const { return m_Id; }
	float Volume() { return m_Volume; }
	float Pan() { return m_Pan; }
	float Peak() { return m_Peak; }
	bool  Muted() { return m_Muted; }
	bool  Mono() { return m_Mono; }
	float TPeak() { return m_TPeak; }
	float RLevel() { return m_Level; }
	float MonoLevel() { return m_MonoLevel; }

	template<typename T = InputChannel>
	void CalcLevel()
	{
		if (m_Muted)
			return;

		float _level = 0, _monoLevel = 0;
		for (auto& i : m_Connected)
			if (static_cast<T*>(i)->Mono())
				_monoLevel += static_cast<T*>(i)->Level();
			else
				_level += static_cast<T*>(i)->Level();
		Level(_level);
		MonoLevel(_monoLevel);
	}

	template<typename T = InputChannel>
	float Level() 
	{ 
		if (m_Muted)
			return 0;

		float _clevel;
		if (m_Mono && m_Group != nullptr)
		{
			auto a = static_cast<OutputChannelGroup*>(m_Group);
			float level = 0;
			for (auto& i : a->Channels())
				level += i->RLevel();

			_clevel = level / a->Size();
		}
		else
		{
			_clevel = m_Level;
		}

		float monoLevels = 0;
		int amt = 0;
		if (m_Group != nullptr)
		{
			auto a = static_cast<OutputChannelGroup*>(m_Group);
			for (auto& i : a->Channels())
			{
				if (!i->MonoLevel())
					continue;

				amt++;
				monoLevels += i->MonoLevel();
			}
		}

		if (amt)
			monoLevels /= amt;

		return (monoLevels + _clevel) * m_Volume * m_Pan;
	}

private:
	bool m_Mono = false,
		m_Muted = false;

	float m_Level = 0, m_MonoLevel = 0,
		m_Peak = 0, 
		m_TPeak = 0,
		m_Pan = 1,
		m_Volume = 1;

	void* m_Group = nullptr;

	std::string m_Name;
	int m_Id;
	std::vector<void*> m_Connected;
};

// Collection of OutputChannel objects
class OutputChannelGroup
{
public:
	using Type = OutputChannel;

	OutputChannelGroup()
	{}

	~OutputChannelGroup()
	{
		//Clear();
	}

	auto Name() -> std::string& { return Size() ? m_Channels[0]->Name() : m_Name; }
	int  ID() const { return Size() ? m_Channels[0]->ID() : -1; }
	int  Size() const { return m_Channels.size(); }
	std::vector<OutputChannel*>& Channels() { return m_Channels; }
	
	void AddChannel(OutputChannel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a == m_Channels.end() && c != nullptr)
		{
			Clear();
			if (Size() == 0)
			{
				c->Mono(false);
				c->Mute(false);
			}
			m_Channels.push_back(c);
			c->Group(this);
			c->Mono(Mono());
			c->Mute(Muted());
			Pan(Pan());
		}
	}

	void RemoveChannel(OutputChannel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
		{
			Clear();
			m_Channels.erase(a);
		}
	}

	template<typename T>
	void Connect(T* in)
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), in);
		if (it == m_Connected.end() && in != nullptr)
			m_Connected.push_back(in);
	}

	template<typename T>
	void Disconnect(T* in)
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), in);
		if (it != m_Connected.end())
			m_Connected.erase(it);
	}

	template<typename T = InputChannelGroup>
	void Clear()
	{
		for (auto& i : m_Connected)
		{
			static_cast<T*>(i)->Disconnect(this);
			Disconnect(i);
		}
	}

	float Volume() { return Size() ? m_Channels[0]->Volume() : 1; }
	void  Volume(float v) { for (auto& c : m_Channels) c->Volume(v); }

	float Pan() { return m_Pan; }
	void  Pan(float p)
	{
		m_Pan = p;
		int _index = 0;
		double _p = -p / 50.0;
		double _a = 1.0 - std::abs((Size() - 1) / 2.0 * _p);
		for (auto& c : m_Channels)
		{
			float _pan = constrain(_p * (_index - (Size() - 1) / 2.0) + _a, 0.0, 1.0);
			c->Pan(_pan);
			_index++;
		}
	}

	bool Muted() { return Size() ? m_Channels[0]->Muted() : false; }
	void Mute(bool v) { for (auto& c : m_Channels) c->Mute(v); }
	bool Mono() { return Size() ? m_Channels[0]->Mono() : false; }
	void Mono(bool v) { for (auto& c : m_Channels) c->Mono(v); }

private:
	float m_Pan = 0;
	std::string m_Name = "APPLE";
	std::vector<OutputChannel*> m_Channels;
	std::vector<void*> m_Connected;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Input Channel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class InputChannel
{
public:
	InputChannel(int id, const std::string& name)
		: m_Id(id), m_Name(name)
	{}

	int  ID() const { return m_Id; }
	auto Name() -> std::string& { return m_Name; }

	void Connect(OutputChannel* out) 
	{ 
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), out);
		if (it == m_Connected.end() && out != nullptr)
			m_Connected.push_back(out);
	}

	void Disconnect(OutputChannel* out) 
	{ 
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), out);
		if (it != m_Connected.end())
			m_Connected.erase(it);
	}

	void Group(void* p) { m_Group = p; }
	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void Level(float l) { m_Level = l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float l) { m_TPeak = l; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }

	float Volume() { return m_Volume; }
	float Pan() { return m_Pan; }

	float RLevel() { return m_Level; }

	template<typename T = InputChannelGroup>
	float Level() 
	{
		if (m_Muted)
			return 0;

		if (m_Mono && m_Group != nullptr)
		{
			auto a = static_cast<T*>(m_Group);
			float level = 0;
			for (auto& i : a->Channels())
				level += i->RLevel();
			
			level *= m_Pan * m_Volume;
			return level /= a->Size();
		}

		return m_Level * m_Volume * m_Pan; 
	}

	float Peak() { return m_Peak; }
	float TPeak() { return m_TPeak; }
	bool  Muted() { return m_Muted; }
	bool  Mono() { return m_Mono; }

private:
	bool m_Mono = false,
		m_Muted = false;

	float m_Level = 0,
		m_Peak = 0, 
		m_TPeak = 0,
		m_Pan = 1,
		m_Volume = 1;

	void* m_Group = nullptr;

	std::string m_Name;
	int m_Id;

	std::vector<OutputChannel*> m_Connected;
};

// Collection of InputChannel objects
class InputChannelGroup
{
public:
	using Type = InputChannel;

	InputChannelGroup()
	{}

	~InputChannelGroup()
	{
		//Clear();
	}

	auto Name() -> std::string& { return Size() ? m_Channels[0]->Name() : m_Name; }
	int  ID() const { return Size() ? m_Channels[0]->ID() : -1; }
	auto Connections() -> std::vector<OutputChannelGroup*> { return m_Connected; }
	int  Size() const { return m_Channels.size(); }
	std::vector<InputChannel*>& Channels() { return m_Channels; }
	
	bool Connected(OutputChannelGroup* out) const 
	{
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), out);
		return it != m_Connected.end();
	}
	
	void Connect(OutputChannelGroup* out) 
	{ 
		// Make sure it isn't already connected
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), out);
		if (it == m_Connected.end() && out != nullptr)
		{
			// Add to connected and connect this to out
			m_Connected.push_back(out);
			out->Connect(this);

			// Then connect all channels
			for (int i = 0; i < Size(); i++)
			{
				m_Channels[i]->Connect(out->Channels()[i % out->Size()]);
				out->Channels()[i % out->Size()]->Connect(m_Channels[i]);
			}
		}
	}
	
	void Disconnect(OutputChannelGroup* out) 
	{ 
		// Make sure it isn't already connected
		auto& it = std::find(m_Connected.begin(), m_Connected.end(), out);
		if (it != m_Connected.end())
		{
			// Remove the connection
			m_Connected.erase(it);
			out->Disconnect(this);

			// Then disconnect all channels
			for (int i = 0; i < Size(); i++)
			{
				m_Channels[i]->Disconnect(out->Channels()[i % out->Size()]);
				out->Channels()[i % out->Size()]->Disconnect(m_Channels[i]);
			}
		}
	}
	
	void Clear() 
	{ 
		for (auto& i : m_Connected)
			Disconnect(i);
	}

	void AddChannel(InputChannel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a == m_Channels.end() && c != nullptr)
		{
			Clear();
			m_Channels.push_back(c);
			c->Group(this);
			c->Mono(Mono());
			c->Mute(Muted());
			Pan(Pan());
		}
	}

	void RemoveChannel(InputChannel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
		{
			Clear();
			m_Channels.erase(a);
		}
	}

	float Volume() { return Size() ? m_Channels[0]->Volume() : 1; }
	void  Volume(float v) { for (auto& c : m_Channels) c->Volume(v); }

	float Pan() { return m_Pan; }
	void Pan(float p) 
	{
		m_Pan = p;
		int _index = 0;
		double _p = - p / 50.0;
		double _a = 1.0 - std::abs((Size() - 1) / 2.0 * _p);
		for (auto& c : m_Channels)
		{
			float _pan = constrain(_p * (_index - (Size() - 1) / 2.0) + _a, 0.0, 1.0);
			c->Pan(_pan);
			_index++;
		}
	}

	bool Muted() { return Size() ? m_Channels[0]->Muted() : false; }
	void Mute(bool v) { for (auto& c : m_Channels) c->Mute(v); }
	bool Mono() { return Size() ? m_Channels[0]->Mono() : false; }
	void Mono(bool v) { for (auto& c : m_Channels) c->Mono(v); }

private:
	float m_Pan = 0;
	std::string m_Name = "WOOF";
	std::vector<InputChannel*> m_Channels;
	std::vector<OutputChannelGroup*> m_Connected;
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