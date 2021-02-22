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

class StereoOutputChannel
{
public:
	StereoOutputChannel(int l, int r, const std::string& name)
		: m_Left(l), m_Right(r), m_Name(name)
	{}
	
	int   ID() const { return m_Left; }
	auto  Name() -> std::string& const { return m_Name; }

	float level_left = 0, 
		level_right = 0,
		peak_left = 0,
		peak_right = 0, 
		pan = 0, volume = 1;

	bool mono = false,
		muted = false;

private:
	std::string m_Name;
	int m_Left,
		m_Right;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Input Channel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class StereoInputChannel
{
public:
	StereoInputChannel(int l, int r, const std::string& name)
		: m_Left(l), m_Right(r), m_Name(name)
	{
		Clear();
	}

	int  ID() { return m_Left; }
	auto Name() -> std::string& { return m_Name; }
	bool Connected(StereoOutputChannel* out) const { return m_Connected[out->ID()] != nullptr; }
	void Connect(StereoOutputChannel* out) { m_Connected[out->ID()] = out;  }
	void Disconnect(StereoOutputChannel* out) { m_Connected[out->ID()] = nullptr;  }
	auto Connections() -> StereoOutputChannel** { return m_Connected; }
	void Clear() { for (int i = 0; i < MAX_CHANNELS; i++) m_Connected[i] = nullptr; }

	float level_left = 0,
		level_right = 0,
		peak_left = 0,
		peak_right = 0,
		pan = 0, volume = 1;

	bool mono = false,
		muted = false;

private:
	std::string m_Name;
	int m_Left,
		m_Right;

	StereoOutputChannel* m_Connected[MAX_CHANNELS];
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
	void SaveRouting();

	std::vector<StereoInputChannel>& Inputs() { return m_Inputs; }
	std::vector<StereoOutputChannel>& Outputs() { return m_Outputs; }

private:
	double m_Samplerate;
	int m_BufferSize;
	
	PaStream* stream = nullptr;
	::Device* m_Device = nullptr;

	std::vector<StereoInputChannel> m_Inputs;
	std::vector<StereoOutputChannel> m_Outputs;
	std::vector<::Device> m_Devices;

	static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};