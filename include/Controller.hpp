#pragma once
#include "pch.hpp"
#include "audio/Asio.hpp"
#include "ui/Soundboard.hpp"
#include "ui/Graphics.hpp"
#include "ui/Frame.hpp"
#include "audio/Audio.hpp"
#include "EffectLoader.hpp"
#include "Midi.hpp"

// Some namespace thingies
namespace GR = SoundMixrGraphics; namespace BT = ButtonType; namespace MT = MenuType;
using MenuButton = Button<GR::Menu, BT::Normal>;
using MenuToggleButton = Button<GR::Menu, BT::Toggle>;
using TitleMenuButton = Button<GR::TitleMenu, BT::Menu<GR::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
using SubMenuButton = Button<GR::SubMenu, BT::Menu<GR::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

// -------------------------------------------------------------------------- \\
 // ---------------------------- Controller ---------------------------------- \\
 // -------------------------------------------------------------------------- \\
 
class Controller
{
public:
    Controller();
    void Run();
    void LoadSettings();
    void SaveSettings();

    void LoadThemes();
    void LoadMidi();
    void LoadEffects();

    auto Audio() -> Audio& { return *m_Audio; }

private:
    bool m_LoadedSettings = false;

    Gui m_Gui;
    SoundMixrFrame& mainWindow, &settings, &effectWindow;

    std::vector<std::string> 
        m_MidiInEnabled,
        m_MidiOutEnabled;

    std::vector<Button<ToggleButtonGraphics, ButtonType::Toggle>*> 
        m_MidiInButtons,
        m_MidiOutButtons;

    Panel
        *m_MidiInDevices, 
        *m_MidiOutDevices;

    DropDown<std::string, DropdownButton2>* m_ThemeDropDown;
    DropDown<int, DropdownButton2>* m_AsioDropDown;
    Parameter<SliderGraphics>* m_ScaleSlider;
    Parameter<SliderGraphics>* m_HueSlider;

    ::Audio* m_Audio;
};