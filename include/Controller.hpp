#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"

/*
 * TODO:
 *  - Add volume visuals
 *  - Nicer GUI
 *  - Option to make GUI vertical
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

    SarAsio m_SarAsio;
};