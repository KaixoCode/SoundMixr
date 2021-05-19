#include "audio/Asio.hpp"

int Asio::SAMPLE_RATE = -1;

Asio::Asio()
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
			m_Devices.emplace(i, ::Device{ i, *info });
	}
}

Asio::~Asio()
{
	LOG("Destructing Asio device ");
	CloseStream();
}

bool Asio::OpenStream(PaStreamCallback c, void* userdata)
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
	double _srates[]{ Device().info.defaultSampleRate, 48000.0, 44100.0, 96000, 192000 };
	do
	{
		if (tries == sizeof(_srates)/sizeof(double))
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

	SAMPLE_RATE = m_Samplerate;

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

void Asio::CloseStream()
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

bool Asio::StartStream()
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

bool Asio::StopStream()
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