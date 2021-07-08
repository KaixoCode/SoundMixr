#include "audio/Asio.hpp"

int Asio::SAMPLE_RATE = -1;

Asio::Asio()
	: m_BufferSize(256), m_Samplerate(48000)
{
	CrashLog("Initializing Portaudio library");
	PaError err;
	err = Pa_Initialize();
	if (err != paNoError)
	{
		CrashLog("Error: " << Pa_GetErrorText(err));
		return;
	}

	CrashLog("Finding Asio devices...");
	const PaDeviceInfo* info;
	for (PaDeviceIndex i = 0; i < Pa_GetDeviceCount(); i++)
	{
		info = Pa_GetDeviceInfo(i);
		if (std::string(Pa_GetHostApiInfo(info->hostApi)->name) == "ASIO")
		{
			CrashLog("Found ASIO device:");
			CrashLog("Id:          " << i);
			CrashLog("Name:        " << info->name);
			CrashLog("Inputs:      " << info->maxInputChannels);
			CrashLog("Outputs:     " << info->maxOutputChannels);
			CrashLog("Sample Rate: " << info->defaultSampleRate);
			m_Devices.emplace(i, ::Device{ i, *info });
		}
	}
}

Asio::~Asio()
{
	CrashLog("Destructing Asio device ");
	CloseStream();
}

bool Asio::OpenStream(PaStreamCallback c, void* userdata)
{
	CrashLog("Attempting to open Asio stream");
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

	CrashLog("Input settings:");
	CrashLog("Device:   " << ip.device);
	CrashLog("Channels: " << ip.channelCount);
	CrashLog("Latency:  " << ip.suggestedLatency);

	CrashLog("Output settings:");
	CrashLog("Device:   " << op.device);
	CrashLog("Channels: " << op.channelCount);
	CrashLog("Latency:  " << op.suggestedLatency);

	// Add all input channels to vector
	CrashLog("Retrieving input channel names:");
	for (int i = 0; i < ip.channelCount; i++)
	{
		const char* name;
		PaAsio_GetInputChannelName(Device().id, i, &name);
		std::string n = name;
		CrashLog(n);
		//n.resize(n.find_last_of(' '));
		auto& a = m_Inputs.emplace_back(i, n, true);
	}

	// Add all output channels to vector
	CrashLog("Retrieving output channel names:");
	for (int i = 0; i < op.channelCount; i++)
	{
		const char* name;
		PaAsio_GetOutputChannelName(Device().id, i, &name);
		std::string n = name;
		CrashLog(n);
		//n.resize(n.find_last_of(' '));
		auto& a = m_Outputs.emplace_back(i, n, false);
	}

	// Try common sample rates
	int tries = 0;
	double _srates[]{ Device().info.defaultSampleRate, 48000.0, 44100.0, 96000, 192000 };
	do
	{
		if (tries == sizeof(_srates)/sizeof(double))
		{
			// If wasn't able to find working samplerate, 
			// close stream and stop trying.
			CrashLog(Pa_GetErrorText(err));
			CloseStream();
			return false;
		}
		m_Samplerate = _srates[tries];
		CrashLog("Trying samplerate " << m_Samplerate);
		tries++;
	} while ((err = Pa_OpenStream(&stream, &ip, &op, m_Samplerate, m_BufferSize, paClipOff, c, userdata)) != 0);

	SAMPLE_RATE = m_Samplerate;

	// Logging
	CrashLog(
		   "Opened stream (" << Device().info.name << ")" <<
		"\n type:       " << "Asio" <<
		"\n samplerate: " << m_Samplerate <<
		"\n buffersize: " << m_BufferSize <<
		"\n inchannels: " << ip.channelCount <<
		"\n outchannels:" << op.channelCount
	);

	CrashLog("Input channel names: ");
	for (auto& i : m_Inputs)
		CrashLog(i.name);

	CrashLog("Output channel names: ");
	for (auto& i : m_Outputs)
		CrashLog(i.name);

	// It's now opened
	m_Opened = true;
	return true;
}

void Asio::CloseStream()
{
	// If stream is not opened, can't close.
	if (!m_Opened)
		return;

	// Close stream
	m_Opened = false;
	CrashLog("Closing Asio stream...");
	PaError err;
	err = Pa_CloseStream(stream);
	if (err != paNoError)
		CrashLog(Pa_GetErrorText(err));
	else
		CrashLog("Closed Asio stream");

	// Clear the vectors of endpoints.
	m_Inputs.clear();
	m_Outputs.clear();
}

bool Asio::StartStream()
{
	// Can't start if not opened
	if (!m_Opened)
		return false;

	// Can't start if already running
	if (StreamRunning())
		return false;

	CrashLog("Starting Asio stream...");
	PaError err = Pa_StartStream(stream);
	if (err != paNoError)
	{
		CrashLog(Pa_GetErrorText(err));
		return false;
	}
	else
		CrashLog("Started Asio stream");

	return true;
}

bool Asio::StopStream()
{
	if (!StreamRunning())
		return false;

	CrashLog("Stopping Asio stream...");
	PaError err = Pa_StopStream(stream);
	if (err != paNoError)
	{
		CrashLog(Pa_GetErrorText(err));
		return false;
	}
	else
		CrashLog("Stopped Asio stream");

	return true;
}