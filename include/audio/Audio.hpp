#pragma once
#include "pch.hpp"
#include "audio/Channel.hpp"

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
};

class Audio
{
public:
	Audio(bool in = true) : m_Input(in) 
	{ 
		UpdateDeviceList(); 
	}

	~Audio()
	{
		// Stop the stream
		try {
			if (m_Running && m_Audio.isStreamOpen() && m_Audio.isStreamRunning())
				m_Audio.stopStream();
		}
		catch (RtAudioError& e) {
			e.printMessage();
		}

		if (m_Running && m_Audio.isStreamOpen())
			m_Audio.closeStream();
	}

	::RtAudio& RtAudio() { return m_Audio; }
	::Device&  Device() { return m_Device; }

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

	bool SetDevice(const ::Device& d)
	{
		m_Device = d;
		return SetSamplerate(d.preferredSampleRate);
	}

	int Samplerate() { return m_Samplerate; }
	bool SetSamplerate(int r)
	{
		int prev = m_Samplerate;
		m_Samplerate = r;
		if (!UpdateStream())
		{
			m_Samplerate = prev;
		}
		else return true;

		int index = 0;
		while (!UpdateStream())
		{
			m_Samplerate = m_Device.sampleRates[index];
			index++;
			if (index > m_Device.sampleRates.size())
				return false;
		}
		return true;
	}

	int Buffersize() { return m_Buffersize; }
	bool SetBuffersize(int s)
	{
		int prev = m_Buffersize;
		m_Buffersize = s;
		if (!UpdateStream())
		{
			
			m_Buffersize = prev;
			return UpdateStream();
		}
	}

	bool UpdateStream()
	{
		if (m_Audio.isStreamOpen())
			m_Audio.closeStream();

		m_Running = false;

		RtAudio::StreamParameters parameters;
		parameters.deviceId = m_Device.id;
		parameters.nChannels = m_Input ? m_Device.inputChannels : m_Device.outputChannels;
		parameters.firstChannel = 0;
		unsigned int sampleRate = m_Samplerate;
		unsigned int bufferFrames = m_Buffersize;
		try {
			if (m_Input)
				m_Audio.openStream(NULL, &parameters, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &InputCallback, (void*)this);
			else 
				m_Audio.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &OutputCallback, (void*)this);

			m_Audio.startStream();
			m_Running = true;
		}
		catch (RtAudioError& e) {
			e.printMessage();
			return false;
		}
		return true;
	}

	void Connect(int id, Channel& c)
	{
		m_Channels.emplace(id, c);
	}

	double avg = 0;
protected:
	std::vector<::Device> m_Devices;
	std::unordered_map<int, std::reference_wrapper<Channel>> m_Channels;
	::Device  m_Device;
	::RtAudio m_Audio;

	int m_Samplerate = -1, 
		m_Buffersize = 256;

	bool m_Input = false,
		m_Running = false;



	static int InputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		double* _buffer = (double*)inputBuffer;
		Audio& _me = *(Audio*)userData;
		int _bufferSize = nBufferFrames * _me.Device().inputChannels;
		double a = 0;
		for (int i = 0; i < _bufferSize; i++)
		{
			double _sample = *_buffer++;
			a += _sample;
			for (auto& _c : _me.m_Channels)
			{
				_c.second.get().enqueue(_sample);
			}
		}
		a /= (double)_bufferSize;
		_me.avg = a;

		return 0;
	}

	static int OutputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		double* _buffer = (double*)outputBuffer;
		Audio& _me = *(Audio*)userData;
		int _bufferSize = nBufferFrames * _me.Device().outputChannels;
		
		for (int i = 0; i < _bufferSize; i++)
		{
			double _sample = 0;
			
			for (auto& _c : _me.m_Channels)
			{
				double _t = 0;
				_c.second.get().try_dequeue(_t);
				_sample += _t;
			};

			*_buffer++ = _sample;
		}

		return 0;
	}
};

