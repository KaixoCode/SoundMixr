#pragma once
#include "pch.hpp"
#include "audio/Channel.hpp"

class Device : public RtAudio::DeviceInfo
{
public:
	Device()
		: id(-1)
	{}

	Device(unsigned int id, RtAudio::DeviceInfo& a, RtAudio::Api api = RtAudio::Api::WINDOWS_WASAPI)
		: id(id), RtAudio::DeviceInfo(a), api(api)
	{}

	unsigned int id;
	RtAudio::Api api;
};



class AudioDevice
{
public:
	AudioDevice(const Device& device, bool in, int samplerate = -1, int bufferSize = 256)
		: m_BufferSize(bufferSize), m_Device(device), m_Input(in), m_Audio(device.api),
		m_Samplerate(samplerate == -1 ? device.preferredSampleRate : samplerate)
	{}

	int  Samplerate() { return m_Samplerate; };
	bool Samplerate(int s) { m_Samplerate = s; return true; }
	::RtAudio& RtAudio() { return m_Audio; }
	::Device&  Device() { return m_Device; }

	bool OpenDevice() 
	{
		try {
			LOG("Attempting to open " << m_Device.name << "\n");
			if (m_Audio.isStreamOpen())
				m_Audio.closeStream();

			RtAudio::StreamParameters parameters;
			parameters.deviceId = m_Device.id;
			parameters.nChannels = m_Input ? m_Device.inputChannels : m_Device.outputChannels;
			parameters.firstChannel = 0;

			RtAudio::StreamOptions options;
			options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;

			unsigned int sampleRate = m_Samplerate;
			unsigned int bufferFrames = m_BufferSize;


			if (m_Input)
				m_Audio.openStream(NULL, &parameters, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &InputCallback, (void*)this, &options);
			else 
				m_Audio.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &OutputCallback, (void*)this, &options);

			LOG("Opened stream (" << m_Device.name << ")" <<
				"\n type:       " << (m_Input ? "Input" : "Output") << 
				"\n samplerate: " << m_Samplerate << 
				"\n buffersize: " << m_BufferSize <<
				"\n channels:   " << parameters.nChannels
			);

			m_Running = true;
		}
		catch (RtAudioError& e) {
			e.printMessage();
			return false;
		}
		return true;
	}

	bool StartStream()
	{
		if (!m_Audio.isStreamOpen() || m_Audio.isStreamRunning())
			return false;

		LOG("START");
		m_Audio.startStream();
		return true;
	}

	bool StopStream()
	{
		if (!m_Audio.isStreamOpen() || !m_Audio.isStreamRunning())
			return false;

		LOG("STOP");
		m_Audio.stopStream();
		return true;
	}

	void Disconnect(int id)
	{
		// Make sure this device is connected to the channel
		if (m_Channels.find(id) != m_Channels.end())
		{
			m_Channels.erase(id);

			// If now 0 connections stop stream
			m_Connections--;
			if (m_Connections == 0)
				StopStream();
		}
	}

	void Connect(int id, Channel& channel) 
	{ 
		// Connect to the channel
		m_Channels.emplace(id, channel);

		// Start stream if this is the first connection
		if (m_Connections == 0)
			StartStream();

		channel.StartRead(m_BufferSize * 2 + m_Samplerate * 0.090);
		m_Connections++;
	}

	std::unordered_map<int, std::reference_wrapper<Channel>>& Channels() { return m_Channels; };

	double avg = 0;

private:
	int m_Samplerate,
		m_BufferSize,
		m_Connections = 0;
	
	bool m_Running = false,
		m_Input = false;


	::Device  m_Device;
	::RtAudio m_Audio { RtAudio::Api::WINDOWS_WASAPI };
	std::unordered_map<int, std::reference_wrapper<Channel>> m_Channels;

	static int InputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		double* _buffer = (double*)inputBuffer;
		AudioDevice& _me = *(AudioDevice*)userData;
		int _bufferSize = nBufferFrames * _me.Device().inputChannels;

		for (auto& _c : _me.Channels())
		{
			_c.second.get().Push(_buffer, _bufferSize);
		}
		

		return 0;
	}

	static int OutputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		double* _buffer = (double*)outputBuffer;
		AudioDevice& _me = *(AudioDevice*)userData;
		int _bufferSize = nBufferFrames * _me.Device().outputChannels;
		for (int i = 0; i < _bufferSize; i++)
		{
			double _sample = 0;
			
			for (auto& _c : _me.Channels())
			{
				_sample += _c.second.get().Pop();
			};

			*_buffer++ = _sample;
		}
		return 0;
	}
};



class AudioIO
{
public:

	AudioIO()
	{}

	void LoadDevices()
	{
		LOG("Loading devices\n");
		// Load available devices
		m_Devices.clear();
		RtAudio::DeviceInfo info; // First all ASIO
		unsigned int devices = m_AsioAudio.getDeviceCount();
		LOG(" ASIO device count: " << devices << "\n");
		for (unsigned int i = 0; i < devices; i++)
		{
			info = m_AsioAudio.getDeviceInfo(i);
			LOG("   Device " << info.name << " is " << (info.probed ? "" : "not ") << "probed\n");
			if (info.probed == true)
				m_Devices.emplace_back(Device{ i, info, m_AsioAudio.getCurrentApi() });
		}
		devices = m_WasapiAudio.getDeviceCount(); // Then all WASAPI
		LOG(" WASAPI device count: " << devices << "\n");
		for (unsigned int i = 0; i < devices; i++)
		{
			info = m_WasapiAudio.getDeviceInfo(i);
			LOG("   Device " << info.name << " is " << (info.probed ? "" : "not ") << "probed\n");
			if (info.probed == true)
				m_Devices.emplace_back(Device{ i, info, m_WasapiAudio.getCurrentApi() });
		}

		// Create AudioDevices
		auto& _d = DeviceInfos();
		m_Inputs.clear();
		m_Inputs.reserve(devices);
		m_Outputs.clear();
		m_Outputs.reserve(devices);
		for (auto& _device : _d)
		{
			if (_device.inputChannels != 0)
				m_Inputs.emplace_back(_device, true, 48000).OpenDevice();

			if (_device.outputChannels != 0)
				m_Outputs.emplace_back(_device, false, 48000).OpenDevice();
		}
	}

	auto DeviceInfos() -> std::vector<Device>& { return m_Devices; }
	auto InputDevices() -> std::vector<AudioDevice>& { return m_Inputs; }
	auto OutputDevices() -> std::vector<AudioDevice>& { return m_Outputs; }

private:
	std::vector<Device> m_Devices;
	std::vector<AudioDevice> m_Inputs, m_Outputs;
	RtAudio m_AsioAudio{ RtAudio::Api::WINDOWS_ASIO };
	RtAudio m_WasapiAudio{ RtAudio::Api::WINDOWS_WASAPI};
};