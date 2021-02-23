#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/ChannelPanel.hpp"
#include "ui/ListPanel.hpp"
#include "ui/Soundboard.hpp"
/*
 * TODO:
 *  - Add sound visuals
 *  - Option to make GUI vertical
 *  - Individual routing volume
 *  - Option to disable clipping
 *  - Audio limiter to prevent clipping
 *  - 'audio test' mode, notify when input hasn't received audio in a while.
 *  x Fix laggy text rendering
 */

 // -------------------------------------------------------------------------- \\
 // ---------------------------- Controller ---------------------------------- \\
 // -------------------------------------------------------------------------- \\
 
class Controller
{
public:
    Controller();
    void Run();
    void LoadRouting();

private:
    Gui m_Gui;
    Frame& mainWindow;
    Soundboard& soundboard;

    ListPanel* m_List = nullptr;

    AsioDevice m_AsioDevice;
};