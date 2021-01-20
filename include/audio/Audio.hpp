#pragma once
#include "pch.hpp"

class Device : public RtAudio::DeviceInfo
{
public:
	Device()
		: id(-1)
	{}

	Device(unsigned int id, RtAudio::DeviceInfo& a)
		: id(id), RtAudio::DeviceInfo(a)
	{}

	unsigned int id;
	
	void Print()
	{
		LOG(name << 
			"\n  Output Channels: " << outputChannels << 
			"\n  Input Channels:  " << inputChannels <<
			"\n  Duplex Channels: " << duplexChannels <<
			"\n  Default Output:  " << isDefaultOutput <<
			"\n  Default Input:   " << isDefaultInput <<
			"\n  Prefered Rate:   " << preferredSampleRate
		);
	}
};

class Audio
{
public:
	Audio(bool in = true) : m_Input(in) { UpdateDeviceList(); }

	const std::vector<::Device> const& Devices() const { return m_Devices; }
	
	void UpdateDeviceList()
	{
		unsigned int devices = m_Audio.getDeviceCount();
		RtAudio::DeviceInfo info;
		for (unsigned int i = 0; i < devices; i++) {
			info = m_Audio.getDeviceInfo(i);
			if (info.probed == true)
				m_Devices.emplace_back(::Device{ i, info });
		}
	}

	void PrintDevices()
	{
		for (::Device& a : m_Devices)
			a.Print();
	}

	int SetDevice(const ::Device& d, int sr = -1)
	{
		m_Device = d;
		if (m_Audio.isStreamOpen())
			m_Audio.closeStream();

		RtAudio::StreamParameters parameters;
		parameters.deviceId = m_Device.id;
		parameters.nChannels = m_Input ? m_Device.inputChannels : m_Device.outputChannels;
		parameters.firstChannel = 0;
		unsigned int sampleRate = sr == -1 ? m_Device.preferredSampleRate : sr;
		unsigned int bufferFrames = 256; // 256 sample frames

		try {
			if (m_Input)
				m_Audio.openStream(NULL, &parameters, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &InputCallback, (void*)this);
			else 
				m_Audio.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &OutputCallback, (void*)this);

			m_Audio.startStream();
		}
		catch (RtAudioError& e) {
			e.printMessage();
			return -1;
		}
		return 0;
	}

	RtAudio& GetAudio() { return m_Audio; }

	::Device& Device()
	{
		return m_Device;
	}

protected:
	bool m_Input = false;
	::Device m_Device;
	std::vector<::Device> m_Devices;
	RtAudio m_Audio;

	static int InputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		return 0;
	}

	static int OutputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		double* buffer = (double*)outputBuffer;
		Audio& me = *(Audio*)userData;

		for (int i = 0; i < nBufferFrames * me.Device().outputChannels; i++)
			*buffer++ = 1;

		return 0;
	}
};

