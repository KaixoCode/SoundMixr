#pragma once
#include "pch.hpp"
#include "audio/Channel.hpp"


class Device
{
public:
	Device(PaDeviceIndex id, const PaDeviceInfo& a)
		: id(id), info(a)
	{}

	PaDeviceIndex id;
	const PaDeviceInfo& info;
};


class StereoOutputChannel
{
public:
	StereoOutputChannel(int l, int r, const std::string& name)
		: left(l), right(r), name(name)
	{}

	std::string name;
	int left,
		right;

	float volume = 1;
};

class StereoInputChannel
{
public:
	StereoInputChannel(int l, int r, const std::string& name)
		: left(l), right(r), name(name)
	{}

	std::string name;
	int left,
		right;

	float volume = 1;

	std::unordered_map<int, StereoOutputChannel*> connected;
};

class SarAsio
{
public:
	SarAsio()
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
		const PaDeviceInfo* info; // First all ASIO
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

	~SarAsio()
	{
		LOG("Destructing SAR ASIO device ");
		CloseStream();
	}

	::Device&  Device() { return *m_Device; }

	bool StreamRunning()
	{
		return Pa_IsStreamActive(stream);
	}

	bool OpenStream() 
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
			LOG(i.name);

		LOG("Output channel names: ");
		for (auto& i : m_Outputs)
			LOG(i.name);

		return true;
	}

	void CloseStream()
	{
		LOG("Closing SAR stream...");
		PaError err;
		err = Pa_CloseStream(stream);		
		if (err != paNoError)
			LOG(Pa_GetErrorText(err));
		else
			LOG("Closed SAR stream");
	}

	bool StartStream()
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

	bool StopStream()
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

	std::vector<StereoInputChannel>& Inputs() { return m_Inputs; }
	std::vector<StereoOutputChannel>& Outputs() { return m_Outputs; }

private:
	int m_Samplerate,
		m_BufferSize;
	
	PaStream* stream = nullptr;
	std::unique_ptr<::Device> m_Device;

	std::vector<StereoInputChannel> m_Inputs;
	std::vector<StereoOutputChannel> m_Outputs;

	static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
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
				float _left = 0;
				float _right = 0;

				for (int k = 0; k < _inChannels; k += 2)
				{
					int _index2 = k / 2;
					auto& _channel = _inputs[_index2];
					if (_channel.connected.find(j) != _channel.connected.end())
					{
						float _volume = _channel.volume;
				
						_left += (_inBuffer[i * _inChannels + k]) * _volume;
						_right += (_inBuffer[i * _inChannels + k + 1]) * _volume;
					}
				}

				float _volume = _outputs[_index].volume;

				*_outBuffer++ = _left * _volume;
				*_outBuffer++ = _right * _volume;
			}
		}
		
		return 0;
	}
};