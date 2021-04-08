#include "Controller.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\
 
Controller::Controller()
    : mainWindow(m_Gui.AddWindow<SoundMixrFrame>("SoundMixr", 728, 500, true)),
    settings(m_Gui.AddWindow<SoundMixrFrame>("Settings", 400, 526, true, false, false)),
    soundboard(m_Gui.AddWindow<Soundboard>())
{}

void Controller::Run()
{
    // For the first time we need to load effects separately, since
    // Controller::LoadEffects assumes a device is already running.
    EffectLoader::LoadEffects();

    // Set window icons.
    mainWindow.Icon(IDI_ICON1);
    settings.Icon(IDI_ICON1);

    // Add shell icon.
    Menu<GR::Vertical, MT::Normal> _shellIconMenu;
    _shellIconMenu.ButtonSize({ 150, 20 });
    _shellIconMenu.Emplace<MenuButton>([] {}, "SoundMixr").Disable();
    _shellIconMenu.Emplace<MenuDivider>(150, 1, 0, 4);
    _shellIconMenu.Emplace<MenuButton>([&] { mainWindow.Show(); }, "Open GUI");
    _shellIconMenu.Emplace<MenuButton>([&] { m_Gui.Close(); }, "Exit");
    Frame::AddShellIcon(IDI_ICON1, "SoundMixr", [&](Event& e)
        {
            if (e.button == Event::MouseButton::LEFT && e.mod)
                mainWindow.Show();

            if (e.button == Event::MouseButton::RIGHT)
                RightClickMenu::Get().Open(&_shellIconMenu, true);
        });
    
    // AudioController panel, contains all channels and effect panel. Basically the main component.
    mainWindow.Panel().Layout<Layout::Grid>(1, 1, 8, 8);
    mainWindow.Panel().Background(ThemeT::Get().window_border);
    m_Audio = &mainWindow.Panel().Emplace<::Audio>(Layout::Hint::Center);

    // The frame menu of the main window.
    auto& _file = mainWindow.Menu().Emplace<TitleMenuButton>("File");
    _file.Size({ 34, 32 });
    _file.MenuBase().ButtonSize({ 170, 20 });
    _file.Emplace<MenuButton>([&] { settings.Show(); }, "Settings...", Key::CTRL_COMMA);
    _file.Emplace<MenuToggleButton>([&](bool c) { Graphics::DebugOverlay(c); }, "Debug Overlay", Key::CTRL_D);
    _file.Emplace<MenuButton>([&] { m_Audio->Asio().CloseStream(); m_Gui.Close(); }, "Exit", Key::ALT_F4);

    // Settings window.
    settings.Panel().Layout<Layout::Grid>(1, 1, 8, 8);
    auto& _settingsPanel = settings.Panel().Emplace<Panel>();
    auto& _midiScrollPanel = _settingsPanel.Emplace<SMXRScrollPanel>();
    _midiScrollPanel.Size({ 384, 100 });
    _midiScrollPanel.EnableScrollbars(false, true);
    m_MidiDevices = &_midiScrollPanel.Panel<Panel>();
    m_MidiDevices->Layout<Layout::Divs>();

    auto& _asioControlPanel = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [&] {
            m_Audio->ShowControlPanel();
        }, "Control Panel...");
    _asioControlPanel.Size({ 110, 18 });
    _asioControlPanel.Disable();

    auto& _refreshEffects = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [this] { 
            LoadEffects(); 
        }, "Reload");
    _refreshEffects.Size({ 110, 18 });

    auto& _resetGrouping = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [this] {
            m_Audio->DefaultRouting();
        }, "Reset");
    _resetGrouping.Size({ 110, 18 });

    m_ThemeDropDown = &_settingsPanel.Emplace<DropDown<std::string, DropdownButton2>>();
    m_ThemeDropDown->Size({ 110, 18 });
    m_ThemeDropDown->ButtonSize({ 110, 20 });

    Effects::Parameter _scaleParam{ "Zoom Display", Effects::ParameterType::Slider };
    m_ScaleSlider = &_settingsPanel.Emplace<Parameter<SliderGraphics>>(_scaleParam);
    m_ScaleSlider->Size({ 110, 18 });
    m_ScaleSlider->Unit("%");
    m_ScaleSlider->Range({ 50, 200 });
    m_ScaleSlider->ResetValue(100);
    m_ScaleSlider->ResetValue();
    m_ScaleSlider->Decimals(0);
    m_ScaleSlider->Multiplier(1);
    m_ScaleSlider->Vertical(false);
    m_ScaleSlider->DisplayName(false);

    m_AsioDropDown = &_settingsPanel.Emplace<DropDown<int, DropdownButton2>>();
    m_AsioDropDown->Size({ 220, 18 });
    m_AsioDropDown->ButtonSize({ 220, 20 });
    m_AsioDropDown->AddOption("No Device", 0, [&](int i)
        {
            // If device closed successfully, disable controlpanl button and save settings.
            if (Audio().CloseDevice())
                _asioControlPanel.Disable(), SaveSettings();
        });
    for (auto& _d : Audio().Asio().Devices())
        m_AsioDropDown->AddOption(_d.info.name, _d.id + 1, [&](int i)
            {
                // If device opened successfully, enable the controlpanel button and save settings.
                if (Audio().OpenDevice(i - 1))
                    _asioControlPanel.Enable(), SaveSettings();
            });
    
    auto& _refreshMidiDevices = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>([this] { LoadMidi(); }, "Refresh List");
    _refreshMidiDevices.Size({ 110, 18 });

    auto& _reloadThemes = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>([this] { LoadThemes(); }, "Reload");
    _reloadThemes.Size({ 110, 18 });

    auto& _asiotext = _settingsPanel.Emplace<TextComponent>("Asio Settings", Graphics::Fonts::Gidole, 24.0f);
    _asiotext.AlignLines(Align::CENTER);
    
    auto& _moretext = _settingsPanel.Emplace<TextComponent>("Midi Settings", Graphics::Fonts::Gidole, 24.0f);
    _moretext.AlignLines(Align::CENTER);

    auto& _evenmore = _settingsPanel.Emplace<TextComponent>("General Settings", Graphics::Fonts::Gidole, 24.0f);
    _evenmore.AlignLines(Align::CENTER);

    // Settings panel layout
    _settingsPanel.Layout<Layout::Divs>();
    _settingsPanel.Div() = { 4, Div::Alignment::Vertical };

    // Asio part
    auto& asio = _settingsPanel.Div()[3] = { 2, Div::Alignment::Vertical, 0, true, 120 };
    asio[1] = { _asiotext, Div::Alignment::Center, 36 };
    asio[0] = { 3, Div::Alignment::Vertical };
    asio[0][2].DivSize(4);
    asio[0][1] = { 2, Div::Alignment::Horizontal };
    asio[0][1][0].DivSize(10);
    asio[0][1][1] = { 3, Div::Alignment::Vertical, 0, false, 10 };
    asio[0][1][1][1] = { 2, Div::Alignment::Horizontal, 0, false, 24 };
    asio[0][1][1][1][0] = { _settingsPanel.Emplace<TextComponent>("Asio Device"), Div::Alignment::Left, 150};
    asio[0][1][1][1][1] = { *m_AsioDropDown, Div::Alignment::Left };
    asio[0][1][1][2] = { 2, Div::Alignment::Horizontal, 0, false, 24 };
    asio[0][1][1][2][0] = { _settingsPanel.Emplace<TextComponent>("Control Panel"), Div::Alignment::Left, 150 };
    asio[0][1][1][2][1] = { _asioControlPanel, Div::Alignment::Left };
    asio[0][0].DivSize(20);
    
    // Midi part
    auto& midi = _settingsPanel.Div()[2] = { 2, Div::Alignment::Vertical, 0, true, 190 };
    midi[1] = { _moretext, Div::Alignment::Center, 36 };
    midi[0] = { 3, Div::Alignment::Vertical, 0, false, 158 };
    midi[0][2] = { _midiScrollPanel, Div::Alignment::Center, 108 };
    midi[0][1] = { _refreshMidiDevices, Div::Alignment::Center, 30 };
    midi[0][0].DivSize(20);

    // General part
    auto& gnrl = _settingsPanel.Div()[1] = { 2, Div::Alignment::Vertical, 0, true, 202 };
    gnrl[1] = { _evenmore, Div::Alignment::Center, 36 };
    gnrl[0] = { 3, Div::Alignment::Vertical };
    gnrl[0][2].DivSize(4);
    gnrl[0][1] = { 2, Div::Alignment::Horizontal };
    gnrl[0][1][0].DivSize(10);
    gnrl[0][1][1] = { 5, Div::Alignment::Vertical };
    gnrl[0][1][1][0] = { 2, Div::Alignment::Horizontal, 0, false, 26 };
    gnrl[0][1][1][0][0] = { _settingsPanel.Emplace<TextComponent>("Reload Themes"), Div::Alignment::Left, 150 };
    gnrl[0][1][1][0][1] = { _reloadThemes, Div::Alignment::Left };
    gnrl[0][1][1][1] = { 2, Div::Alignment::Horizontal, 0, false, 26 };
    gnrl[0][1][1][1][0] = { _settingsPanel.Emplace<TextComponent>("Theme"), Div::Alignment::Left, 150 };
    gnrl[0][1][1][1][1] = { *m_ThemeDropDown, Div::Alignment::Left };
    gnrl[0][1][1][2] = { 2, Div::Alignment::Horizontal, 0, false, 26 };
    gnrl[0][1][1][2][0] = { _settingsPanel.Emplace<TextComponent>("Reload Effects"), Div::Alignment::Left, 150 };
    gnrl[0][1][1][2][1] = { _refreshEffects, Div::Alignment::Left };
    gnrl[0][1][1][3] = { 2, Div::Alignment::Horizontal, 0, false, 26 };
    gnrl[0][1][1][3][0] = { _settingsPanel.Emplace<TextComponent>("Reset Channel Grouping"), Div::Alignment::Left, 150 };
    gnrl[0][1][1][3][1] = { _resetGrouping, Div::Alignment::Left };
    gnrl[0][1][1][4] = { 2, Div::Alignment::Horizontal, 0, false, 26 };
    gnrl[0][1][1][4][0] = { _settingsPanel.Emplace<TextComponent>("Zoom Display"), Div::Alignment::Left, 150 };
    gnrl[0][1][1][4][1] = { *m_ScaleSlider, Div::Alignment::Left };
    gnrl[0][0].DivSize(20);

    //
    // Main loop
    //

    LoadMidi();
    LoadThemes();
    LoadSettings();

    double pscale = 0;
    int _saveCounter = 5 * 60 * 60;
    int _midiReloadCounter = 60;
    int _midiDeviceCount = Midi::Get().DeviceCount();
    while (m_Gui.Loop())
    {
        // Coloring
        mainWindow.Color(ThemeT::Get().window_border);
        soundboard.Color(ThemeT::Get().window_border);
        settings.Color(ThemeT::Get().window_border);
        mainWindow.Panel().Background(ThemeT::Get().window_border);
        _settingsPanel.LayoutManager().DividerColor(ThemeT::Get().divider);
        _settingsPanel.Background(ThemeT::Get().window_panel);
        _midiScrollPanel.Background(ThemeT::Get().window_frame);

        Midi::Get().ReadMessages();

        m_Audio->UpdateEffects();

        _midiReloadCounter--;
        if (_midiReloadCounter <= 0)
        {
            _midiReloadCounter = 60;
            if (Midi::Get().DeviceCount() != _midiDeviceCount)
            {
                _midiDeviceCount = Midi::Get().DeviceCount();

                LoadMidi();
            }
        }

        if (m_ScaleSlider->Value() != pscale)
        {
            pscale = m_ScaleSlider->Value();
            mainWindow.Scale(1.0 / (pscale * 0.01));
            RightClickMenu::Get().Scale(1.0 / (pscale * 0.01));
        }
        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 5 * 60 * 60;
            SaveSettings();
            m_Audio->SaveRouting();
        }
    }

    // Final save to make sure it is saved when exited.
    SaveSettings();
    m_Audio->SaveRouting();
}

void Controller::LoadSettings()
{
    m_LoadedSettings = false;
    try
    {
        std::ifstream _if;
        _if.open("./settings/settings");
        if (!_if.is_open())
            return;

        nlohmann::json _json;
        _json << _if;
        _if.close();

        auto& theme = _json.at("theme").get<std::string>();
        auto device = _json.at("device").get<int>() + 1;
        auto zoom = _json.at("zoom").get<double>();

        for (auto& i : _json.at("midi-enabled"))
        {
            m_MidiEnabled.push_back(i.get<std::string>());
            for (auto& dev : Midi::Get().Devices())
                if (dev.name == i.get<std::string>() && dev.id >= 0 && dev.id < m_MidiButtons.size())
                    m_MidiButtons[dev.id]->Active(true);
        }

        m_ScaleSlider->Value(zoom);
        m_AsioDropDown->Select(device);
        m_ThemeDropDown->Select(theme);

        SaveSettings();
    }
    catch (...)
    {
        LOG("Failed to load settings.");
    }
    m_LoadedSettings = true;
}

void Controller::SaveSettings()
{
    if (!m_LoadedSettings)
        return;

    try
    {
        nlohmann::json _json = nlohmann::json::object();
        _json["device"] = m_Audio->Asio().DeviceId();
        _json["zoom"] = m_ScaleSlider->Value();
        _json["theme"] = ThemeT::Get().Name();
        _json["midi-enabled"] = nlohmann::json::array();
        for (auto& m : m_MidiEnabled)
            _json["midi-enabled"] += m;

        std::ofstream _of;
        std::filesystem::path dir("./settings");
        std::filesystem::create_directories(dir);
        _of.open("./settings/settings");
        _of << /*std::setw(4) <<*/ _json;
        _of.close();
    }
    catch (...)
    {
        LOG("Failed to save settings.");
    }
}

void Controller::LoadThemes()
{
    SaveSettings();
    m_LoadedSettings = false;
    ThemeT::ReloadThemes();
    m_ThemeDropDown->Clear();

    for (auto& [key, val] : ThemeT::Themes())
    {
        m_ThemeDropDown->AddOption(key, key, [&](std::string& t)
            {
                ThemeT::SetTheme(t);
                SaveSettings();
            });
    }
    LoadSettings();
}

void Controller::LoadMidi()
{
    // Load the midi ports
    Midi::Get().LoadPorts();

    // Get the vector of all midi devices
    auto& _devices = Midi::Get().Devices();
    int _deviceCount = _devices.size();

    // Some settings for the midiDevices Panel
    m_MidiDevices->Div().Clear();
    m_MidiDevices->Clear();
    m_MidiDevices->Width(384);
    m_MidiDevices->Height(_deviceCount * 24);
    m_MidiDevices->Div().Align(Div::Alignment::Vertical);
    m_MidiDevices->Div().Divs(_deviceCount);
    m_MidiDevices->Div().DivSize(_deviceCount * 24);

    // Clear all MidiButtons
    m_MidiButtons.clear();

    // Go through all the midi devices
    for (int i = 0; i < _deviceCount; i++)
    {
        // Make sure it is closed.
        Midi::Get().ClosePort(i);

        // Add the enable button and text component to the div.
        m_MidiDevices->Div()[i] = { 4, Div::Alignment::Horizontal, 0, false, 24 };
        m_MidiDevices->Div()[i][0].DivSize(8);
        m_MidiDevices->Div()[i][1] = { m_MidiDevices->Emplace<TextComponent>(_devices[i].name), Div::Alignment::Left, 150 };
        auto& _b = m_MidiDevices->Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(
            
            // The callback for the enable button
            [&, i](bool& s)
            {
                if (s)
                {
                    // If enabling, try to open port, if fails, set button to false again.
                    if (!Midi::Get().OpenPort(i)) s = false;

                    // Otherwise add the device to the enabled devices and save settings if it's not already enabled. 
                    else if (std::find(m_MidiEnabled.begin(), m_MidiEnabled.end(), _devices[i].name) == m_MidiEnabled.end())
                        m_MidiEnabled.push_back(_devices[i].name), SaveSettings();
                }
                else
                {
                    // If disabling, remove the device from the enabled devices, close the port and save settings.
                    m_MidiEnabled.erase(std::remove_if(m_MidiEnabled.begin(), m_MidiEnabled.end(),
                        [&, i](auto& a) { return a == _devices[i].name; }));
                    Midi::Get().ClosePort(i);
                    SaveSettings();
                }
            }, "Enable");
        _b.Size({ 50, 18 });
        m_MidiButtons.push_back(&_b);

        // Open the device if it's in the midiEnabled vector.
        auto& a = std::find(m_MidiEnabled.begin(), m_MidiEnabled.end(), _devices[i].name);
        if (a != m_MidiEnabled.end())
            Midi::Get().OpenPort(i);

        // Also make sure that the 'enable' button is active if it is opened.
        if (Midi::Get().Opened().find(i) != Midi::Get().Opened().end())
            _b.Active(true);

        // Add the 'enable' button to the div.
        m_MidiDevices->Div()[i][2] = { _b, Div::Alignment::Right };
        m_MidiDevices->Div()[i][3].DivSize(8);
    }

    // Refresh the layout manager, so it recalculates all positions.
    m_MidiDevices->LayoutManager().Refresh();
}

void Controller::LoadEffects()
{
    // In order to load effects we need to stop the stream, because when reloading effects
    // we will deallocate all effects from the DLLs, so no audio can be routed when reloading.

    // First save routing.
    m_Audio->SaveRouting();

    // Stop stream and clear all channels, so all effects will be removed.
    m_Audio->Asio().StopStream();
    m_Audio->Clear();

    // Load effects.
    EffectLoader::LoadEffects();

    // Now we can load the routing again, which will load all channels and effects again.
    m_Audio->LoadRouting();

    // Finally we can start the stream again, since everything is loaded the as 
    // back when we started.
    m_Audio->Asio().StartStream();
}