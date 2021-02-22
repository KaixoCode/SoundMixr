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
	LOG("Destructing SAR ASIO device ");
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

void AsioDevice::CloseStream()
{
	LOG("Closing SAR stream...");
	PaError err;
	err = Pa_CloseStream(stream);
	if (err != paNoError)
		LOG(Pa_GetErrorText(err));
	else
		LOG("Closed SAR stream");
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

	double _r = 0.9;

	for (int i = 0; i < nBufferFrames; i++)
	{
		for (int k = 0; k < _inChannels; k += 2)
		{
			int _index = k / 2;
			auto& _inChannel = _inputs[_index];

			float _left = _inBuffer[i * _inChannels + k];
			float _right = _inBuffer[i * _inChannels + k + 1];

			if (i == 0)
			{
				float _inLeft = _inChannel.peak_left;
				float _inRight = _inChannel.peak_right;

				if (_inChannel.mono)
				{
					_inLeft = 0.5 * (_inChannel.peak_left + _inChannel.peak_right);
					_inRight = 0.5 * (_inChannel.peak_left + _inChannel.peak_right);
				}

				float _panLeft = (50 - std::abs(min(_inChannel.pan, 0.0f))) / 50.0;
				float _panRight = (50 - max(_inChannel.pan, 0.0f)) / 50.0;

				_inChannel.level_left = _inChannel.level_left * _r + (1.0 - _r) * _inLeft * _panLeft * _inChannel.volume;
				_inChannel.level_right = _inChannel.level_right * _r + (1.0 - _r) * _inRight * _panRight * _inChannel.volume;

				_inChannel.peak_left = 0;
				_inChannel.peak_right = 0;
			}
			
			if (_inChannel.muted)
				continue;

			if (_inChannel.peak_left < std::abs(_left)) 
				_inChannel.peak_left = std::abs(_left);
			if (_inChannel.peak_right < std::abs(_right))
				_inChannel.peak_right = std::abs(_right);
		}

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
				if (_inChannel.muted)
					continue;

				if (_inChannel.Connected(&_outChannel))
				{
					float _volume = _inChannel.volume;

					float _panLeft = (50 - std::abs(min(_inChannel.pan, 0.0f))) / 50.0;
					float _panRight = (50 - max(_inChannel.pan, 0.0f)) / 50.0;

					float _inLeft = (_inBuffer[i * _inChannels + k]) * _volume;
					float _inRight = (_inBuffer[i * _inChannels + k + 1]) * _volume;

					if (_inChannel.mono)
					{
						_left += 0.5 * (_inLeft + _inRight) * _panLeft;
						_right += 0.5 * (_inLeft + _inRight) * _panRight;
					}
					else
					{
						_left += _inLeft * _panLeft;
						_right += _inRight * _panRight;
					}
				}
			}

			float _panLeft = (50 - std::abs(min(_outChannel.pan, 0.0f))) / 50.0;
			float _panRight = (50 - max(_outChannel.pan, 0.0f)) / 50.0;

			if (i == 0)
			{
				float _outLeft = _outChannel.peak_left;
				float _outRight = _outChannel.peak_right;

				if (_outChannel.mono)
					_outRight = _outLeft = 0.5 * (_outChannel.peak_left + _outChannel.peak_right);

				_outChannel.level_left = _outChannel.level_left * _r + (1.0 - _r) * _outLeft * _panLeft * _outChannel.volume;
				_outChannel.level_right = _outChannel.level_right * _r + (1.0 - _r) * _outRight * _panRight * _outChannel.volume;

				_outChannel.peak_left = 0;
				_outChannel.peak_right = 0;
			}

			if (_outChannel.muted)
			{
				*_outBuffer++ = 0;
				*_outBuffer++ = 0;
				continue;
			}

			if (_outChannel.peak_left < std::abs(_left))
				_outChannel.peak_left = std::abs(_left);
			if (_outChannel.peak_right < std::abs(_right))
				_outChannel.peak_right = std::abs(_right);

			if (_outChannel.mono)
			{
				float _out = 0.5 * (_left + _right);

				*_outBuffer++ = constrain(_out * _outChannel.volume * _panLeft, -1.0f, 1.0f);
				*_outBuffer++ = constrain(_out * _outChannel.volume * _panRight, -1.0f, 1.0f);
			}
			else
			{
				*_outBuffer++ = constrain(_left * _outChannel.volume * _panLeft, -1.0f, 1.0f);
				*_outBuffer++ = constrain(_right * _outChannel.volume * _panRight, -1.0f, 1.0f);
			}
		}
	}

	return 0;
}

void AsioDevice::SaveRouting()
{
	if (&Device() == nullptr)
		return;

	LOG("Saving Routing");
	std::string data;
	for (auto& _i : Inputs())
	{
		data += "in:";
		data += std::to_string(_i.ID()) + ";";
		data += std::to_string(_i.muted) + ";";
		data += std::to_string(_i.mono) + ";";
		data += std::to_string(_i.pan) + ";";
		data += std::to_string(_i.volume) + ";";
		for (int i = 0; i < MAX_CHANNELS; i++)
			if (_i.Connections()[i] != nullptr)
				data += std::to_string(i) + ",";
		data += "\n";
	}
	for (auto& _i : Outputs())
	{
		data += "out:";
		data += std::to_string(_i.ID()) + ";";
		data += std::to_string(_i.muted) + ";";
		data += std::to_string(_i.mono) + ";";
		data += std::to_string(_i.pan) + ";";
		data += std::to_string(_i.volume) + "\n";
	}

	std::ofstream _out;
	_out.open("./routing" + std::to_string(Device().id));
	_out << data;
	_out.close();
}