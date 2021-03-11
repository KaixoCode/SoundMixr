#include "Controller.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
: mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 728, 500, true)),
soundboard(m_Gui.AddWindow<Soundboard>()),
m_AsioDevice(soundboard)
{}

void Controller::Run()
{

    // 
    // Loading Theme
    //

    LOG("Selecting default theme");
    std::ifstream _in;
    _in.open("./settings/theme");
    std::string _line;
    std::getline(_in, _line);
    int _themeId = std::atoi(_line.c_str());
    Themes::Theme = (Themes::N) _themeId;
    _in.close();

    //
    // Some namespaces
    //

    namespace G = SoundMixrGraphics; namespace BT = ButtonType; namespace MT = MenuType;
    using MenuButton = Button<G::Menu, BT::Normal>;
    using MenuToggleButton = Button<G::Menu, BT::Toggle>;
    using TitleMenuButton = Button<G::TitleMenu, BT::Menu<G::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<G::SubMenu, BT::Menu<G::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

    mainWindow.Color(Theme<C::WindowBorder>::Get());
    soundboard.Color(Theme<C::WindowBorder>::Get());
    mainWindow.Icon(ASSET("textures/logo.png"));

    //
    // Set shell icon
    //

    Menu<G::Vertical, MT::Normal> _closeMenu;
    _closeMenu.ButtonSize({ 150, 20 });
    _closeMenu.Emplace<MenuButton>([] {}, "SoundMixr").Disable();
    auto& _div = _closeMenu.Emplace<MenuAccessories::Divider>(150, 1, 0, 4);
    _closeMenu.Emplace<MenuButton>([&] { mainWindow.Show(); }, "Open GUI");
    _closeMenu.Emplace<MenuButton>([&] { m_Gui.Close(); }, "Exit");

    Frame::AddShellIcon("SoundMixr.ico", "SoundMixr", [&] (Event& e)
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
    _panel.Background(Theme<C::WindowBorder>::Get());
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

    auto& _sub = _file.Emplace<SubMenuButton>("Select ASIO Device");
    _sub.MenuBase().ButtonSize({ 210, 20 });
    int _key = BT::List::NewKey();


    //
    // Load default device
    //

    LOG("Selecting default device");
    _in.open("./settings/device");
    std::getline(_in, _line);
    int _deviceId = std::atoi(_line.c_str());
    if (!_line.empty())
    {
        auto _it = std::find_if(m_AsioDevice.Devices().begin(), m_AsioDevice.Devices().end(), [=](const Device& obj) {return obj.id == _deviceId; });
        if (_it != m_AsioDevice.Devices().end()) {
            m_AsioDevice.Device(*_it);
            m_AsioDevice.OpenStream();
            m_AsioDevice.StartStream();
            LoadRouting();
            _titleButton.Name(m_AsioDevice.Device().info.name);
        }
    }
    _in.close();

    //
    // Load selection menu for ASIO devices
    //

    for (auto& _d : m_AsioDevice.Devices())
    {
        auto& _button = _sub.Emplace<Button<G::Menu, BT::List>>([&]
            {
                if (&m_AsioDevice.Device() != nullptr && m_AsioDevice.Device().id == _d.id)
                    return;

                // First save the routing
                SaveRouting();

                // Close stream, set new device and open/start stream
                m_AsioDevice.CloseStream();

                // Clear the channels from the ListPanel
                m_List->Clear();
                m_AsioDevice.Device(_d);
                m_AsioDevice.OpenStream();
                m_AsioDevice.StartStream();

                // Set title to ASIO device name
                _titleButton.Name(_d.info.name);

                // Save the current device 
                std::ofstream _out;
                _out.open("./settings/device");
                _out << std::to_string(_d.id);
                _out.close();

                // Load the routing for this new device.
                LoadRouting();
            }, _d.info.name, _key);

        // Make sure the correct initial button in the List Group is set to 'Selected'
        if (_d.id == _deviceId)
            _button.Selected(true);
    }

    //
    // ASIO Control Panel button in Frame menu
    //

    _file.Emplace<MenuButton>([&]
        {
            if (!&m_AsioDevice.Device())
                return;

            SaveRouting();
            m_AsioDevice.CloseStream();
            PaAsio_ShowControlPanel(m_AsioDevice.Device().id, mainWindow.GetWin32Handle());
            m_AsioDevice.OpenStream();
            m_AsioDevice.StartStream();
            m_List->Clear();
            LoadRouting();
        }, "ASIO Control Panel", Key::CTRL_O);

    //
    // The Windows Aero Effect button in the Frame menu
    //

    _file.Emplace<MenuToggleButton>([&](bool a)
        {
            m_List->Transparency(a);
            mainWindow.Aero(a);
        }, "Windows Aero Effect", Key::CTRL_T);


    /*_file.Emplace<MenuButton>([&]
        {
            SaveRouting();
        }, "Save Routing", Key::CTRL_S);*/

    _file.Emplace<MenuButton>([&]
        {
            soundboard.Save();
        }, "Save Soundboard", Key::CTRL_S);

    //
    // Themes
    //

    // Callback that sets colors of staticly colored gui 
    // components, and saves the theme.
    auto& _themeCallback = [&] 
    {            
        _panel.Background(Theme<C::WindowBorder>::Get());
        mainWindow.Color(Theme<C::WindowBorder>::Get());
        soundboard.Color(Theme<C::WindowBorder>::Get());
        m_List->Panel().Background(Theme<C::MainPanel>::Get());
        m_List->Background(Theme<C::MainPanel>::Get());
        _div.Color(Theme<C::Divider>::Get());

        std::ofstream _out;
        _out.open("./settings/theme");
        _out << std::to_string(Themes::Theme);
        _out.close();
    };

    auto& _sub2 = _file.Emplace<SubMenuButton>("Theme");
    _sub2.MenuBase().ButtonSize({ 120, 20 });
    _key = BT::List::NewKey();
    auto& _theme1 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::DARK;
            _themeCallback();
        }, "Dark", _key);
    if (Themes::Theme == Themes::DARK) _theme1.Selected(true);

    /*auto& _theme2 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::LIGHT;
            _themeCallback();
        }, "Light", _key);
    if (Themes::Theme == Themes::LIGHT) _theme2.Selected(true);*/

    auto& _theme3 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::BLUE;
            _themeCallback();
        }, "Blue", _key);
    if (Themes::Theme == Themes::BLUE) _theme3.Selected(true);

    //
    // Reset channel grouping button.
    //

    _file.Emplace<MenuButton>([&]
        {
            m_List->ResetGrouping();
        }, "Reset Grouping");

    //
    // Soundboard button
    //

    _file.Emplace<MenuToggleButton>([&](bool s)
        {
            if (s) soundboard.Show(); else soundboard.Hide();
        }, "Soundboard", Key::CTRL_SHIFT_S);


    _file.Emplace<MenuButton>([&]
        {
            m_Gui.Close();
        }, "Exit");

    //
    // Main loop
    //

    int _saveCounter = 5 * 60 * 60;
    while (m_Gui.Loop())
    {
        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 5 * 60 * 60;
            SaveRouting();
        }
    }

    // Final save to make sure it is saved when exited.
    SaveRouting();
}

void Controller::SaveRouting()
{
    if (&m_AsioDevice.Device() == nullptr)
        return;

    LOG("Saving Routing");

    json _json;
    _json["input_channels"] = json::array();

    // Inputs
    for (auto& _ch : m_List->Channels())
    {
        if (!_ch->IsInput() || _ch->IsSpecial())
            continue;

        _json["input_channels"].push_back(*_ch);
    }

    _json["output_channels"] = json::array();

    // Outputs
    for (auto& _ch : m_List->Channels())
    {
        if (_ch->IsInput() || _ch->IsSpecial())
            continue;

        _json["output_channels"].push_back(*_ch);
    }

    _json["special_channels"] = json::array();

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
            json _json;
            _in >> _json;

            // First load all the output channels
            auto _outputs = _json.at("output_channels");
            for (auto& i : _outputs)
            {
                // Emplace a channelgroup to the list
                auto& _c = m_List->EmplaceChannel(false);
           
                // Add all channels that are in this channelgroup
                json _channels = i.at("channels");
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
                json _channels = i.at("channels");
                for (int i : _channels)
                {
                    _inputIdsLoaded[i] = true;
                    _c.AddChannel(&m_AsioDevice.Inputs()[i]);
                }

                // Then add all the connections of this channelgroup
                json _connections = i.at("connections");
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
            json _connections = _json.at("special_channels")[0].at("connections");
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
        catch (json::parse_error err)
        {
            _error = true;
        }
        catch (json::other_error err)
        {
            _error = true;
        }
        catch (json::type_error err)
        {
            _error = true;
        }
        catch (std::exception& e)
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