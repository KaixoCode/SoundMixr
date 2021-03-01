#include "audio/Audio.hpp"

// -------------------------------------------------------------------------- \\
// -------------------------- SAR ASIO Device ------------------------------- \\
// -------------------------------------------------------------------------- \\

AsioDevice::AsioDevice()
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
		if (info->hostApi == 0)
			m_Devices.emplace_back(i, *info);
	}
}

AsioDevice::~AsioDevice()
{
	LOG("Destructing ASIO device ");
	CloseStream();
}

bool AsioDevice::OpenStream()
{
	LOG("Attempting to open ASIO stream");
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
	unsigned int bufferFrames = m_BufferSize;

	for (int i = 0; i < ip.channelCount; i++)
	{
		const char* name;
		PaAsio_GetInputChannelName(Device().id, i, &name);
		std::string n = name;
		n.resize(n.find_last_of(' '));
		auto& a = m_Inputs.emplace_back(i, n);
	}

	for (int i = 0; i < op.channelCount; i++)
	{
		const char* name;
		PaAsio_GetOutputChannelName(Device().id, i, &name);
		std::string n = name;
		n.resize(n.find_last_of(' '));
		auto& a = m_Outputs.emplace_back(i, n);
	}

	int tries = 0;
	double _srates[]{ 48000.0, 44100.0 };
	do
	{
		if (tries == 2)
		{
			LOG(Pa_GetErrorText(err));
			return false;
		}
		m_Samplerate = _srates[tries];
		LOG("Trying samplerate " << m_Samplerate);
		tries++;
	} while ((err = Pa_OpenStream(&stream, &ip, &op, m_Samplerate, bufferFrames, paClipOff, SarCallback, (void*)this)) != 0);

	LOG("Opened stream (" << Device().info.name << ")" <<
		"\n type:       " << "ASIO" <<
		"\n samplerate: " << m_Samplerate <<
		"\n buffersize: " << m_BufferSize <<
		"\n inchannels: " << ip.channelCount <<
		"\n outchannels:" << op.channelCount
	);

	LOG("Input channel names: ");
	for (auto& i : m_Inputs)
		LOG(i.Name());

	LOG("Output channel names: ");
	for (auto& i : m_Outputs)
		LOG(i.Name());

	return true;
}

void AsioDevice::CloseStream()
{
	LOG("Closing SAR stream...");
	PaError err;
	err = Pa_CloseStream(stream);
	if (err != paNoError)
		LOG(Pa_GetErrorText(err));
	else
		LOG("Closed SAR stream");
	
	m_Inputs.clear();
	m_Outputs.clear();
}

bool AsioDevice::StartStream()
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

bool AsioDevice::StopStream()
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

int AsioDevice::SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	AsioDevice& _this = *(AsioDevice*)userData;

	float* _inBuffer = (float*)inputBuffer;
	float* _outBuffer = (float*)outputBuffer;

	int _inChannels = _this.Device().info.maxInputChannels;
	int _outChannels = _this.Device().info.maxOutputChannels;

	auto& _inputs = _this.Inputs();
	auto& _outputs = _this.Outputs();

	double _r = 0.8;

	for (int i = 0; i < nBufferFrames; i++)
	{
		for (int k = 0; k < _inChannels; k++)
		{
			auto& _inChannel = _inputs[k];
			_inChannel.Level(_inBuffer[i * _inChannels + k]);
			//_inChannel.Level((k + 1.0) / (_inChannels + 1.0));

			if (i == 0)
			{
				_inChannel.Peak(_inChannel.Peak() * _r + (1 - _r) * _inChannel.TPeak());
				_inChannel.TPeak(0);
			}

			float _level = _inChannel.Level();
			if (_level > _inChannel.TPeak())
				_inChannel.TPeak(_level);
			if (-_level > _inChannel.TPeak())
				_inChannel.TPeak(-_level);
		}

		for (int j = 0; j < _outChannels; j++)
		{
			auto& _outChannel = _outputs[j];
			_outChannel.Level(0);
			
			if (i == 0)
			{
				_outChannel.Peak(_outChannel.Peak() * _r + (1 - _r) * _outChannel.TPeak());
				_outChannel.TPeak(0);
			}

			_outChannel.CalcLevel();
		}

		for (int j = 0; j < _outChannels; j++)
		{
			auto& _outChannel = _outputs[j];
			float _level = _outChannel.Level();
			if (_level > _outChannel.TPeak())
				_outChannel.TPeak(_level);
			if (-_level > _outChannel.TPeak())
				_outChannel.TPeak(-_level);
		
			*_outBuffer++ = constrain(_level, -1.0f, 1.0f);
		}
	}

	return 0;
}
