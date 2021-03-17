#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "ui/ChannelPanel.hpp"
#include "ui/ListPanel.hpp"
#include "ui/Soundboard.hpp"
#include "ui/Graphics.hpp"
#include "ui/SettingsPanel.hpp"

 // -------------------------------------------------------------------------- \\
 // ---------------------------- Controller ---------------------------------- \\
 // -------------------------------------------------------------------------- \\
 
class Controller
{
public:
    Controller();
    void Run();
    void LoadRouting();
    void SaveRouting();

private:
    Gui m_Gui;
    Frame& mainWindow, &settings;
    Soundboard& soundboard;

    ListPanel* m_List = nullptr;

    AsioDevice m_AsioDevice;
};