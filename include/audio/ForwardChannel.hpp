#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

class ForwardChannel : public ChannelBase
{
public:
	ForwardChannel()
		: ChannelBase(ChannelBase::Type::Forward | ChannelBase::Type::Output | ChannelBase::Type::Input)
	{
		name.Content("Forward");

		Lines(2);
	}

	RtAudio m_Asio{ RtAudio::Api::WINDOWS_WASAPI };
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	int m_InputDevice = -1;
	int m_OutputDevice = -1;

	void CloseStream() 
	{
		if (!m_Asio.isStreamRunning())
			return;
		
		m_Asio.stopStream();
	}

	void OpenStream()
	{
		if (m_Asio.isStreamRunning())
			return;

		if (m_Asio.isStreamOpen())
			m_Asio.closeStream();

		CrashLog("Attempting to open Asio stream");
		RtAudio::StreamParameters ip, op;

		// Input device settings
		ip.deviceId = m_InputDevice;
		ip.nChannels = m_Asio.getDeviceInfo(m_InputDevice).inputChannels;
		
		// Output device settings
		op.deviceId = m_OutputDevice;
		op.nChannels = m_Asio.getDeviceInfo(m_OutputDevice).outputChannels;

		// Add all input channels to vector
		m_Inputs.clear();
		for (int i = 0; i < ip.nChannels; i++)
		{
			std::string n = m_Asio.getDeviceInfo(m_InputDevice).name;
			auto& a = m_Inputs.emplace_back(i, n, true);
		}

		// Add all output channels to vector
		m_Outputs.clear();
		for (int i = 0; i < op.nChannels; i++)
		{
			std::string n = m_Asio.getDeviceInfo(m_OutputDevice).name;
			auto& a = m_Outputs.emplace_back(i, n, false);
		}

		// Try common sample rates
		unsigned int frames = 256;
		m_Asio.openStream(&op, &ip, RTAUDIO_FLOAT32, 48000, &frames, &AsioCallback, this);
		m_Asio.startStream();

		Lines(ip.nChannels);

		// Logging
		CrashLog("Opened stream (" << m_Asio.getDeviceInfo(m_InputDevice).name << ")" <<
			"\n samplerate: " << 48000 <<
			"\n buffersize: " << 256 <<
			"\n inchannels: " << ip.nChannels <<
			"\n outchannels:" << op.nChannels
		);

		CrashLog("Input channel names: ");
		for (auto& i : m_Inputs)
			CrashLog(i.name);

		CrashLog("Output channel names: ");
		for (auto& i : m_Outputs)
			CrashLog(i.name);

		// It's now opened

		return;
	}

	static inline int AsioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void* data)
	{
		ForwardChannel& _this = *static_cast<ForwardChannel*>(data);

		float* _inBuffer = (float*)inputBuffer;
		float* _outBuffer = (float*)outputBuffer;

		auto& _inputs = _this.m_Inputs;
		auto& _outputs = _this.m_Outputs;

		for (int i = 0; i < nBufferFrames; i++)
		{
			std::lock_guard<std::mutex> _{ _this.m_Lock };
			
			int index = i * _inputs.size();
			for (auto& level : _this.m_Levels)
				level = _inBuffer[index++];

			// Process main channel things
			_this.ChannelBase::Process();

			index = i * _inputs.size();
			for (auto& level : _this.m_Levels)
				_outBuffer[index++] = level;

			// Reset levels
			for (auto& i : _this.m_Levels)
				i = 0;
		}
		return 0;
	}

};