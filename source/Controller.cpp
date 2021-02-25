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
    LOG("Selecting default theme");
    std::ifstream _in;
    _in.open("./settings/theme");
    std::string _line;
    std::getline(_in, _line);
    int _themeId = std::atoi(_line.c_str());
    Themes::Theme = (Themes::N) _themeId;
    _in.close();

    namespace G = SoundMixrGraphics; namespace BT = ButtonType; namespace MT = MenuType;
    using MenuButton = Button<G::Menu, BT::Normal>;
    using MenuToggleButton = Button<G::Menu, BT::Toggle>;
    using TitleMenuButton = Button<G::TitleMenu, BT::Menu<G::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<G::SubMenu, BT::Menu<G::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

    mainWindow.Color(Theme<C::WindowBorder>::Get());
    soundboard.Color(Theme<C::WindowBorder>::Get());
    auto& _panel = mainWindow.Panel();
    auto& _menu = mainWindow.Menu();
    mainWindow.Icon(ASSET("textures/logo.png"));

    Menu<G::Vertical, MT::Normal> _closeMenu;
    _closeMenu.ButtonSize({ 150, 20 });
    _closeMenu.Emplace<MenuButton>([] {}, "SoundMixr").Disable();
    _closeMenu.Emplace<MenuAccessories::Divider>(150, 1, 0, 4);
    _closeMenu.Emplace<MenuButton>([&] { mainWindow.Show(); }, "Open GUI");
    _closeMenu.Emplace<MenuButton>([&] { m_Gui.Close(); }, "Exit");

    Frame::AddShellIcon("SoundMixr.ico", "SoundMixr", [&] (Event& e)
        { 
            if (e.button == Event::MouseButton::LEFT && e.mod)
                mainWindow.Show();

            if (e.button == Event::MouseButton::RIGHT)
                RightClickMenu::Get().Open(&_closeMenu);
        });

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
    auto& _channelPanel = _p3.Emplace<ListPanel>(Layout::Hint::Center, m_AsioDevice);
    _channelPanel.Background(Theme<C::MainPanel>::Get());
    m_List = &_channelPanel;
    auto& _p33 = _channelPanel.Panel();
    _p33.Background(Theme<C::MainPanel>::Get());
    _p33.Layout<Layout::SidewaysStack>(0);
    _p33.AutoResize(true, false);

    auto& _file = _menu.Emplace<TitleMenuButton>("Options");
    _file.Size({ 54, 32 });
    _file.MenuBase().ButtonSize({ 200, 20 });

    auto& _sub = _file.Emplace<SubMenuButton>("Select ASIO Device");
    _sub.MenuBase().ButtonSize({ 210, 20 });
    int _key = BT::List::NewKey();

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
        _titleButton.Name(_it->info.name);
        LoadRouting();
    }
    _in.close();


    for (auto& _d : m_AsioDevice.Devices())
    {
        auto& _button = _sub.Emplace<Button<G::Menu, BT::List>>([&]
            {
                if (&m_AsioDevice.Device() != nullptr && m_AsioDevice.Device().id == _d.id)
                    return;

                SaveRouting();
                _channelPanel.Clear();
                m_AsioDevice.CloseStream();
                m_AsioDevice.Device(_d);
                _titleButton.Name(_d.info.name);
                m_AsioDevice.OpenStream();
                m_AsioDevice.StartStream();
                std::ofstream _out;
                _out.open("./settings/device");
                _out << std::to_string(_d.id);
                _out.close();
                LoadRouting();
            }, _d.info.name, _key);

        if (_d.id == _deviceId)
            _button.Selected(true);
    }

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

    _file.Emplace<MenuToggleButton>([&](bool a)
        {
            _channelPanel.Transparency(a);
            mainWindow.Aero(a);
        }, "Windows Aero Effect", Key::CTRL_T);


    // Themes
    auto& _sub2 = _file.Emplace<SubMenuButton>("Theme");
    _sub2.MenuBase().ButtonSize({ 120, 20 });
    _key = BT::List::NewKey();
    auto& _theme1 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::DARK;
            _panel.Background(Theme<C::WindowBorder>::Get());
            mainWindow.Color(Theme<C::WindowBorder>::Get());
            soundboard.Color(Theme<C::WindowBorder>::Get());
            _p33.Background(Theme<C::MainPanel>::Get());
            _channelPanel.Background(Theme<C::MainPanel>::Get());

            std::ofstream _out;
            _out.open("./settings/theme");
            _out << std::to_string(Themes::Theme);
            _out.close();
        }, "Dark", _key);
    if (Themes::Theme == Themes::DARK) _theme1.Selected(true);
    auto& _theme2 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::LIGHT;
            _panel.Background(Theme<C::WindowBorder>::Get());
            mainWindow.Color(Theme<C::WindowBorder>::Get());
            soundboard.Color(Theme<C::WindowBorder>::Get());
            _p33.Background(Theme<C::MainPanel>::Get());
            _channelPanel.Background(Theme<C::MainPanel>::Get());

            std::ofstream _out;
            _out.open("./settings/theme");
            _out << std::to_string(Themes::Theme);
            _out.close();
        }, "Light", _key);
    if (Themes::Theme == Themes::LIGHT) _theme2.Selected(true);
    auto& _theme3 = _sub2.Emplace<Button<G::Menu, BT::List>>([&]
        {
            Themes::Theme = Themes::BLUE;
            _panel.Background(Theme<C::WindowBorder>::Get());
            mainWindow.Color(Theme<C::WindowBorder>::Get());
            soundboard.Color(Theme<C::WindowBorder>::Get());
            _p33.Background(Theme<C::MainPanel>::Get());
            _channelPanel.Background(Theme<C::MainPanel>::Get());

            std::ofstream _out;
            _out.open("./settings/theme");
            _out << std::to_string(Themes::Theme);
            _out.close();
        }, "Blue", _key);
    if (Themes::Theme == Themes::BLUE) _theme3.Selected(true);

    _file.Emplace<MenuButton>([&]
        {
            _channelPanel.ResetGrouping();
        }, "Reset Grouping");
    //_sub2.Emplace<Button<G::Menu, BT::List>>([&]
    //    {
    //        Themes::Theme = Themes::RED;
    //        _panel.Background(Theme<C::WindowBorder>::Get());
    //        mainWindow.Color(Theme<C::WindowBorder>::Get());
    //        soundboard.Color(Theme<C::WindowBorder>::Get());
    //        _p33.Background(Theme<C::MainPanel>::Get());
    //        _channelPanel.Background(Theme<C::MainPanel>::Get());
    //    }, "Red", _key);

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

    /*_file.Emplace<MenuToggleButton>([&] (bool s)
        {
            if (s) soundboard.Show(); else soundboard.Hide();
        }, "Soundboard", Key::CTRL_SHIFT_S);*/

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
        for (int i = 0; i < MAX_CHANNELS; i++)
            if (_i.Connections()[i] != nullptr)
                data += std::to_string(i) + ",";
        data += "\n";
    }

    std::ofstream _out;
    _out.open("./settings/routing" + std::to_string(m_AsioDevice.Device().id));
    _out << data;
    _out.close();
}

void Controller::LoadRouting()
{
    if (m_List == nullptr)
        return;

    LOG("Loading Routing");
    std::ifstream _in;
    _in.open("./settings/routing" + std::to_string(m_AsioDevice.Device().id));
    std::string _line;

    while (std::getline(_in, _line))
    {
        size_t p = _line.find_first_of(":") + 1;
        std::string _type = _line.substr(0, p - 1);
        std::string _rest = _line.substr(p);

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

        p = _rest.find_first_of(";");
        std::string _mutedS = _rest.substr(0, p);
        bool _muted = std::atoi(_mutedS.c_str());
        _rest = _rest.substr(p + 1);

        p = _rest.find_first_of(";");
        std::string _monoS = _rest.substr(0, p);
        bool _mono = std::atoi(_monoS.c_str());
        _rest = _rest.substr(p + 1);

        p = _rest.find_first_of(";");
        std::string _panS = _rest.substr(0, p);
        float _pan = std::atof(_panS.c_str());
        _rest = _rest.substr(p + 1);

        p = _rest.find_first_of(";");
        std::string _volumeS = _rest.substr(0, p);
        float _volume = std::atof(_volumeS.c_str());
        _rest = _rest.substr(p + 1);

        LOG("LOADING FROM FILE: \nids:");
        for (int i : _ids)
            LOG(i);
        LOG("muted: " << _muted);
        LOG("mono: " << _mono);
        LOG("pan: " << _pan);
        LOG("volume: " << _volume);

        if (_type == "in")
        {
            auto& _c = m_List->EmplaceChannel<InputChannelPanel>();
            for (int i : _ids)
                _c.AddChannel(&m_AsioDevice.Inputs()[i]);
            
            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);


            auto& _out = m_List->OutputChannels();
            while ((p = _rest.find_first_of(",")) != -1)
            {
                std::string _linkS = _rest.substr(0, p);
                int _link = std::atoi(_linkS.c_str());
                _rest = _rest.substr(p + 1);
                auto _it = std::find_if(_out.begin(), _out.end(), [&_link](OutputChannelPanel* obj) { LOG(obj->Channels().ID()); return obj->Channels().ID() == _link; });
                if (_it != _out.end())
                {
                    LOG(_c.Channels().ID() << " : " << _link);
                    auto _index = std::distance(_out.begin(), _it);
                    _c.Channels().Connect(&_out[_index]->Channels());
                }
            }

            //auto& _it = m_List->InputChannels().find(_id);
            //if (_it == m_List->InputChannels().end())
            //    continue;

            //auto& _c = *m_List->InputChannels()[_id];
            //if (&_c == nullptr)
            //    continue;

            //_c.mono.Active(_mono);
            //_c.muted.Active(_muted);
            //_c.pan.SliderValue(_pan);
            //_c.volume.SliderValue(_volume);

            //_c.Channels().Clear();
            //auto& _out = m_List->OutputChannels();

            //while ((p = _rest.find_first_of(",")) != -1)
            //{
            //    std::string _linkS = _rest.substr(0, p);
            //    int _link = std::atoi(_linkS.c_str());
            //    _rest = _rest.substr(p + 1);
                //auto _it = std::find_if(_out.begin(), _out.end(), [&_link](const OutputChannels& obj) {return obj.ID() == _link; });
                //if (_it != _out.end())
                //{
                    //auto _index = std::distance(_out.begin(), _it);
                    //_c.Channels().Connect(_out[_index].get());
               // }
            //}
        }
        else
        {
            auto& _c = m_List->EmplaceChannel<OutputChannelPanel>();
            for (int i : _ids)
                _c.AddChannel(&m_AsioDevice.Outputs()[i]);

            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);

            //auto& _it = m_List->OutputChannels().find(-_id - 2);
            //if (_it == m_List->OutputChannels().end())
            //    continue;
            
            //auto& _c = *m_List->OutputChannels()[-_id - 2];
            //if (&_c == nullptr)
            //    continue;

            //_c.mono.Active(_mono);
            //_c.muted.Active(_muted);
            //_c.pan.SliderValue(_pan);
            //_c.volume.SliderValue(_volume);
        }    
    }
    _in.close();
}