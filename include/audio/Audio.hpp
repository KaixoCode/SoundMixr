#pragma once
#include "pch.hpp"
#include "audio/Channel.hpp"

class Device : public RtAudio::DeviceInfo
{
public:
	Device()
		: id(-1)
	{}

	Device(unsigned int id, RtAudio::DeviceInfo& a)
		: id(id), RtAudio::DeviceInfo(a)
	{}

	unsigned int id;
};

class AudioIO
{
public:
	AudioIO(const Device& device, bool in, int samplerate = -1, int bufferSize = 256)
		: m_BufferSize(bufferSize), m_Device(device), m_Input(in)
	{
		if (samplerate == -1)
			m_Samplerate = m_Device.preferredSampleRate;
	}

	int  Samplerate() { return m_Samplerate; };
	bool Samplerate(int s) { m_Samplerate = s; return true; }
	::RtAudio& RtAudio() { return m_Audio; }
	::Device&  Device() { return m_Device; }

	bool OpenDevice() 
	{
		if (m_Audio.isStreamOpen())
			m_Audio.closeStream();

		RtAudio::StreamParameters parameters;
		parameters.deviceId = m_Device.id;
		parameters.nChannels = m_Input ? m_Device.inputChannels : m_Device.outputChannels;
		parameters.firstChannel = 0;

		unsigned int sampleRate = m_Samplerate;
		unsigned int bufferFrames = m_BufferSize;

		try {
			if (m_Input)
				m_Audio.openStream(NULL, &parameters, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &InputCallback, (void*)this);
			else 
				m_Audio.openStream(&parameters, NULL, RTAUDIO_FLOAT64,
					sampleRate, &bufferFrames, &OutputCallback, (void*)this);

			m_Audio.startStream();
			m_Running = true;
		}
		catch (RtAudioError& e) {
			e.printMessage();
			return false;
		}
		return true;
	}



private:
	int m_Samplerate,
		m_BufferSize;
	
	bool m_Running = false,
		m_Input = false;

	::Device  m_Device;

	::RtAudio m_Audio { RtAudio::Api::WINDOWS_WASAPI };

	static int InputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		/*double* _buffer = (double*)inputBuffer;
		AudioIO& _me = *(AudioIO*)userData;
		int _bufferSize = nBufferFrames * _me.Device().inputChannels;
		double a = 0;
		for (int i = 0; i < _bufferSize; i++)
		{
			double _sample = *_buffer++;
			a += _sample;
			for (auto& _c : _me.m_Channels)
			{
				_c.second.get().enqueue(_sample);
			}
		}
		a /= (double)_bufferSize;
		_me.avg = a;*/

		return 0;
	}

	static int OutputCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* userData)
	{
		/*double* _buffer = (double*)outputBuffer;
		AudioIO& _me = *(AudioIO*)userData;
		int _bufferSize = nBufferFrames * _me.Device().outputChannels;
		
		for (int i = 0; i < _bufferSize; i++)
		{
			double _sample = 0;
			
			for (auto& _c : _me.m_Channels)
			{
				double _t = 0;
				_c.second.get().try_dequeue(_t);
				_sample += _t;
			};

			*_buffer++ = _sample;
		}*/

		return 0;
	}
};

