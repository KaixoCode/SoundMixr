#include "audio/Asio.hpp"

int Asio::SAMPLE_RATE = -1;

Asio::Asio()
{
	CrashLog("Finding Asio devices...");
	for (auto& _device : Devices())
	{
		CrashLog("Found ASIO device:");
		CrashLog("Id:          " << _device.id);
		CrashLog("Name:        " << _device.name);
		CrashLog("Inputs:      " << _device.inputChannels);
		CrashLog("Outputs:     " << _device.outputChannels);
		CrashLog("Sample Rate: " << _device.sampleRates[0]);
	}
}

Asio::~Asio()
{
	CrashLog("Destructing Asio device ");
	Close();
}

bool Asio::OpenStream(const StreamParameters& s)
{
	CrashLog("Attempting to open Asio stream");
	Stream::Open(s);

	auto& _info = Information();
	auto& _device = Stream::Device(_info.input);

	// Add all input channels to vector
	CrashLog("Retrieving input channel names:");
	for (int i = 0; i < _device.inputChannels; i++)
	{
		auto& _channel = _device.Channel(i, true);
		m_Inputs.emplace_back(i, _channel.name, true);
	}

	// Add all output channels to vector
	CrashLog("Retrieving output channel names:");
	for (int i = 0; i < _device.outputChannels; i++)
	{
		auto& _channel = _device.Channel(i, false);
		m_Outputs.emplace_back(i, _channel.name, false);
	}

	SAMPLE_RATE = _info.sampleRate;

	// Logging
	CrashLog(
		"Opened stream (" << _device.name << ")" <<
		"\n type:       " << "Asio" <<
		"\n samplerate: " << _info.sampleRate <<
		"\n buffersize: " << _info.bufferSize <<
		"\n inchannels: " << _info.inputChannels <<
		"\n outchannels:" << _info.outputChannels
	);

	CrashLog("Input channel names: ");
	for (auto& i : m_Inputs)
		CrashLog(i.name);

	CrashLog("Output channel names: ");
	for (auto& i : m_Outputs)
		CrashLog(i.name);

	return true;
}

void Asio::CloseStream()
{
	CrashLog("Closing Asio stream...");
	Error err = Close();
	if (err != NoError)
		CrashLog("Failed to close Asio stream");
	else
		CrashLog("Closed Asio stream");

	// Clear the vectors of endpoints.
	m_Inputs.clear();
	m_Outputs.clear();
}

bool Asio::StartStream()
{
	CrashLog("Starting Asio stream...");
	auto err = Start();
	if (err != NoError)
	{
		CrashLog("Failed to start asio stream");
		return false;
	}
	else
		CrashLog("Started Asio stream");

	return true;
}

bool Asio::StopStream()
{
	CrashLog("Stopping Asio stream...");
	Error err = Stop();
	if (err != NoError)
	{
		CrashLog("Failed to stop");
		return false;
	}
	else
		CrashLog("Stopped Asio stream");

	return true;
}