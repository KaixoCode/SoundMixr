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




private:
    Gui m_Gui;
    Frame& mainWindow;

    AudioIO m_AudioIO;
};