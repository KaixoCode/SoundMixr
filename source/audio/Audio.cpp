#include "audio/Audio.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- SAR ASIO Device ------------------------------- \\
// -------------------------------------------------------------------------- \\

SarAsio::SarAsio()
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

	LOG("Finding SAR ASIO device");
	const PaDeviceInfo* info;
	for (PaDeviceIndex i = 0; i < Pa_GetDeviceCount(); i++)
	{
		info = Pa_GetDeviceInfo(i);
		if (strcmp(info->name, "Synchronous Audio Router") == 0)
		{
			LOG("Found SAR");
			m_Device = std::make_unique<::Device>(i, *info);
			if (OpenStream())
				StartStream();
			else
				LOG("Couldn't open stream");
		}
	}
}

SarAsio::~SarAsio()
{
	LOG("Destructing SAR ASIO device ");
	CloseStream();
}

bool SarAsio::OpenStream()
{
	LOG("Attempting to open SAR stream");
	PaError err;
	PaStreamParameters ip, op;
	ip.device = Device().id;
	ip.channelCount = Device().info.maxInputChannels;
	ip.sampleFormat = paFloat32;
	ip.suggestedLatency = Device().info.defaultLowInputLatency;
	ip.hostApiSpecificStreamInfo = NULL;
	op.device = Device().id;
	op.channelCount = Device().info.maxOutputChannels;
	op.sampleFormat = paFloat32;
	op.suggestedLatency = Device().info.defaultLowOutputLatency;
	op.hostApiSpecificStreamInfo = NULL;

	double sampleRate = m_Samplerate;
	unsigned int bufferFrames = m_BufferSize;

	err = Pa_OpenStream(&stream, &ip, &op, sampleRate, bufferFrames, paClipOff, SarCallback, (void*)this);

	if (err != paNoError)
	{
		LOG(Pa_GetErrorText(err));
		return false;
	}

	LOG("Opened stream (" << Device().info.name << ")" <<
		"\n type:       " << "SAR" <<
		"\n samplerate: " << m_Samplerate <<
		"\n buffersize: " << m_BufferSize <<
		"\n inchannels: " << ip.channelCount <<
		"\n outchannels:" << op.channelCount
	);

	for (int i = 0; i < ip.channelCount; i += 2)
	{
		const char* name;
		PaAsio_GetInputChannelName(Device().id, i, &name);
		std::string n = name;
		n.resize(n.find_last_of(' '));
		m_Inputs.emplace_back(i, i + 1, n);
	}

	for (int i = 0; i < op.channelCount; i += 2)
	{
		const char* name;
		PaAsio_GetOutputChannelName(Device().id, i, &name);
		std::string n = name;
		n.resize(n.find_last_of(' '));
		m_Outputs.emplace_back(i, i + 1, n);
	}

	LOG("Input channel names: ");
	for (auto& i : m_Inputs)
		LOG(i.Name());

	LOG("Output channel names: ");
	for (auto& i : m_Outputs)
		LOG(i.Name());

	return true;
}

void SarAsio::CloseStream()
{
	LOG("Closing SAR stream...");
	PaError err;
	err = Pa_CloseStream(stream);
	if (err != paNoError)
		LOG(Pa_GetErrorText(err));
	else
		LOG("Closed SAR stream");
}

bool SarAsio::StartStream()
{
	if (StreamRunning())
		return false;

	LOG("Starting SAR stream...");
	PaError err = Pa_StartStream(stream);
	if (err != paNoError)
		LOG(Pa_GetErrorText(err));
	else
		LOG("Started SAR stream");

	return true;
}

bool SarAsio::StopStream()
{
	if (!StreamRunning())
		return false;

	LOG("Stopping SAR stream...");
	PaError err = Pa_StopStream(stream);
	if (err != paNoError)
		LOG(Pa_GetErrorText(err));
	else
		LOG("Stopped SAR stream");

	return true;
}

int SarAsio::SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	SarAsio& _this = *(SarAsio*)userData;

	float* _inBuffer = (float*)inputBuffer;
	float* _outBuffer = (float*)outputBuffer;

	int _inChannels = _this.Device().info.maxInputChannels;
	int _outChannels = _this.Device().info.maxOutputChannels;

	auto& _inputs = _this.Inputs();
	auto& _outputs = _this.Outputs();

	for (int i = 0; i < nBufferFrames; i++)
	{
		for (int j = 0; j < _outChannels; j += 2)
		{
			int _index = j / 2;
			auto& _outChannel = _outputs[_index];
			float _left = 0;
			float _right = 0;

			for (int k = 0; k < _inChannels; k += 2)
			{
				int _index2 = k / 2;
				auto& _inChannel = _inputs[_index2];
				if (_inChannel.Connected(&_outChannel))
				{
					float _volume = _inChannel.Volume();

					_left += (_inBuffer[i * _inChannels + k]) * _volume;
					_right += (_inBuffer[i * _inChannels + k + 1]) * _volume;
				}
			}

			float _volume = _outChannel.Volume();

			*_outBuffer++ = constrain(_left * _volume, -1.0f, 1.0f);
			*_outBuffer++ = constrain(_right * _volume, -1.0f, 1.0f);
		}
	}

	return 0;
}
