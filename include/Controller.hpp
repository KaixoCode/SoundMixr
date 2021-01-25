#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"

/*
 * TODO:
 *  - Add sound visuals
 *  - Option to make GUI vertical
 *  - Individual routing volume
 *  - Option to disable clipping
 *  - Audio limiter to prevent clipping
 *  - 'audio test' mode, notify when input hasn't received audio in a while.
 *  - Fix laggy text rendering
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