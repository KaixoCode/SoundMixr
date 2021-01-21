#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"

/*
 * Input devices
 *  - Option to add or remove devices
 *  - Select to which output devices
 *
 * Output devices
 *  - Option to add or remove devices
 * 
 * Channel
 *  - Select in and out
 * 
 * 
 * Bugs:
 *  - Crash when closing application
 *  - No audio input when in ASIO api
 * 
 * 
 * Ideas:
 *  - Audio per application (if possible)
 * 
 */

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

class Controller
{
public:

    Controller();
    void Run();


    std::vector<::Device>& Devices() { return m_Devices; }

	void UpdateDeviceList()
	{
		unsigned int devices = m_Audio.getDeviceCount();
		RtAudio::DeviceInfo info;
		for (unsigned int i = 0; i < devices; i++) {
			info = m_Audio.getDeviceInfo(i);
			if (info.probed == true)
				m_Devices.emplace_back(::Device{ i, info });
		}
	}

private:
    Gui m_Gui;
    Frame& mainWindow;
	RtAudio m_Audio { RtAudio::Api::WINDOWS_WASAPI };
	std::vector<::Device> m_Devices;
};