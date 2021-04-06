#pragma once
#include "pch.hpp"

class Device
{
public:
	Device(PaDeviceIndex id, const PaDeviceInfo& a)
		: id(id), info(a)
	{}

	PaDeviceIndex id;
	const PaDeviceInfo& info;
};

class Endpoint
{
public:
	Endpoint(int id, const std::string& name, bool input)
		: id(id), name(name), input(input)
	{}

	
	float sample = 0;

	const std::string name;
	int id;
	bool input;
};

class Asio
{
public:

	Asio() 
		: m_BufferSize(256), m_Samplerate(48000)
	{
		LOG("Initializing Portaudio library");
		PaError err;
		err = Pa_Initialize();
		if (err != paNoError)
		{
			LOG(Pa_GetErrorText(err));
			return;
		}

		LOG("Finding ASIO devices");
		const PaDeviceInfo* info;
		for (PaDeviceIndex i = 0; i < Pa_GetDeviceCount(); i++)
		{
			info = Pa_GetDeviceInfo(i);
			if (std::string(Pa_GetHostApiInfo(info->hostApi)->name) == "ASIO")
				m_Devices.emplace_back(i, *info);
		}
	}

	~Asio()
	{
		LOG("Destructing ASIO device ");
		CloseStream();
	}

	auto Devices() -> std::vector<::Device>& { return m_Devices; }
	void Device(::Device& d) { m_Device = &d; }
	void Device(int d) { m_Device = &Devices()[d]; }
	void NoDevice() { m_Device = nullptr; }
	auto Device() -> ::Device& { return *m_Device; }
	bool StreamRunning() { return Pa_IsStreamActive(stream); }
	bool Opened() { return m_Opened; }
	int  DeviceId() { return m_Device ? m_Device->id : -1; }
	
	/**
	 * Opens a stream with the set device and the given callback and userdata.
	 * @param c callback
	 * @param userdata userdata
	 */
	bool OpenStream(PaStreamCallback c, void* userdata)
	{
		LOG("Attempting to open ASIO stream");
		PaError err;
		PaStreamParameters ip, op;

		// Input device settings
		ip.device = Device().id;
		ip.channelCount = Device().info.maxInputChannels;
		ip.sampleFormat = paFloat32;
		ip.suggestedLatency = Device().info.defaultLowInputLatency;
		ip.hostApiSpecificStreamInfo = NULL;

		// Output device settings
		op.device = Device().id;
		op.channelCount = Device().info.maxOutputChannels;
		op.sampleFormat = paFloat32;
		op.suggestedLatency = Device().info.defaultLowOutputLatency;
		op.hostApiSpecificStreamInfo = NULL;

		// Add all input channels to vector
		for (int i = 0; i < ip.channelCount; i++)
		{
			const char* name;
			PaAsio_GetInputChannelName(Device().id, i, &name);
			std::string n = name;
			n.resize(n.find_last_of(' '));
			auto& a = m_Inputs.emplace_back(i, n, true);
		}

		// Add all output channels to vector
		for (int i = 0; i < op.channelCount; i++)
		{
			const char* name;
			PaAsio_GetOutputChannelName(Device().id, i, &name);
			std::string n = name;
			n.resize(n.find_last_of(' '));
			auto& a = m_Outputs.emplace_back(i, n, false);
		}

		// Try common sample rates
		int tries = 0;
		double _srates[]{ 48000.0, 44100.0 };
		do
		{
			if (tries == 2)
			{
				// If wasn't able to find working samplerate, 
				// close stream and stop trying.
				LOG(Pa_GetErrorText(err));
				CloseStream();
				return false;
			}
			m_Samplerate = _srates[tries];
			LOG("Trying samplerate " << m_Samplerate);
			tries++;
		} while ((err = Pa_OpenStream(&stream, &ip, &op, m_Samplerate, m_BufferSize, paClipOff, c, userdata)) != 0);

		// Logging
		LOG("Opened stream (" << Device().info.name << ")" <<
			"\n type:       " << "ASIO" <<
			"\n samplerate: " << m_Samplerate <<
			"\n buffersize: " << m_BufferSize <<
			"\n inchannels: " << ip.channelCount <<
			"\n outchannels:" << op.channelCount
		);

		LOG("Input channel names: ");
		for (auto& i : m_Inputs)
			LOG(i.name);

		LOG("Output channel names: ");
		for (auto& i : m_Outputs)
			LOG(i.name);

		// It's now opened
		m_Opened = true;
		return true;
	}

	/**
	 * Closes the stream if it is opened.
	 */
	void CloseStream()
	{
		// If stream is not opened, can't close.
		if (!m_Opened)
			return;

		// Close stream
		m_Opened = false;
		LOG("Closing SAR stream...");
		PaError err;
		err = Pa_CloseStream(stream);
		if (err != paNoError)
			LOG(Pa_GetErrorText(err));
		else
			LOG("Closed SAR stream");

		// Clear the vectors of endpoints.
		m_Inputs.clear();
		m_Outputs.clear();
	}
	
	/**
	 * Starts the stream if it's opened and not already running.
	 * @return true when started, false when not open, already running, or error.
	 */
	bool StartStream()
	{
		// Can't start if not opened
		if (!m_Opened)
			return false;

		// Can't start if already running
		if (StreamRunning())
			return false;

		LOG("Starting SAR stream...");
		PaError err = Pa_StartStream(stream);
		if (err != paNoError)
		{
			LOG(Pa_GetErrorText(err));
			return false;
		}
		else
			LOG("Started SAR stream");

		return true;
	}

	/**
	 * Stops a stream if it's running.
	 * @return true when stopped, false if not running or error.
	 */
	bool StopStream()
	{
		if (!StreamRunning())
			return false;

		LOG("Stopping SAR stream...");
		PaError err = Pa_StopStream(stream);
		if (err != paNoError)
		{
			LOG(Pa_GetErrorText(err));
			return false;
		}
		else
			LOG("Stopped SAR stream");

		return true;
	}

	std::vector<Endpoint>& Inputs() { return m_Inputs; }
	std::vector<Endpoint>& Outputs() { return m_Outputs; }

private:
	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	double m_Samplerate;
	int m_BufferSize;
	bool m_Opened = false;

	PaStream* stream = nullptr;
	::Device* m_Device = nullptr;

	std::vector<::Device> m_Devices;
};