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
		Listener() += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
			{
				m_Menu.Clear();
				m_Menu.ButtonSize({ 180, 20 });
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(name.Content()).Disable();
				m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
				m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { OpenDevices(); }, "Open Device");
				auto& _sub = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>
					("Input Device");
				_sub.MenuBase().ButtonSize({ 250, 20 });
				auto& _sub2 = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>
					("Output Device");
				_sub2.MenuBase().ButtonSize({ 250, 20 });
				int _id1 = ButtonType::List::NewKey();
				int _id2 = ButtonType::List::NewKey();
				m_Asio.ReloadDevices();
				for (auto& _c : m_Asio.Devices())
				{
					std::string name = std::string(Pa_GetHostApiInfo(_c.second.info.hostApi)->name);
					name += ": ";
					name += _c.second.info.name;
					int _devid = _c.second.id;
					if (_c.second.info.maxInputChannels != 0)
						_sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([=]
							{ m_InputDevice = _devid; }, name, _id1);

					if (_c.second.info.maxOutputChannels != 0)
						_sub2.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([=]
							{ m_OutputDevice = _devid; }, name, _id2);
				}
				RightClickMenu::Get().Open(&m_Menu);
			}
		};
	}

	/**
	 * Processing for the endpoint channel happens as follows:
	 *  - Get the levels from the endpoints
	 *  - Apply effectchain, panning, volume, mute, and mono.
	 *  - Forward to all connections.
	 */
	virtual void Process() override
	{
		m_Lock.lock();



		// Process main channel things
		ChannelBase::Process();

		// Reset levels
		for (auto& i : m_Levels)
			i = 0;

		m_Lock.unlock();
	};

private:
	Asio m_Asio{ false };
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	int m_InputDevice = -1;
	int m_OutputDevice = -1;

	void OpenDevices()
	{
		LOG("Attempting to open Asio stream");
		PaError err;
		PaStreamParameters ip, op;

		// Input device settings
		ip.device = m_Asio.Devices()[m_InputDevice].id;
		ip.channelCount = m_Asio.Devices()[m_InputDevice].info.maxInputChannels;
		ip.sampleFormat = paFloat32;
		ip.suggestedLatency = m_Asio.Devices()[m_InputDevice].info.defaultLowInputLatency;
		ip.hostApiSpecificStreamInfo = NULL;

		// Output device settings
		op.device = m_Asio.Devices()[m_OutputDevice].id;
		op.channelCount = m_Asio.Devices()[m_OutputDevice].info.maxOutputChannels;
		op.sampleFormat = paFloat32;
		op.suggestedLatency = m_Asio.Devices()[m_OutputDevice].info.defaultLowOutputLatency;
		op.hostApiSpecificStreamInfo = NULL;

		// Add all input channels to vector
		for (int i = 0; i < ip.channelCount; i++)
		{
			const char* name = "";
			//PaAsio_GetInputChannelName(m_Asio.Devices()[m_InputDevice].id, i, &name);
			std::string n = name;
			//n.resize(n.find_last_of(' '));
			auto& a = m_Inputs.emplace_back(i, n, true);
		}

		// Add all output channels to vector
		for (int i = 0; i < op.channelCount; i++)
		{
			const char* name = "";
			//PaAsio_GetOutputChannelName(m_Asio.Devices()[m_OutputDevice].id, i, &name);
			std::string n = name;
			//n.resize(n.find_last_of(' '));
			auto& a = m_Outputs.emplace_back(i, n, false);
		}

		PaStream* stream = m_Asio.Stream();

		// Try common sample rates
		int tries = 0;
		double _srates[]{ m_Asio.Devices()[m_OutputDevice].info.defaultSampleRate, 48000.0, 44100.0, 96000, 192000 };
		do
		{
			if (tries == sizeof(_srates) / sizeof(double))
			{
				// If wasn't able to find working samplerate, 
				// close stream and stop trying.
				LOG(Pa_GetErrorText(err));
				m_Asio.CloseStream();
				return;
			}
			m_Asio.SampleRate(_srates[tries]);
			LOG("Trying samplerate " << m_Asio.SampleRate());
			tries++;
		} while ((err = Pa_OpenStream(&stream, &ip, &op, m_Asio.SampleRate(), 256, paClipOff, AsioCallback, this)) != 0);

		// Logging
		LOG("Opened stream (" << m_Asio.Devices()[m_InputDevice].info.name << ")" <<
			"\n samplerate: " << m_Asio.SampleRate() <<
			"\n buffersize: " << 256 <<
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

		return;
	}

	static inline int AsioCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		return 0;
	}

};