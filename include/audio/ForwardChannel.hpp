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

	Stream<Api::Wasapi> m_Audio;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	int m_VirtualDevice = -1;
	int m_PhysicalDevice = -1;

	void CloseStream() 
	{
		m_Audio.Close();
	}

	void OpenStream()
	{
		// This thing doesn't rely on anything outside of the channel. So do this.
		std::thread([this](){
			for (int tries = 0; tries < 3; tries++)
			{
				try {
					if (m_VirtualDevice == -1 || m_PhysicalDevice == -1)
						return;

					m_Audio.Close();

					CrashLog("Attempting to open stream");

					int input = m_VirtualDevice;
					int output = m_PhysicalDevice;
					if (Type() & ChannelBase::Type::Input)
					{
						input = m_PhysicalDevice;
						output = m_VirtualDevice;
					}

					m_Audio.UserData(*this);
					m_Audio.Callback(AsioCallback);
					auto err = m_Audio.Open({
						.input = input,
						.output = output
					});

					if (err != NoError)
						throw nullptr; // retry

					auto& info = m_Audio.Information();

					// Add all input channels to vector
					m_Inputs.clear();
					for (int i = 0; i < info.inputChannels; i++)
					{
						m_Inputs.emplace_back(i, m_Audio.Device(info.input).name, true);
					}

					// Add all output channels to vector
					m_Outputs.clear();
					for (int i = 0; i < info.outputChannels; i++)
					{
						auto& a = m_Outputs.emplace_back(i, m_Audio.Device(info.input).name, false);
					}

					Lines(std::min(info.inputChannels, info.outputChannels));
					m_Audio.Start();

					// Logging
					CrashLog("Opened stream (" << m_Audio.Device(info.input).name << ")" <<
						"\n samplerate: " << info.sampleRate <<
						"\n buffersize: " << info.bufferSize <<
						"\n inchannels: " << info.inputChannels <<
						"\n outchannels:" << info.outputChannels
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

	static inline void AsioCallback(Buffer<float>& input, Buffer<float>& output, CallbackInfo info, ForwardChannel& _this)
	{

		auto& _inputs = _this.m_Inputs;
		auto& _outputs = _this.m_Outputs;

		for (int i = 0; i < info.bufferSize; i++)
		{
			std::lock_guard<std::mutex> _{ _this.m_Lock };
			
			int index = 0;
			for (auto& level : _this.m_Levels)
				level = input[i][index++];

			// Process main channel things
			_this.ChannelBase::Process();

			index = 0;
			for (auto& level : _this.m_Levels)
				output[i][index++] = level;

			// Reset levels
			for (auto& i : _this.m_Levels)
				i = 0;
		}
	}

};