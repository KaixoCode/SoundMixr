#include "Controller.hpp"
#include "EffectLoader.hpp"
#include <midi\Midi.hpp>

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
: mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 728, 500, true)),
settings(m_Gui.AddWindow<Frame>("Settings", 400, 500, true, false, false)),
soundboard(m_Gui.AddWindow<Soundboard>()),
m_AsioDevice(soundboard)
{}

void Controller::Run()
{
    EffectLoader::LoadEffects();

    //
    // Some namespaces
    //

    namespace G = SoundMixrGraphics; namespace BT = ButtonType; namespace MT = MenuType;
    using MenuButton = Button<G::Menu, BT::Normal>;
    using MenuToggleButton = Button<G::Menu, BT::Toggle>;
    using TitleMenuButton = Button<G::TitleMenu, BT::Menu<G::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<G::SubMenu, BT::Menu<G::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

    mainWindow.Color(ThemeT::Get().window_border);
    soundboard.Color(ThemeT::Get().window_border);
    mainWindow.Icon(IDI_ICON1);
    settings.Icon(IDI_ICON1);

    //
    // Set shell icon
    //

    Menu<G::Vertical, MT::Normal> _closeMenu;
    _closeMenu.ButtonSize({ 150, 20 });
    _closeMenu.Emplace<MenuButton>([] {}, "SoundMixr").Disable();
    auto& _div = _closeMenu.Emplace<MenuAccessories::Divider>(150, 1, 0, 4);
    _closeMenu.Emplace<MenuButton>([&] { mainWindow.Show(); }, "Open GUI");
    _closeMenu.Emplace<MenuButton>([&] { m_Gui.Close(); }, "Exit");

    Frame::AddShellIcon(IDI_ICON1, "SoundMixr", [&](Event& e)
        {
            if (e.button == Event::MouseButton::LEFT && e.mod)
                mainWindow.Show();

            if (e.button == Event::MouseButton::RIGHT)
                RightClickMenu::Get().Open(&_closeMenu, true);
        });

    //
    // Panels layout
    //

    auto& _panel = mainWindow.Panel();
    _panel.Layout<Layout::Grid>(1, 1, 8, 8);
    _panel.Background(ThemeT::Get().window_border);
    _panel.SmartPanel(false);

    auto& _p3 = _panel.Emplace<Panel>();
    _p3.Width(260);
    _p3.Layout<Layout::Border>(0, 0, false, false, false, false);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    auto& _titleButton = _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels");
    _titleButton.Disable();
    m_List = &_p3.Emplace<ListPanel>(Layout::Hint::Center, m_AsioDevice);

    //
    // Frame menu
    //

    auto& _menu = mainWindow.Menu();
    auto& _file = _menu.Emplace<TitleMenuButton>("Options");
    _file.Size({ 54, 32 });
    _file.MenuBase().ButtonSize({ 200, 20 });

    auto& _sub = _file.Emplace<MenuButton>([&] { settings.Show(); }, "Settings...", Key::CTRL_COMMA);


    //
    // Soundboard button
    //

    /*_file.Emplace<MenuToggleButton>([&](bool s)
        {
            if (s) soundboard.Show(); else soundboard.Hide();
        }, "Soundboard", Key::CTRL_SHIFT_S);*/


    _file.Emplace<MenuButton>([&]
        {
            m_Gui.Close();
        }, "Exit");


    //
    // Settings
    //

    auto& _ppp = settings.Panel();
    _ppp.Layout<Layout::Grid>(1, 1, 8, 8);
    auto& _sp = _ppp.Emplace<Panel>();
    auto& _scrp = _sp.Emplace<SMXRScrollPanel>();
    _scrp.Size({ 384, 100 });
    _scrp.EnableScrollbars(false, true);
    auto& _midiDevices = _scrp.Panel<Panel>();
    _midiDevices.Layout<Layout::Divs>();

    auto& _themeCallback = [&]
    {
        _panel.Background(ThemeT::Get().window_border);
        mainWindow.Color(ThemeT::Get().window_border);
        soundboard.Color(ThemeT::Get().window_border);
        settings.Color(ThemeT::Get().window_border);
        m_List->Panel().Background(ThemeT::Get().window_panel);
        m_List->Background(ThemeT::Get().window_panel);
        _div.Color(ThemeT::Get().divider);
        _sp.LayoutManager().DividerColor(ThemeT::Get().divider);
        _sp.Background(ThemeT::Get().window_panel);
        _scrp.Background(ThemeT::Get().window_frame);
    };

    auto& _refreshEffect = [&]
    {
        SaveRouting();
        m_AsioDevice.RemoveGroups();
        m_List->Clear();
        EffectLoader::LoadEffects();
        m_List->ReloadEffects();
        LoadRouting();
    };

    auto& _resetGroup = [&] { m_List->ResetGrouping(); };

    auto& _asioControlPanel = _sp.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>([&]
        {
            if (!&m_AsioDevice.Device())
                return;

            SaveRouting();
            m_AsioDevice.CloseStream();
            PaAsio_ShowControlPanel(m_AsioDevice.Device().id, mainWindow.GetWin32Handle());
            m_AsioDevice.OpenStream();
            m_List->Clear();
            LoadRouting();
            m_AsioDevice.StartStream();
        }, "Control Panel...");
    _asioControlPanel.Size({ 110, 18 });

    auto& _refreshEffects = _sp.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(_refreshEffect, "Reload");
    _refreshEffects.Size({ 110, 18 });

    auto& _resetGrouping = _sp.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(_resetGroup, "Reset");
    _resetGrouping.Size({ 110, 18 });

    auto& _themeDropDown = _sp.Emplace<DropDown<std::string, DropdownButton2>>();
    _themeDropDown.Size({ 110, 18 });
    _themeDropDown.ButtonSize({ 110, 20 });

    bool loaded = false;
    auto& _saveSettings = [&]
    {
        if (!loaded)
            return;
        try
        {
            nlohmann::json _json = nlohmann::json::object();
            _json["device"] = (&m_AsioDevice.Device() != nullptr ? m_AsioDevice.Device().id : -1);
            _json["theme"] = ThemeT::Get().Name();
            _json["midi-enabled"] = nlohmann::json::array();
            for (auto& [key, val] : Midi::Get().Opened())
                _json["midi-enabled"] += key;

            std::ofstream _of;
            _of.open("./settings/settings");
            _of << std::setw(4) << _json;
            _of.close();
        }
        catch (...)
        {
            LOG("Failed to save settings.");
        }
    };

    auto& _asioDropDown = _sp.Emplace<DropDown<int, DropdownButton2>>();
    _asioDropDown.Size({ 220, 18 });
    _asioDropDown.ButtonSize({ 220, 20 });
    _asioDropDown.AddOption("No Device", 0, [&](int i)
        {
            _asioControlPanel.Disable();

            // First save the routing
            SaveRouting();

            // Close stream, set new device and open/start stream
            m_AsioDevice.CloseStream();
            m_AsioDevice.NoDevice();

            // Clear the channels from the ListPanel
            m_List->Clear();
            _titleButton.Name("No Device");

            // Save the current device 
            _saveSettings();
        });
    for (auto& _d : m_AsioDevice.Devices())
    {
        _asioDropDown.AddOption(_d.info.name, _d.id + 1, [&](int i)
            {
                if (&m_AsioDevice.Device() != nullptr && m_AsioDevice.Device().id == _d.id)
                    return;
                _asioControlPanel.Enable();

                // First save the routing
                SaveRouting();

                // Close stream, set new device and open/start stream
                m_AsioDevice.CloseStream();

                // Clear the channels from the ListPanel
                m_List->Clear();
                m_AsioDevice.Device(_d);
                m_AsioDevice.OpenStream();

                // Set title to ASIO device name
                _titleButton.Name(_d.info.name);

                // Save the current device 
                _saveSettings();

                // Load the routing for this new device.
                LoadRouting();
                m_AsioDevice.StartStream();
            });
    }

    std::vector<Button<ToggleButtonGraphics, ButtonType::Toggle>*> m_MidiButtons;

    auto& _loadSettings = [&]
    {
        try
        {
            std::ifstream _if;
            _if.open("./settings/settings");
            nlohmann::json _json;
            _json << _if;
            _if.close();

            auto& theme = _json.at("theme").get<std::string>();
            auto device = _json.at("device").get<int>() + 1;
            
            for (auto& i : _json.at("midi-enabled"))
                m_MidiButtons[i.get<int>()]->Active(true);

            _asioDropDown.Select(device);
            _themeDropDown.Select(theme);

            _saveSettings();
        }
        catch (...)
        {
            LOG("Failed to load settings.");
        }
    };

    auto& _refreshMidi = [&] 
    {
        Midi::Get().LoadPorts();
        auto& _dev = Midi::Get().Devices();
        int _devs = _dev.size();
        _midiDevices.Div().Clear();
        _midiDevices.Clear();
        _midiDevices.Width(384);
        _midiDevices.Height(_devs * 24);
        _midiDevices.Div().Align(Div::Alignment::Vertical);
        _midiDevices.Div().Divs(_devs);
        _midiDevices.Div().DivSize(_devs * 24);
        m_MidiButtons.clear();
        for (int i = 0; i < _devs; i++)
        {
            _midiDevices.Div()[i].Align(Div::Alignment::Horizontal);
            _midiDevices.Div()[i].Divs(4);
            _midiDevices.Div()[i].DivSize(24);
            _midiDevices.Div()[i][0].DivSize(8);
            _midiDevices.Div()[i][1].DivSize(150);
            _midiDevices.Div()[i][1].Align(Div::Alignment::Left);
            _midiDevices.Div()[i][1] = _midiDevices.Emplace<TextComponent<Align::LEFT>>(_dev[i].name);
            auto& _b = _midiDevices.Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(
                [i, _saveSettings](bool& s)
                {
                    if (s)
                    {
                        if (!Midi::Get().OpenPort(i)) s = false;
                    }
                    else
                        Midi::Get().ClosePort(i);
                }, "Enable");
            m_MidiButtons.push_back(&_b);
            if (Midi::Get().Opened().find(i) != Midi::Get().Opened().end())
                _b.Active(true);

            _b.Size({ 50, 18 });
            _midiDevices.Div()[i][2].Align(Div::Alignment::Right);
            _midiDevices.Div()[i][2] = _b;
            _midiDevices.Div()[i][3].DivSize(8);
        }
        _midiDevices.LayoutManager().m_PrevDim = { 0 ,0, 0, 0 };
    };

    auto& _refreshMidiDevices = _sp.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(_refreshMidi, "Refresh List");
    _refreshMidiDevices.Size({ 110, 18 });

    auto& _themeLoader = [&, _themeCallback, _saveSettings, _loadSettings]
    {
        _saveSettings();
        ThemeT::ReloadThemes();
        _themeDropDown.Clear();
        for (auto& [key, val] : ThemeT::Themes())
        {
            _themeDropDown.AddOption(key, key, [&](std::string& t)
                {
                    ThemeT::SetTheme(t);
                    _saveSettings();
                    _themeCallback();
                });
        }
        _loadSettings();
        _themeCallback();
    };
    
    _refreshMidi();
    _themeLoader();
    loaded = true;

    auto& _reloadThemes = _sp.Emplace<Button<NormalButtonGraphics, ButtonType::Normal>>(_themeLoader, "Reload");
    _reloadThemes.Size({ 110, 18 });

    _sp.Layout<Layout::Divs>();
    _sp.Div().Align(Div::Alignment::Vertical);
    _sp.Div().Divs(4);
    _sp.Div()[3].Align(Div::Alignment::Vertical);
    _sp.Div()[3].Dividers(true);
    _sp.Div()[3].Divs(2);
    _sp.Div()[3].DivSize(120);
    _sp.Div()[3][1].DivSize(36);
    _sp.Div()[3][1] = _sp.Emplace<TextComponent<>>("Asio Settings", Graphics::Fonts::Gidole, 24.0f);;
    _sp.Div()[3][0].Align(Div::Alignment::Vertical);
    _sp.Div()[3][0].Divs(3);
    _sp.Div()[3][0][2].DivSize(4);
    _sp.Div()[3][0][1].Align(Div::Alignment::Horizontal);
    _sp.Div()[3][0][1].Divs(2);
    _sp.Div()[3][0][1][0].DivSize(10);
    _sp.Div()[3][0][1][1].Align(Div::Alignment::Vertical);
    _sp.Div()[3][0][1][1].Divs(3);
    _sp.Div()[3][0][1][1][1].Align(Div::Alignment::Horizontal);
    _sp.Div()[3][0][1][1][1].Divs(2);
    _sp.Div()[3][0][1][1][1].DivSize(24);
    _sp.Div()[3][0][1][1][1][0].DivSize(150);
    _sp.Div()[3][0][1][1][1][0].Align(Div::Alignment::Left);
    _sp.Div()[3][0][1][1][1][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Asio Device");
    _sp.Div()[3][0][1][1][1][1].Align(Div::Alignment::Left);
    _sp.Div()[3][0][1][1][1][1] = _asioDropDown;
    _sp.Div()[3][0][1][1][2].Align(Div::Alignment::Horizontal);
    _sp.Div()[3][0][1][1][2].Divs(2);
    _sp.Div()[3][0][1][1][2].DivSize(24);
    _sp.Div()[3][0][1][1][2][0].DivSize(150);
    _sp.Div()[3][0][1][1][2][0].Align(Div::Alignment::Left);
    _sp.Div()[3][0][1][1][2][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Control Panel");
    _sp.Div()[3][0][1][1][2][1].Align(Div::Alignment::Left);
    _sp.Div()[3][0][1][1][2][1] = _asioControlPanel;
    _sp.Div()[3][0][0].DivSize(20);
    _sp.Div()[2].Align(Div::Alignment::Vertical);
    _sp.Div()[2].Dividers(true);
    _sp.Div()[2].Divs(2);
    _sp.Div()[2].DivSize(190);
    _sp.Div()[2][1].DivSize(36);
    _sp.Div()[2][1] = _sp.Emplace<TextComponent<>>("Midi Settings", Graphics::Fonts::Gidole, 24.0f);
    _sp.Div()[2][0].DivSize(158);
    _sp.Div()[2][0].Divs(3);
    _sp.Div()[2][0].Align(Div::Alignment::Vertical);
    _sp.Div()[2][0][2].DivSize(108);
    _sp.Div()[2][0][2] = _scrp;
    _sp.Div()[2][0][1].DivSize(30);
    _sp.Div()[2][0][1] = _refreshMidiDevices;
    _sp.Div()[2][0][0].DivSize(20);
    _sp.Div()[1].Align(Div::Alignment::Vertical);
    _sp.Div()[1].Dividers(true);
    _sp.Div()[1].Divs(2);
    _sp.Div()[1].DivSize(176);
    _sp.Div()[1][1].DivSize(36);
    _sp.Div()[1][1] = _sp.Emplace<TextComponent<>>("General Settings", Graphics::Fonts::Gidole, 24.0f);
    _sp.Div()[1][0].Divs(3);
    _sp.Div()[1][0].Align(Div::Alignment::Vertical);
    _sp.Div()[1][0][2].DivSize(4);
    _sp.Div()[1][0][1].Align(Div::Alignment::Horizontal);
    _sp.Div()[1][0][1].Divs(2);
    _sp.Div()[1][0][1][0].DivSize(10);
    _sp.Div()[1][0][1][1].Align(Div::Alignment::Vertical);
    _sp.Div()[1][0][1][1].Divs(4);
    _sp.Div()[1][0][1][1][0].Align(Div::Alignment::Horizontal);
    _sp.Div()[1][0][1][1][0].Divs(2);
    _sp.Div()[1][0][1][1][0].DivSize(26);
    _sp.Div()[1][0][1][1][0][0].DivSize(150);
    _sp.Div()[1][0][1][1][0][0].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][0][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Reload Themes");
    _sp.Div()[1][0][1][1][0][1].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][0][1] = _reloadThemes;
    _sp.Div()[1][0][1][1][1].Align(Div::Alignment::Horizontal);
    _sp.Div()[1][0][1][1][1].Divs(2);
    _sp.Div()[1][0][1][1][1].DivSize(26);
    _sp.Div()[1][0][1][1][1][0].DivSize(150);
    _sp.Div()[1][0][1][1][1][0].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][1][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Theme");
    _sp.Div()[1][0][1][1][1][1].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][1][1] = _themeDropDown;
    _sp.Div()[1][0][1][1][2].Align(Div::Alignment::Horizontal);
    _sp.Div()[1][0][1][1][2].Divs(2);
    _sp.Div()[1][0][1][1][2].DivSize(26);
    _sp.Div()[1][0][1][1][2][0].DivSize(150);
    _sp.Div()[1][0][1][1][2][0].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][2][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Reload Effects");
    _sp.Div()[1][0][1][1][2][1].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][2][1] = _refreshEffects;
    _sp.Div()[1][0][1][1][3].Align(Div::Alignment::Horizontal);
    _sp.Div()[1][0][1][1][3].Divs(2);
    _sp.Div()[1][0][1][1][3].DivSize(26);
    _sp.Div()[1][0][1][1][3][0].DivSize(150);
    _sp.Div()[1][0][1][1][3][0].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][3][0] = _sp.Emplace<TextComponent<Align::LEFT>>("Reset Channel Grouping");
    _sp.Div()[1][0][1][1][3][1].Align(Div::Alignment::Left);
    _sp.Div()[1][0][1][1][3][1] = _resetGrouping;
    _sp.Div()[1][0][0].DivSize(20);

    //
    // Main loop
    //

    int _saveCounter = 5 * 60 * 60;
    while (m_Gui.Loop())
    {
        Midi::Get().ReadMessages();

        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 5 * 60 * 60;
            _saveSettings();
            SaveRouting();
        }
    }

    // Final save to make sure it is saved when exited.
    _saveSettings();
    SaveRouting();
}

void Controller::SaveRouting()
{
    if (&m_AsioDevice.Device() == nullptr)
        return;

    LOG("Saving Routing");

    nlohmann::json _json;
    _json["input_channels"] = nlohmann::json::array();

    // Inputs
    for (auto& _ch : m_List->Channels())
    {
        if (!_ch->IsInput() || _ch->IsSpecial())
            continue;

        _json["input_channels"].push_back(*_ch);
    }

    _json["output_channels"] = nlohmann::json::array();

    // Outputs
    for (auto& _ch : m_List->Channels())
    {
        if (_ch->IsInput() || _ch->IsSpecial())
            continue;

        _json["output_channels"].push_back(*_ch);
    }

    _json["special_channels"] = nlohmann::json::array();

    // Special channels
    for (auto& _ch : m_List->Channels())
    {
        if (!_ch->IsSpecial())
            continue;

        _json["special_channels"].push_back(*_ch);
    }

    std::ofstream _out;
    _out.open("./settings/testrouting" + std::to_string(m_AsioDevice.Device().id));
    _out << std::setw(4) << _json;
    _out.close();
}

void Controller::LoadRouting()
{
    if (m_List == nullptr || &m_AsioDevice.Device() == nullptr)
        return;

    // Keep track which channels have been loaded from the file so
    // we can later add the ones that weren't added separately.
    std::unordered_map<int, bool> _inputIdsLoaded, _outputIdsLoaded;
    for (int i = 0; i < m_AsioDevice.Device().info.maxInputChannels; i++)
        _inputIdsLoaded.emplace(i, false);

    for (int i = 0; i < m_AsioDevice.Device().info.maxOutputChannels; i++)
        _outputIdsLoaded.emplace(i, false);

    // Open the routing file for the current device.
    LOG("Loading Routing");
    std::ifstream _in;
    _in.open("./settings/testrouting" + std::to_string(m_AsioDevice.Device().id));

    bool _error = true;
    if (!_in.fail())
    {
        _error = false;
        try 
        {
            nlohmann::json _json;
            _in >> _json;

            // First load all the output channels
            auto _outputs = _json.at("output_channels");
            for (auto& i : _outputs)
            {
                // Emplace a channelgroup to the list
                auto& _c = m_List->EmplaceChannel(false);
           
                // Add all channels that are in this channelgroup
                nlohmann::json _channels = i.at("channels");
                for (int i : _channels)
                {
                    _outputIdsLoaded[i] = true;
                    _c.AddChannel(&m_AsioDevice.Outputs()[i]);
                }

                // Set the rest of the parameters
                _c = i;
            }

            // Load all the input channels
            auto _inputs = _json.at("input_channels");
            for (auto& i : _inputs)
            {
                // Emplace a channelgroup to the list
                auto& _c = m_List->EmplaceChannel(true);

                // Add all channels that are in this channelgroup
                nlohmann::json _channels = i.at("channels");
                for (int i : _channels)
                {
                    _inputIdsLoaded[i] = true;
                    _c.AddChannel(&m_AsioDevice.Inputs()[i]);
                }

                // Then add all the connections of this channelgroup
                nlohmann::json _connections = i.at("connections");
                for (int i : _connections)
                {
                    // Find the output channelgroup by id.
                    auto& _it = std::find_if(m_List->Channels().begin(), m_List->Channels().end(),
                        [=](ChannelPanel* c) { return !c->IsInput() && c->ChannelGroup().ID() == i; }
                    );
                
                    // If it exists, connect with it.
                    if (_it != m_List->Channels().end())
                        _c.ChannelGroup().Connect(&(*_it)->ChannelGroup());
                }

                _c = i;
            }

            auto& _soundboardChannel = m_List->EmplaceSpecialChannel();
            for (auto& a : m_AsioDevice.SoundboardChannels())
                _soundboardChannel.AddChannel(&a);

            // Then add all the connections of this channelgroup
            nlohmann::json _connections = _json.at("special_channels")[0].at("connections");
            for (int i : _connections)
            {
                // Find the output channelgroup by id.
                auto& _it = std::find_if(m_List->Channels().begin(), m_List->Channels().end(),
                    [=](ChannelPanel* c) { return !c->IsInput() && c->ChannelGroup().ID() == i; }
                );

                // If it exists, connect with it.
                if (_it != m_List->Channels().end())
                    _soundboardChannel.ChannelGroup().Connect(&(*_it)->ChannelGroup());
            }

            _soundboardChannel = _json.at("special_channels")[0];
        }
    
        // If error occured (either file didn't exist or was parced incorrectly
        // load all the channels as stereo channels.
        catch (...)
        {
            _error = true;
        }
    }

    if (_error)
    {
        m_List->Clear();

        auto& _soundboardChannel = m_List->EmplaceSpecialChannel();
        for (auto& a : m_AsioDevice.SoundboardChannels())
            _soundboardChannel.AddChannel(&a);

        int i = 0;
        for (i = 0; i < m_AsioDevice.Device().info.maxInputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel(true);
            _c.AddChannel(&m_AsioDevice.Inputs()[i]);
            _c.AddChannel(&m_AsioDevice.Inputs()[i + 1]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.Value(0);
            _c.volume.Value(1);
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == m_AsioDevice.Device().info.maxInputChannels - 1)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel(true);
            _c.AddChannel(&m_AsioDevice.Inputs()[i]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.Value(0);
            _c.volume.Value(1);
        }

        for (i = 0; i < m_AsioDevice.Device().info.maxOutputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the outputs
            auto& _c = m_List->EmplaceChannel(false);
            _c.AddChannel(&m_AsioDevice.Outputs()[i]);
            _c.AddChannel(&m_AsioDevice.Outputs()[i + 1]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.Value(0);
            _c.volume.Value(1);
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == m_AsioDevice.Device().info.maxOutputChannels - 1)
        {
            // Add a ChannelPanel with all the outputs
            auto& _c = m_List->EmplaceChannel(false);
            _c.AddChannel(&m_AsioDevice.Outputs()[i]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.Value(0);
            _c.volume.Value(1);
        }

        _in.close();// Close the file!! important!!
        return;
    }

    _in.close();// Close the file!! important!!

    // If a couple channels were loaded then just load all the 
    // channels from the ASIO that weren't loaded from the file
    // as mono channels.
    for (auto& i : _inputIdsLoaded)
    {
        if (!i.second)
        {
            // Add a ChannelPanel with the input
            auto& _c = m_List->EmplaceChannel(true);
            _c.AddChannel(&m_AsioDevice.Inputs()[i.first]);
        }
    }

    for (auto& i : _outputIdsLoaded)
    {
        if (!i.second)
        {
            // Add a ChannelPanel with the output
            auto& _c = m_List->EmplaceChannel(false);
            _c.AddChannel(&m_AsioDevice.Outputs()[i.first]);
        }
        
    }
}