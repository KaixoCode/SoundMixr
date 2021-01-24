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

class StereoOutputChannel
{
public:
	StereoOutputChannel(int l, int r, const std::string& name)
		: m_Left(l), m_Right(r), m_Name(name)
	{}
	
	int   ID() { return m_Left; }
	auto  Name() -> std::string& { return m_Name; }
	void  Volume(float v) { m_Volume = v; }
	float Volume() { return m_Volume; }

private:
	std::string m_Name;
	int m_Left,
		m_Right;

	float m_Volume = 1;
};

// -------------------------------------------------------------------------- \\
// --------------------------- Input Channel -------------------------------- \\
// -------------------------------------------------------------------------- \\

class StereoInputChannel
{
public:
	StereoInputChannel(int l, int r, const std::string& name)
		: m_Left(l), m_Right(r), m_Name(name)
	{}

	int   ID() { return m_Left; }
	auto  Name() -> std::string& { return m_Name; }
	void  Volume(float v) { m_Volume = v; }
	float Volume() { return m_Volume; }
	bool  Connected(StereoOutputChannel* out) const { return m_Connected.find(out->ID()) != m_Connected.end(); }
	void  Connect(StereoOutputChannel* out) { m_Connected.emplace(out->ID(), out); }
	void  Disconnect(StereoOutputChannel* out) { m_Connected.erase(out->ID()); }

private:
	std::string m_Name;
	int m_Left,
		m_Right;

	float m_Volume = 1;
	std::unordered_map<int, StereoOutputChannel*> m_Connected;
};

// -------------------------------------------------------------------------- \\
// -------------------------- SAR ASIO Device ------------------------------- \\
// -------------------------------------------------------------------------- \\

class SarAsio
{
public:
	SarAsio();

	~SarAsio();

	auto Device() -> ::Device& { return *m_Device; }
	bool StreamRunning() { return Pa_IsStreamActive(stream); }
	bool OpenStream();
	void CloseStream();
	bool StartStream();
	bool StopStream();

	std::vector<StereoInputChannel>& Inputs() { return m_Inputs; }
	std::vector<StereoOutputChannel>& Outputs() { return m_Outputs; }

private:
	int m_Samplerate,
		m_BufferSize;
	
	PaStream* stream = nullptr;
	std::unique_ptr<::Device> m_Device;

	std::vector<StereoInputChannel> m_Inputs;
	std::vector<StereoOutputChannel> m_Outputs;

	static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};