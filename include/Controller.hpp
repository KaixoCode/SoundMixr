#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "ui/Soundboard.hpp"
#include "ui/Graphics.hpp"
#include "ui/Frame.hpp"
#include "audio/AudioController.hpp"
#include "EffectLoader.hpp"
#include "midi/Midi.hpp"

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

    auto Audio() -> AudioController& { return *m_Audio; }

private:
    bool m_LoadedSettings = false;

    Gui m_Gui;
    SoundMixrFrame& mainWindow, &settings;
    Soundboard& soundboard;

    std::vector<std::string> m_MidiEnabled;

    std::vector<Button<ToggleButtonGraphics, ButtonType::Toggle>*> m_MidiButtons;
    Panel* m_MidiDevices;

    DropDown<std::string, DropdownButton2>* m_ThemeDropDown;
    DropDown<int, DropdownButton2>* m_AsioDropDown;
    Parameter<SliderGraphics>* m_ScaleSlider;

    AudioController* m_Audio;
};