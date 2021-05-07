#include "Controller.hpp"
#include <audio/SoundboardChannel.hpp>

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\
 
Controller::Controller()
    : mainWindow(m_Gui.AddWindow<SoundMixrFrame>(WindowData{ "SoundMixr", { 728, 500 }, false, false, true, true, true, true, nullptr })),
    settings(m_Gui.AddWindow<SoundMixrFrame>(WindowData{ "Settings", { 400, 526 }, false, false, true, false, true, true, &mainWindow })),
    effectWindow(m_Gui.AddWindow<SoundMixrFrame>(WindowData{ "Effect Chain", { 348, 316 }, false, false, true, false, true, true, &mainWindow })),
    soundboard(m_Gui.AddWindow<Soundboard>())
{}

void Controller::Run()
{
    // For the first time we need to load effects separately, since
    // Controller::LoadEffects assumes a device is already running.
    EffectLoader::LoadEffects();
    ThemeT::ReloadThemes();

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
    effectWindow.Panel().Layout<Layout::Grid>(1, 1, 8, 8);
    effectWindow.Panel().Background(ThemeT::Get().window_border);
    effectWindow.MaxSize({ 348, -1 });
    effectWindow.MinSize({ 348, -1 });
    auto& _effectWindowPanel = effectWindow.Panel().Emplace<EffectPanel>();
    _effectWindowPanel.m_ShowSidebar = false;
    mainWindow.Panel().Layout<Layout::Grid>(1, 1, 8, 8);
    mainWindow.Panel().Background(ThemeT::Get().window_border);
    m_Audio = &mainWindow.Panel().Emplace<::Audio>(Layout::Hint::Center);

    // The frame menu of the main window.
    auto& _file = mainWindow.Menu().Emplace<TitleMenuButton>("File");
    _file.Size({ 34, 32 });
    _file.MenuBase().ButtonSize({ 220, 20 });
    _file.Emplace<MenuButton>([&] { settings.Show(); }, "Settings...", Key::CTRL_COMMA);
db_ _file.Emplace<MenuToggleButton>([&](bool c) { Graphics::DebugOverlay(c); }, "Debug Overlay", Key::CTRL_D);
    _file.Emplace<MenuButton>([&] { soundboard.Show(); }, "Soundboard...", Key::CTRL_SHIFT_S);
    _file.Emplace<MenuButton>([&] { effectWindow.Show(); }, "Effect Window...", Key::CTRL_E);
    _file.Emplace<MenuToggleButton>([&](bool c) {
        SetWindowPos(mainWindow.GetWin32Handle(), c ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
        }, "Always On Top", Key::CTRL_SPACE);
    _file.Emplace<MenuButton>([&] { m_Audio->Asio().CloseStream(); m_Gui.Close(); }, "Exit", Key::ALT_F4);

    // Settings window.
    settings.Panel().Layout<Layout::Grid>(1, 1, 8, 8);
    auto& _settingsScrollPanel = settings.Panel().Emplace<SMXRScrollPanel>();
    _settingsScrollPanel.EnableScrollbars(false, false);
    auto& _settingsPanel = _settingsScrollPanel.Panel<Panel>();
    _settingsPanel.MinHeight(505);
    _settingsPanel.Height(505);
    auto& _midiInScrollPanel = _settingsPanel.Emplace<SMXRScrollPanel>();
    _midiInScrollPanel.Size({ 384, 100 });
    _midiInScrollPanel.EnableScrollbars(false, true);
    m_MidiInDevices = &_midiInScrollPanel.Panel<Panel>();
    m_MidiInDevices->Layout<Layout::Divs>();

    auto& _midiOutScrollPanel = _settingsPanel.Emplace<SMXRScrollPanel>();
    _midiOutScrollPanel.Size({ 384, 100 });
    _midiOutScrollPanel.EnableScrollbars(false, true);
    m_MidiOutDevices = &_midiOutScrollPanel.Panel<Panel>();
    m_MidiOutDevices->Layout<Layout::Divs>();

    auto& _asioControlPanel = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [&] {
            m_Audio->ShowControlPanel();
        }, "Control Panel...");
    _asioControlPanel.Size({ 98, 18 });
    _asioControlPanel.Disable();

    auto& _refreshEffects = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [this] { 
            LoadEffects(); 
        }, "Reload");
    _refreshEffects.Size({ 98, 18 });

    auto& _resetGrouping = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(
        [this] {
            m_Audio->DefaultRouting();
        }, "Reset");
    _resetGrouping.Size({ 98, 18 });

    m_ThemeDropDown = &_settingsPanel.Emplace<DropDown<std::string, DropdownButton2>>();
    m_ThemeDropDown->Size({ 98, 18 });
    m_ThemeDropDown->ButtonSize({ 98, 20 });

    Effects::Parameter _scaleParam{ "Zoom Display", Effects::ParameterType::Slider };
    m_ScaleSlider = &_settingsPanel.Emplace<Parameter<SliderGraphics>>(_scaleParam);
    m_ScaleSlider->Size({ 98, 18 });
    m_ScaleSlider->Unit("%");
    m_ScaleSlider->Range({ 50, 200 });
    m_ScaleSlider->ResetValue(100);
    m_ScaleSlider->ResetValue();
    m_ScaleSlider->Decimals(0);
    m_ScaleSlider->Multiplier(1);
    m_ScaleSlider->Vertical(false);
    m_ScaleSlider->DisplayName(false);

    m_AsioDropDown = &_settingsPanel.Emplace<DropDown<int, DropdownButton2>>();
    m_AsioDropDown->Size({ 198, 18 });
    m_AsioDropDown->ButtonSize({ 198, 20 });
    m_AsioDropDown->AddOption("No Device", 0, [&](int i)
        {
            // If device closed successfully, disable controlpanl button and save settings.
            if (Audio().CloseDevice())
                _asioControlPanel.Disable(), SaveSettings();
        });
    for (auto& _d : Audio().Asio().Devices())
        m_AsioDropDown->AddOption(_d.second.info.name, _d.second.id + 1, [&](int i)
            {
                // If device opened successfully, enable the controlpanel button and save settings.
                if (Audio().OpenDevice(i - 1))
                    _asioControlPanel.Enable(), SaveSettings();
            });
    
    auto& _refreshMidiDevices = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>([this] { LoadMidi(); }, "Refresh List");
    _refreshMidiDevices.Size({ 98, 18 });

    auto& _reloadThemes = _settingsPanel.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>([this] { LoadThemes(); }, "Reload");
    _reloadThemes.Size({ 98, 18 });

    auto& _asiotext = _settingsPanel.Emplace<SMXRTextComponent>("Asio Settings", 24.0f);
    _asiotext.AlignLines(Align::CENTER);
    
    auto& _moretext = _settingsPanel.Emplace<SMXRTextComponent>("Midi Settings", 24.0f);
    _moretext.AlignLines(Align::CENTER);

    auto& _evenmore = _settingsPanel.Emplace<SMXRTextComponent>("General Settings", 24.0f);
    _evenmore.AlignLines(Align::CENTER);

    // Settings panel layout
    _settingsPanel.Layout<Layout::Divs>();
    _settingsPanel.Div() = { 4, Div::Alignment::Vertical };

    // Asio part
    auto& asio = _settingsPanel.Div()[3] = { 2, Div::Alignment::Vertical, 0, true, 120 };
    asio[1] = { _asiotext, Div::Alignment::Center, 36 };
    asio[0] = { 3, Div::Alignment::Vertical };
    asio[0][2].DivSize(4);
    asio[0][1] = { 4, Div::Alignment::Horizontal };
    asio[0][1][0].DivSize(10);
    asio[0][1][1] = { 3, Div::Alignment::Vertical };
    asio[0][1][2] = { 3, Div::Alignment::Vertical };
    asio[0][1][1][1] = { _settingsPanel.Emplace<SMXRTextComponent>("Asio Device"), Div::Alignment::Left, 24 };
    asio[0][1][2][1] = { *m_AsioDropDown, Div::Alignment::Left, 24 };
    asio[0][1][1][2] = { _settingsPanel.Emplace<SMXRTextComponent>("Control Panel"), Div::Alignment::Left, 24 };
    asio[0][1][2][2] = { _asioControlPanel, Div::Alignment::Left, 24 };
    asio[0][1][3].DivSize(40);
    asio[0][0].DivSize(20);
    
    // Midi part
    auto& midi = _settingsPanel.Div()[2] = { 2, Div::Alignment::Vertical, 0, true, Div::AUTO};
    midi[1] = { _moretext, Div::Alignment::Center, 36 };
    midi[0] = { 5, Div::Alignment::Vertical };
    midi[0][4].DivSize(8);
    midi[0][3] = { 2, Div::Alignment::Horizontal, 8, false, 16 };
    midi[0][3][0] = { _settingsPanel.Emplace<SMXRTextComponent>("Inputs"), Div::Alignment::Center, Div::AUTO };
    midi[0][3][1] = { _settingsPanel.Emplace<SMXRTextComponent>("Outputs"), Div::Alignment::Center, Div::AUTO };
    midi[0][2] = { 2, Div::Alignment::Horizontal, 8 };
    midi[0][2][0] = { _midiInScrollPanel, Div::Alignment::Center, Div::AUTO, true };
    midi[0][2][1] = { _midiOutScrollPanel, Div::Alignment::Center, Div::AUTO, true };
    midi[0][1] = { _refreshMidiDevices, Div::Alignment::Center, 26, false };
    midi[0][0].DivSize(20);

    // General part
    auto& gnrl = _settingsPanel.Div()[1] = { 2, Div::Alignment::Vertical, 0, true, 174 };
    gnrl[1] = { _evenmore, Div::Alignment::Center, 36 };
    gnrl[0] = { 3, Div::Alignment::Vertical };
    gnrl[0][2].DivSize(4);
    gnrl[0][1] = { 4, Div::Alignment::Horizontal, 0, false, 130 };
    gnrl[0][1][0].DivSize(10);
    gnrl[0][1][1] = { 5, Div::Alignment::Vertical };
    gnrl[0][1][2] = { 5, Div::Alignment::Vertical };
    gnrl[0][1][1][0] = { _settingsPanel.Emplace<SMXRTextComponent>("Reload Themes"), Div::Alignment::Left, 26 };
    gnrl[0][1][2][0] = { _reloadThemes, Div::Alignment::Left, 26};
    gnrl[0][1][1][1] = { _settingsPanel.Emplace<SMXRTextComponent>("Theme"), Div::Alignment::Left, 26 };
    gnrl[0][1][2][1] = { *m_ThemeDropDown, Div::Alignment::Left, 26 };
    gnrl[0][1][1][2] = { _settingsPanel.Emplace<SMXRTextComponent>("Reload Effects"), Div::Alignment::Left, 26 };
    gnrl[0][1][2][2] = { _refreshEffects, Div::Alignment::Left, 26};
    gnrl[0][1][1][3] = { _settingsPanel.Emplace<SMXRTextComponent>("Reset Channel Grouping"), Div::Alignment::Left, 26 };
    gnrl[0][1][2][3] = { _resetGrouping, Div::Alignment::Left, 26 };
    gnrl[0][1][1][4] = { _settingsPanel.Emplace<SMXRTextComponent>("Zoom Display"), Div::Alignment::Left, 26 };
    gnrl[0][1][2][4] = { *m_ScaleSlider, Div::Alignment::Left, 26 };
    gnrl[0][1][3].DivSize(40);
    gnrl[0][0].DivSize(4);

    _settingsPanel.Div()[0].DivSize(20);

    m_Audio->EmplaceChannel<SoundboardChannel>(soundboard);

    //
    // Main loop
    //

    LoadMidi();
    LoadThemes();
    LoadSettings();
    soundboard.Load();

    double pscale = 0;
    int _saveCounter = 5 * 60 * 60;
    int _midiReloadCounter = 60;
    int _midiInDeviceCount = Midi::Get().InputDeviceCount();
    int _midiOutDeviceCount = Midi::Get().OutputDeviceCount();
    while (m_Gui.Loop())
    {
        // Coloring
        mainWindow.Color(ThemeT::Get().window_border);
        soundboard.Color(ThemeT::Get().window_border);
        settings.Color(ThemeT::Get().window_border);
        mainWindow.Panel().Background(ThemeT::Get().window_border);
        effectWindow.Panel().Background(ThemeT::Get().window_border);
        effectWindow.Color(ThemeT::Get().window_border);
        _settingsPanel.LayoutManager().DividerColor(ThemeT::Get().divider);
        _settingsPanel.Background(ThemeT::Get().window_panel);
        _midiInScrollPanel.Background(ThemeT::Get().window_frame);
        _midiOutScrollPanel.Background(ThemeT::Get().window_frame);
        _settingsScrollPanel.Background(ThemeT::Get().window_panel);

        Midi::Get().ReadMessages();

        m_Audio->UpdateEffects();

        _midiReloadCounter--;
        if (_midiReloadCounter <= 0)
        {
            _midiReloadCounter = 60;
            if (Midi::Get().InputDeviceCount() != _midiInDeviceCount ||
                Midi::Get().OutputDeviceCount() != _midiOutDeviceCount)
            {
                _midiInDeviceCount = Midi::Get().InputDeviceCount();
                _midiOutDeviceCount = Midi::Get().OutputDeviceCount();

                LoadMidi();
            }
        }

        if (_effectWindowPanel.EffectChain() != m_Audio->m_EffectPanel.EffectChain())
        {
            _effectWindowPanel.EffectChain(m_Audio->m_EffectPanel.EffectChain());
            _effectWindowPanel.Name(m_Audio->m_EffectPanel.Name());
        }

        if (m_ScaleSlider->Value() != pscale)
        {
            pscale = m_ScaleSlider->Value();
            mainWindow.Scale(1.0 / (pscale * 0.01));
            effectWindow.Scale(1.0 / (pscale * 0.01));
            effectWindow.MaxSize({ (int)std::ceil(348 * (pscale * 0.01)), -1 });
            effectWindow.MinSize({ (int)std::ceil(348 * (pscale * 0.01)), -1 });
            effectWindow.Size({ 0, effectWindow.RealSize().height - 39 });
            RightClickMenu::Get().Scale(1.0 / (pscale * 0.01));
        }
        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 5 * 60 * 60;
            SaveSettings();
            m_Audio->SaveRouting();
            soundboard.Save();
        }
    }

    // Final save to make sure it is saved when exited.
    SaveSettings();
    m_Audio->SaveRouting();
    soundboard.Save();
}

void Controller::LoadSettings()
{
    m_LoadedSettings = false;
    try
    {
        std::ifstream _if;
        _if.open("./settings/settings");
        if (!_if.is_open())
            throw std::exception();

        nlohmann::json _json;
        _json << _if;
        _if.close();

        auto& theme = _json.at("theme").get<std::string>();
        auto device = _json.at("device").get<int>() + 1;
        auto zoom = _json.at("zoom").get<double>();

        for (auto& i : _json.at("midiin-enabled"))
        {
            if (std::find(m_MidiInEnabled.begin(), m_MidiInEnabled.end(), i.get<std::string>()) == m_MidiInEnabled.end())
                m_MidiInEnabled.push_back(i.get<std::string>());

            for (auto& dev : Midi::Get().InputDevices())
                if (dev.name == i.get<std::string>() && dev.id >= 0 && dev.id < m_MidiInButtons.size())
                    m_MidiInButtons[dev.id]->Active(true);
        }
        
        for (auto& i : _json.at("midiout-enabled"))
        {
            if (std::find(m_MidiOutEnabled.begin(), m_MidiOutEnabled.end(), i.get<std::string>()) == m_MidiOutEnabled.end())
                m_MidiOutEnabled.push_back(i.get<std::string>());

            for (auto& dev : Midi::Get().OutputDevices())
                if (dev.name == i.get<std::string>() && dev.id >= 0 && dev.id < m_MidiOutButtons.size())
                    m_MidiOutButtons[dev.id]->Active(true);
        }

        m_ScaleSlider->Value(zoom);
        m_AsioDropDown->Select(device);
        m_ThemeDropDown->Select(theme);

        SaveSettings();
    }
    catch (std::exception)
    {
        m_LoadedSettings = true;
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
        _json["midiin-enabled"] = nlohmann::json::array();
        for (auto& m : m_MidiInEnabled)
            _json["midiin-enabled"] += m;

        _json["midiout-enabled"] = nlohmann::json::array();
        for (auto& m : m_MidiOutEnabled)
            _json["midiout-enabled"] += m;

        std::ofstream _of;
        std::filesystem::path dir("./settings");
        std::filesystem::create_directories(dir);
        LOG(std::filesystem::current_path());
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
    m_LoadedSettings = true;
    LoadSettings();
}

void Controller::LoadMidi()
{
    // Load the midi ports
    Midi::Get().LoadPorts();

    // Get the vector of all midi devices
    auto& _devices = Midi::Get().InputDevices();
    int _deviceCount = _devices.size();

    // Some settings for the midiDevices Panel
    m_MidiInDevices->Div().Clear();
    m_MidiInDevices->Clear();
    m_MidiInDevices->Width(384);
    m_MidiInDevices->Height(_deviceCount * 24);
    m_MidiInDevices->Div().Align(Div::Alignment::Vertical);
    m_MidiInDevices->Div().Divs(_deviceCount);
    m_MidiInDevices->Div().DivSize(_deviceCount * 24);

    // Clear all MidiButtons
    m_MidiInButtons.clear();

    // Go through all the midi devices
    for (int i = 0; i < _deviceCount; i++)
    {
        // Make sure it is closed.
        Midi::Get().CloseInputPort(i);

        // Add the enable button and text component to the div.
        m_MidiInDevices->Div()[i] = { 4, Div::Alignment::Horizontal, 0, false, 24 };
        m_MidiInDevices->Div()[i][0].DivSize(8);
        auto& _text = m_MidiInDevices->Emplace<SMXRTextComponent>(_devices[i].name);
        _text.MaxWidth(_text.Width());
        m_MidiInDevices->Div()[i][1] = { _text, Div::Alignment::Left, Div::AUTO, true };
        auto& _b = m_MidiInDevices->Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(
            
            // The callback for the enable button
            [&, i](bool& s)
            {
                if (s)
                {
                    // If enabling, try to open port, if fails, set button to false again.
                    if (!Midi::Get().OpenInputPort(i)) s = false;

                    // Otherwise add the device to the enabled devices and save settings if it's not already enabled. 
                    else if (std::find(m_MidiInEnabled.begin(), m_MidiInEnabled.end(), _devices[i].name) == m_MidiInEnabled.end())
                        m_MidiInEnabled.push_back(_devices[i].name), SaveSettings();
                }
                else
                {
                    // If disabling, remove the device from the enabled devices, close the port and save settings.
                    m_MidiInEnabled.erase(std::remove_if(m_MidiInEnabled.begin(), m_MidiInEnabled.end(),
                        [&, i](auto& a) { return a == _devices[i].name; }));
                    Midi::Get().CloseInputPort(i);
                    SaveSettings();
                }
            }, "Enable");
        _b.Size({ 50, 18 });
        m_MidiInButtons.push_back(&_b);

        // Open the device if it's in the midiEnabled vector.
        auto& a = std::find(m_MidiInEnabled.begin(), m_MidiInEnabled.end(), _devices[i].name);
        if (a != m_MidiInEnabled.end())
            Midi::Get().OpenInputPort(i);

        // Also make sure that the 'enable' button is active if it is opened.
        if (Midi::Get().InputsOpened().find(i) != Midi::Get().InputsOpened().end())
            _b.Active(true);

        // Add the 'enable' button to the div.
        m_MidiInDevices->Div()[i][2] = { _b, Div::Alignment::Right, 50 };
        m_MidiInDevices->Div()[i][3].DivSize(8);
    }

    // Refresh the layout manager, so it recalculates all positions.
    m_MidiInDevices->LayoutManager().Refresh();

    // Get the vector of all midi devices
    auto& _outdevices = Midi::Get().OutputDevices();
    _deviceCount = _outdevices.size();

    // Some settings for the midiDevices Panel
    m_MidiOutDevices->Div().Clear();
    m_MidiOutDevices->Clear();
    m_MidiOutDevices->Width(384);
    m_MidiOutDevices->Height(_deviceCount * 24);
    m_MidiOutDevices->Div().Align(Div::Alignment::Vertical);
    m_MidiOutDevices->Div().Divs(_deviceCount);
    m_MidiOutDevices->Div().DivSize(_deviceCount * 24);

    // Clear all MidiButtons
    m_MidiOutButtons.clear();

    // Go through all the midi devices
    for (int i = 0; i < _deviceCount; i++)
    {
        // Make sure it is closed.
        Midi::Get().CloseOutputPort(i);

        // Add the enable button and text component to the div.
        m_MidiOutDevices->Div()[i] = { 4, Div::Alignment::Horizontal, 0, false, 24 };
        m_MidiOutDevices->Div()[i][0].DivSize(8);
        auto& _text = m_MidiOutDevices->Emplace<SMXRTextComponent>(_outdevices[i].name);
        _text.MaxWidth(_text.Width());
        m_MidiOutDevices->Div()[i][1] = { _text, Div::Alignment::Left, Div::AUTO, true };
        auto& _b = m_MidiOutDevices->Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(

            // The callback for the enable button
            [&, i](bool& s)
            {
                if (s)
                {
                    // If enabling, try to open port, if fails, set button to false again.
                    if (!Midi::Get().OpenOutputPort(i)) s = false;

                    // Otherwise add the device to the enabled devices and save settings if it's not already enabled. 
                    else if (std::find(m_MidiOutEnabled.begin(), m_MidiOutEnabled.end(), _outdevices[i].name) == m_MidiOutEnabled.end())
                        m_MidiOutEnabled.push_back(_outdevices[i].name), SaveSettings();
                }
                else
                {
                    // If disabling, remove the device from the enabled devices, close the port and save settings.
                    m_MidiOutEnabled.erase(std::remove_if(m_MidiOutEnabled.begin(), m_MidiOutEnabled.end(),
                        [&, i](auto& a) { return a == _outdevices[i].name; }));
                    Midi::Get().CloseOutputPort(i);
                    SaveSettings();
                }
            }, "Enable");
        _b.Size({ 50, 18 });
        m_MidiOutButtons.push_back(&_b);

        // Open the device if it's in the midiEnabled vector.
        auto& a = std::find(m_MidiOutEnabled.begin(), m_MidiOutEnabled.end(), _outdevices[i].name);
        if (a != m_MidiOutEnabled.end())
            Midi::Get().OpenOutputPort(i);

        // Also make sure that the 'enable' button is active if it is opened.
        if (Midi::Get().OutputsOpened().find(i) != Midi::Get().OutputsOpened().end())
            _b.Active(true);

        // Add the 'enable' button to the div.
        m_MidiOutDevices->Div()[i][2] = { _b, Div::Alignment::Right, 50 };
        m_MidiOutDevices->Div()[i][3].DivSize(8);
    }

    // Refresh the layout manager, so it recalculates all positions.
    m_MidiOutDevices->LayoutManager().Refresh();
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