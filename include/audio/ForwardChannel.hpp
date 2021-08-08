#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

class ForwardChannel : public ChannelBase
{
public:
	ForwardChannel(int t)
		: ChannelBase(ChannelBase::Type::Forward | t)
	{
		name.Content("Forward");

		Lines(2);
	}

	RtAudio m_Asio{ RtAudio::Api::WINDOWS_WASAPI };
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	int m_VirtualDevice = -1;
	int m_PhysicalDevice = -1;

	void CloseStream() 
	{
		try
		{
			if (!m_Asio.isStreamRunning())
				return;
		
			m_Asio.stopStream();
		}
		catch (...)
		{
			CrashLog("Failed to close stream of forwardchannel with ids " << m_VirtualDevice << ", " << m_PhysicalDevice << " and type " << m_Type);
		}
	}

	void OpenStream()
	{
		// This thing doesn't rely on anything outside of the channel. So do this.
		std::thread([this](){
			for (int tries = 0; tries < 10; tries++)
			{
				try {
					if (m_VirtualDevice == -1 || m_PhysicalDevice == -1)
						return;

					if (m_Asio.isStreamRunning())
						m_Asio.stopStream();

					if (m_Asio.isStreamOpen())
						m_Asio.closeStream();

					CrashLog("Attempting to open stream");
					RtAudio::StreamParameters ip, op;

					int input = m_VirtualDevice;
					int output = m_PhysicalDevice;
					if (Type() & ChannelBase::Type::Input)
					{
						input = m_PhysicalDevice;
						output = m_VirtualDevice;
					}

					// Input device settings
					ip.deviceId = input;
					ip.nChannels = m_Asio.getDeviceInfo(input).inputChannels;
		
					// Output device settings
					op.deviceId = output;
					op.nChannels = m_Asio.getDeviceInfo(output).outputChannels;

					// Add all input channels to vector
					m_Inputs.clear();
					for (int i = 0; i < ip.nChannels; i++)
					{
						std::string n = m_Asio.getDeviceInfo(input).name;
						auto& a = m_Inputs.emplace_back(i, n, true);
					}

					// Add all output channels to vector
					m_Outputs.clear();
					for (int i = 0; i < op.nChannels; i++)
					{
						std::string n = m_Asio.getDeviceInfo(output).name;
						auto& a = m_Outputs.emplace_back(i, n, false);
					}

					// Try common sample rates
					unsigned int frames = 512;
					m_Asio.openStream(&op, &ip, RTAUDIO_FLOAT32, 48000, &frames, &AsioCallback, this);
					m_Asio.startStream();

					Lines(ip.nChannels);

					// Logging
					CrashLog("Opened stream (" << m_Asio.getDeviceInfo(input).name << ")" <<
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

					return;
				}
				catch (...) 
				{
					CrashLog("Failed to initialize stream of forwardchannel with ids " << m_VirtualDevice << ", " << m_PhysicalDevice << " and type " << m_Type << ", retrying in 2 seconds.");
					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
					continue;
				}
				CrashLog("Failed to initialize stream of forwardchannel with ids " << m_VirtualDevice << ", " << m_PhysicalDevice << " and type " << m_Type << ", no more retrying.");

			}
			// It's now opened
			}).detach();
		return;
	}

	virtual operator nlohmann::json() override 
	{
		nlohmann::json _json = ChannelBase::operator nlohmann::json();

		_json["physical"] = m_PhysicalDevice;

		return _json;
	};

	virtual void operator=(const nlohmann::json& json) override 
	{
		ChannelBase::operator =(json);
		m_PhysicalDevice = json.at("physical").get<int>();
	};

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