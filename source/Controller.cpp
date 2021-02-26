#include "Controller.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
: mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 728, 500, true)),
soundboard(m_Gui.AddWindow<Soundboard>())
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
                RightClickMenu::Get().Open(&_closeMenu);
        });

    //
    // Panels layout
    //

    auto& _panel = mainWindow.Panel();
    _panel.Layout<Layout::Grid>(1, 1, 8, 8);
    _panel.Background(Theme<C::WindowBorder>::Get());

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
    m_List->Background(Theme<C::MainPanel>::Get());
    auto& _p33 = m_List->Panel();
    _p33.Background(Theme<C::MainPanel>::Get());
    _p33.Layout<Layout::SidewaysStack>(0);
    _p33.AutoResize(true, false);

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
        m_AsioDevice.Device(*_it);
        m_AsioDevice.OpenStream();
        m_AsioDevice.StartStream();
        LoadRouting();
        _titleButton.Name(m_AsioDevice.Device().info.name);
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

                // Clear the channels from the ListPanel
                m_List->Clear();

                // Close stream, set new device and open/start stream
                m_AsioDevice.CloseStream();
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
        _p33.Background(Theme<C::MainPanel>::Get());
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

    auto& _theme2 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::LIGHT;
            _themeCallback();
        }, "Light", _key);
    if (Themes::Theme == Themes::LIGHT) _theme2.Selected(true);

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
    // Vertical UI button
    //

    /*_file.Emplace<MenuToggleButton>([&](bool s) 
        {
            if (s)
            {
                _p33.Layout<Layout::Stack>(8);
                _p33.AutoResize(false, true);
                _channelPanel.Vertical();
            }
            else
            {
                _p33.Layout<Layout::SidewaysStack>(8);
                _p33.AutoResize(true, false);
                _channelPanel.Horizontal();
            }
            _p33.Background(Color{ 40, 40, 40, (s ? 245.0f : 255.0f) });
        }, "Vertical UI", Key::CTRL_L);*/

    //
    // Soundboard button
    //

    /*_file.Emplace<MenuToggleButton>([&] (bool s)
        {
            if (s) soundboard.Show(); else soundboard.Hide();
        }, "Soundboard", Key::CTRL_SHIFT_S);*/

    //
    // Main loop
    //

    int _saveCounter = 1000;
    while (m_Gui.Loop())
    {
        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 60 * 60;
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
    std::string data;
    for (auto& _ch : m_List->OutputChannels())
    {
        auto& _i = _ch->Channels();
        data += "out:";
        for (auto& _a : _i.Channels())
            data += std::to_string(_a->ID()) + ",";
        data += ";";
        data += std::to_string(_i.Muted()) + ";";
        data += std::to_string(_i.Mono()) + ";";
        data += std::to_string(_i.Pan()) + ";";
        data += std::to_string(_i.Volume()) + "\n";
    }

    for (auto& _ch : m_List->InputChannels())
    {
        auto& _i = _ch->Channels();
        data += "in:";
        for (auto& _a : _i.Channels())
            data += std::to_string(_a->ID()) + ",";
        data += ";";
        data += std::to_string(_i.Muted()) + ";";
        data += std::to_string(_i.Mono()) + ";";
        data += std::to_string(_i.Pan()) + ";";
        data += std::to_string(_i.Volume()) + ";";
        for (auto& i : _i.Connections())
            data += std::to_string(i->ID()) + ",";
        data += "\n";
    }

    std::ofstream _out;
    _out.open("./settings/routing" + std::to_string(m_AsioDevice.Device().id));
    _out << data;
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
    _in.open("./settings/routing" + std::to_string(m_AsioDevice.Device().id));
    std::string _line;

    // Get all lines in the file
    int _linesLoaded = 0;
    while (std::getline(_in, _line))
    {
        _linesLoaded++;
        size_t p = _line.find_first_of(":") + 1;
        std::string _type = _line.substr(0, p - 1);
        std::string _rest = _line.substr(p);

        // First in line are all ids
        p = _rest.find_first_of(";");
        std::string _idsS = _rest.substr(0, p);
        _rest = _rest.substr(p + 1);
        std::vector<int> _ids;
        while ((p = _idsS.find_first_of(",")) != -1)
        {
            std::string _idS = _idsS.substr(0, p);
            int _id = std::atoi(_idS.c_str());
            _idsS = _idsS.substr(p + 1);
            _ids.push_back(_id);
        }

        // Muted
        p = _rest.find_first_of(";");
        std::string _mutedS = _rest.substr(0, p);
        bool _muted = std::atoi(_mutedS.c_str());
        _rest = _rest.substr(p + 1);

        // Mono
        p = _rest.find_first_of(";");
        std::string _monoS = _rest.substr(0, p);
        bool _mono = std::atoi(_monoS.c_str());
        _rest = _rest.substr(p + 1);

        // Pan
        p = _rest.find_first_of(";");
        std::string _panS = _rest.substr(0, p);
        float _pan = std::atof(_panS.c_str());
        _rest = _rest.substr(p + 1);

        // Volume
        p = _rest.find_first_of(";");
        std::string _volumeS = _rest.substr(0, p);
        float _volume = std::atof(_volumeS.c_str());
        _rest = _rest.substr(p + 1);

        // Debug
        LOG("LOADING FROM FILE: \nids:");
        for (int i : _ids)
            LOG(i);
        LOG("muted: " << _muted);
        LOG("mono: " << _mono);
        LOG("pan: " << _pan);
        LOG("volume: " << _volume);

        if (_type == "in")
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel<InputChannelPanel>();
            for (int i : _ids)
            {
                _inputIdsLoaded[i] = true;
                _c.AddChannel(&m_AsioDevice.Inputs()[i]);
            }

            // Set all parameters of this Channel
            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);

            // Load the routing for this panel
            auto& _out = m_List->OutputChannels();
            while ((p = _rest.find_first_of(",")) != -1)
            {
                // Find the next id in the file
                std::string _linkS = _rest.substr(0, p);
                int _link = std::atoi(_linkS.c_str());
                _rest = _rest.substr(p + 1);
                
                // Find the OutputChannelPanel belonging to that id and connect them
                auto _it = std::find_if(_out.begin(), _out.end(), [&_link](OutputChannelPanel* obj)
                    { return obj->Channels().ID() == _link; }
                );

                if (_it != _out.end())
                {
                    LOG(_c.Channels().ID() << " routed to: " << _link);
                    auto _index = std::distance(_out.begin(), _it);
                    _c.Channels().Connect(&_out[_index]->Channels());
                }
            }
        }
        else
        {
            // Add ChannelPanel with all the outputs
            auto& _c = m_List->EmplaceChannel<OutputChannelPanel>();
            for (int i : _ids)
            {
                _outputIdsLoaded[i] = true;
                _c.AddChannel(&m_AsioDevice.Outputs()[i]);
            }

            // Set the parameters
            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);
        }    
    }

    // Close the file!! important!!
    _in.close();

    // If no lines were read from the file (either file didn't exist 
    // or was empty) load all the channels as stereo channels.
    LOG(_linesLoaded);
    if (_linesLoaded == 0)
    {
        int i = 0;
        for (i = 0; i < m_AsioDevice.Device().info.maxInputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel<InputChannelPanel>();
            _c.AddChannel(&m_AsioDevice.Inputs()[i]);
            _c.AddChannel(&m_AsioDevice.Inputs()[i+1]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.SliderValue(0);
            _c.volume.SliderValue(1);
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == m_AsioDevice.Device().info.maxInputChannels - 1)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel<InputChannelPanel>();
            _c.AddChannel(&m_AsioDevice.Inputs()[i]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.SliderValue(0);
            _c.volume.SliderValue(1);
        }

        for (i = 0; i < m_AsioDevice.Device().info.maxOutputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel<OutputChannelPanel>();
            _c.AddChannel(&m_AsioDevice.Outputs()[i]);
            _c.AddChannel(&m_AsioDevice.Outputs()[i+1]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.SliderValue(0);
            _c.volume.SliderValue(1);
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == m_AsioDevice.Device().info.maxOutputChannels - 1)
        {
            // Add a ChannelPanel with all the inputs
            auto& _c = m_List->EmplaceChannel<OutputChannelPanel>();
            _c.AddChannel(&m_AsioDevice.Outputs()[i]);

            // Set all parameters of this Channel
            _c.mono.Active(false);
            _c.muted.Active(false);
            _c.pan.SliderValue(0);
            _c.volume.SliderValue(1);
        }
    }
    else
    {
        // If a couple channels were loaded then just load all the 
        // channels from the ASIO that weren't loaded from the file
        // as mono channels.
        for (auto& i : _inputIdsLoaded)
        {
            if (!i.second)
            {
                // Add a ChannelPanel with the input
                auto& _c = m_List->EmplaceChannel<InputChannelPanel>();
                _c.AddChannel(&m_AsioDevice.Inputs()[i.first]);
            }
        }

        for (auto& i : _outputIdsLoaded)
        {
            if (!i.second)
            {
                // Add a ChannelPanel with the input
                auto& _c = m_List->EmplaceChannel<OutputChannelPanel>();
                _c.AddChannel(&m_AsioDevice.Outputs()[i.first]);
            }
        }
    }
}