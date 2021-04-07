#pragma once
#include "pch.hpp"

/**
 * Device class contains information regarting an Asio device.
 */
class Device
{
public:
	Device(PaDeviceIndex id, const PaDeviceInfo& a)
		: id(id), info(a)
	{}

	PaDeviceIndex id;
	const PaDeviceInfo& info;
};

/**
 * The Endpoint class is used as a single endpoint of a device.
 * <code>sample</code> is used as an input/output.
 */
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

/**
 * Asio class opens/closes Asio devices and their streams.
 */
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

		LOG("Finding Asio devices");
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
		LOG("Destructing Asio device ");
		CloseStream();
	}

	/**
	 * Get all available devices.
	 * @return vector of devices
	 */
	auto Devices() -> std::vector<::Device>& { return m_Devices; }

	/**
	 * Set the device that should be opened.
	 * @param d device
	 */
	void Device(::Device& d) { m_Device = &d; }

	/**
	 * Set the device that should be opened using its id.
	 * @param d id
	 */
	void Device(int d) { m_Device = &Devices()[d]; }
	
	/**
	 * Clear the device, sets it to nullptr.
	 */
	void NoDevice() { m_Device = nullptr; }

	/**
	 * Get the current device.
	 * @return device
	 */
	auto Device() -> ::Device& { return *m_Device; }

	/**
	 * Returns true if the stream is currently running.
	 * @return true if stream running
	 */
	bool StreamRunning() { return Pa_IsStreamActive(stream); }

	/**
	 * Returns true if a stream is opened, also returns true if it's also running.
	 * @return true if stream open
	 */
	bool StreamOpened() { return m_Opened; }

	/**
	 * Get the device id of the device that is currently open. -1 if no device.
	 * @return device id
	 */
	int DeviceId() { return m_Device ? m_Device->id : -1; }
	
	/**
	 * Opens a stream with the set device and the given callback and userdata.
	 * @param c callback
	 * @param userdata userdata
	 */
	bool OpenStream(PaStreamCallback c, void* userdata)
	{
		LOG("Attempting to open Asio stream");
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
			"\n type:       " << "Asio" <<
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
		LOG("Closing Asio stream...");
		PaError err;
		err = Pa_CloseStream(stream);
		if (err != paNoError)
			LOG(Pa_GetErrorText(err));
		else
			LOG("Closed Asio stream");

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

		LOG("Starting Asio stream...");
		PaError err = Pa_StartStream(stream);
		if (err != paNoError)
		{
			LOG(Pa_GetErrorText(err));
			return false;
		}
		else
			LOG("Started Asio stream");

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

		LOG("Stopping Asio stream...");
		PaError err = Pa_StopStream(stream);
		if (err != paNoError)
		{
			LOG(Pa_GetErrorText(err));
			return false;
		}
		else
			LOG("Stopped Asio stream");

		return true;
	}

	/**
	 * Get all the input Endpoints.
	 * @return inputs
	 */
	std::vector<Endpoint>& Inputs() { return m_Inputs; }

	/**
	 * Get all the output Endpoints.
	 * @return outputs
	 */
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