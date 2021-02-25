#pragma once
#include "pch.hpp"
#define MAX_CHANNELS 100

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

class OutputChannel
{
public:
	OutputChannel(int id, const std::string& name)
		: m_Id(id), m_Name(name)
	{}
	
	int   ID() const { return m_Id; }
	auto  Name() -> std::string& { return m_Name; }

	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void Level(float l) { m_Level = l; }
	void AddLevel(float l) { m_Level += l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float v) { m_TPeak = v; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }

	float Volume() { return m_Volume; }
	float Pan() { return m_Pan; }
	float Level() { return (1 - m_Muted) * m_Level * m_Volume * m_Pan; }
	float Peak() { return m_Peak; }
	bool  Muted() { return m_Muted; }
	bool  Mono() { return m_Mono; }
	float TPeak() { return m_TPeak; }

private:
	bool m_Mono = false,
		m_Muted = false;

	float m_Level = 0,
		m_Peak = 0, 
		m_TPeak = 0,
		m_Pan = 1,
		m_Volume = 1;

	std::string m_Name;
	int m_Id;
};

class InputChannels;
class OutputChannels;

class OutputChannels
{
public:
	using Type = OutputChannel;

	OutputChannels()
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			m_Connected[i] = nullptr;
		}
	}

	~OutputChannels()
	{}

	auto Name() -> std::string& { return Size() ? m_Channels[0]->Name() : m_Name; }
	int  ID() const { return Size() ? m_Channels[0]->ID() : -1; }

	void AddChannel(OutputChannel* c)
	{
		m_Channels.push_back(c);
	}

	void RemoveChannel(OutputChannel* c)
	{
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
			m_Channels.erase(a);
	}

	int Size() const { return m_Channels.size(); }
	std::vector<OutputChannel*>& Channels() { return m_Channels; }

	template<typename T>
	void Connect(T* in)
	{
		m_Connected[in->ID()] = in;
	}

	template<typename T>
	void Disconnect(T* in)
	{
		m_Connected[in->ID()] = nullptr;
	}

	template<typename T>
	void Clear()
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			if (m_Connected[i] != nullptr)
				static_cast<T*>(m_Connected[i])->Disconnect(this);

			m_Connected[i] = nullptr;
		}
	}

	float Volume()
	{
		return Size() ? m_Channels[0]->Volume() : 1;
	}

	void Volume(float v)
	{
		for (auto& c : m_Channels)
			c->Volume(v);
	}

	float Pan()
	{
		return m_Pan;
	}

	void Pan(float p)
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

	bool Muted()
	{
		return Size() ? m_Channels[0]->Muted() : false;
	}

	void Mute(bool v)
	{
		for (auto& c : m_Channels)
			c->Mute(v);
	}

	bool Mono()
	{
		return Size() ? m_Channels[0]->Muted() : false;
	}

	void Mono(bool v)
	{
		for (auto& c : m_Channels)
			c->Mono(v);
	}

private:
	float m_Pan = 0;
	std::string m_Name = "APPLE";
	std::vector<OutputChannel*> m_Channels;
	void* m_Connected[MAX_CHANNELS];
};

// -------------------------------------------------------------------------- \\
// --------------------------- Input Channel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class InputChannel
{
public:
	InputChannel(int id, const std::string& name)
		: m_Id(id), m_Name(name)
	{
		Clear();
	}

	int  ID() const { return m_Id; }
	auto Name() -> std::string& { return m_Name; }

	bool Connected(OutputChannel* out) const { return m_Connected[out->ID()] != nullptr; }
	void Connect(OutputChannel* out) { m_Connected[out->ID()] = out;  }
	void Disconnect(OutputChannel* out) { m_Connected[out->ID()] = nullptr;  }
	auto Connections() -> OutputChannel** { return m_Connected; }
	void Clear() { for (int i = 0; i < MAX_CHANNELS; i++) m_Connected[i] = nullptr; }

	void Volume(float v) { m_Volume = v; }
	void Pan(float p) { m_Pan = p; }
	void Level(float l) { m_Level = l; }
	void Peak(float l) { m_Peak = l; }
	void TPeak(float l) { m_TPeak = l; }
	void Mute(bool v) { m_Muted = v; }
	void Mono(bool v) { m_Mono = v; }

	float Volume() { return m_Volume; }
	float Pan() { return m_Pan; }
	float Level() { return (1 - m_Muted) * m_Level * m_Volume * m_Pan; }
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

	std::string m_Name;
	int m_Id;

	OutputChannel* m_Connected[MAX_CHANNELS];
};


class InputChannels
{
public:
	using Type = InputChannel;

	InputChannels()
	{
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			m_Connected[i] = nullptr;
		}
	}

	~InputChannels()
	{}

	auto Name() -> std::string& { return Size() ? m_Channels[0]->Name() : m_Name; }
	int  ID() const { return Size() ? m_Channels[0]->ID() : -1; }
	bool Connected(OutputChannels* out) const { return m_Connected[out->ID()] != nullptr; }
	auto Connections() -> OutputChannels** { return m_Connected; }
	int  Size() const { return m_Channels.size(); }
	std::vector<InputChannel*>& Channels() { return m_Channels; }
	
	void Connect(OutputChannels* out) 
	{ 
		for (int i = 0; i < Size(); i++)
		{
			int index = i % out->Size();
			m_Channels[i]->Connect(out->Channels()[index]);
		}
		
		m_Connected[out->ID()] = out; 
		out->Connect(this);
	}
	
	void Disconnect(OutputChannels* out) 
	{ 
		for (int i = 0; i < Size(); i++)
		{
			int index = i % out->Size();
			m_Channels[i]->Disconnect(out->Channels()[index]);
		}
		if (m_Connected[out->ID()])
			m_Connected[out->ID()]->Disconnect(this);
		
		m_Connected[out->ID()] = nullptr;
	}
	
	void Clear() 
	{ 
		for (int i = 0; i < MAX_CHANNELS; i++) 
		{
			if (m_Connected[i] != nullptr)
			{
				Disconnect(m_Connected[i]);
			}
		}
	}

	void AddChannel(InputChannel* c)
	{		
		Clear();
		m_Channels.push_back(c);
		for (int i = 0; i < MAX_CHANNELS; i++)
		{
			if (m_Connected[i] != nullptr)
			{
				int index = (Size() - 1) % m_Connected[i]->Size();
				c->Connect(m_Connected[i]->Channels()[index]);
				m_Connected[i]->Connect(this);
			}
		}
		c->Mono(Mono());
		c->Mute(Muted());
		Pan(Pan());
	}

	void RemoveChannel(InputChannel* c)
	{
		Clear();
		auto& a = std::find(m_Channels.begin(), m_Channels.end(), c);
		if (a != m_Channels.end())
			m_Channels.erase(a);
	}

	float Volume()
	{
		return Size() ? m_Channels[0]->Volume() : 1;
	}

	void Volume(float v) 
	{
		for (auto& c : m_Channels)
			c->Volume(v);
	}

	float Pan()
	{
		return m_Pan;
	}

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

	bool Muted()
	{
		return Size() ? m_Channels[0]->Muted() : false;
	}

	void Mute(bool v) 
	{
		for (auto& c : m_Channels)
			c->Mute(v);
	}

	bool Mono()
	{
		return Size() ? m_Channels[0]->Muted() : false;
	}

	void Mono(bool v)
	{
		for (auto& c : m_Channels)
			c->Mono(v);
	}

	void Level(int i, float v) 
	{
		m_Channels[i]->Level(v);
	};

private:
	float m_Pan = 0;
	std::string m_Name = "WOOF";
	std::vector<InputChannel*> m_Channels;
	OutputChannels* m_Connected[MAX_CHANNELS];
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