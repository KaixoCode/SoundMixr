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



    namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
    using MenuButton = Button<BG::Menu, BT::Normal>;
    using MenuToggleButton = Button<BG::Menu, BT::Toggle>;
    using TitleMenuButton = Button<BG::TitleMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

    auto& _panel = mainWindow.Panel();
    auto& _menu = mainWindow.Menu();
    mainWindow.Icon(ASSET("textures/logo.png"));

    Menu<MG::Vertical, MT::Normal> _closeMenu;
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
    _panel.Background(Color{ 23, 23, 23, 255 });

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
    _channelPanel.Background(Color{ 40, 40, 40, 245 });
    m_List = &_channelPanel;
    auto& _p33 = _channelPanel.Panel<Panel>();
    _p33.Background(Color{ 40, 40, 40, 0 });
    _p33.Layout<Layout::SidewaysStack>(8);
    _p33.AutoResize(true, false);

    auto& _file = _menu.Emplace<TitleMenuButton>("Options");
    _file.Size({ 54, 32 });
    _file.MenuBase().ButtonSize({ 200, 20 });

    auto& _sub = _file.Emplace<SubMenuButton>("Select ASIO Device");
    _sub.MenuBase().ButtonSize({ 210, 20 });
    int _key = BT::List::NewKey();

    LOG("Selecting default device");
    std::ifstream _in;
    _in.open("./settings/device");
    std::string _line;
    std::getline(_in, _line);
    int _deviceId = std::atoi(_line.c_str());
    if (!_line.empty())
    {
        auto _it = std::find_if(m_AsioDevice.Devices().begin(), m_AsioDevice.Devices().end(), [=](const Device& obj) {return obj.id == _deviceId; });
        m_AsioDevice.Device(*_it);
        m_AsioDevice.OpenStream();
        m_AsioDevice.StartStream();
        _titleButton.Name(_it->info.name);
        _channelPanel.LoadChannels();
        LoadRouting();
    }

    for (auto& _d : m_AsioDevice.Devices())
    {
        auto& _button = _sub.Emplace<Button<BG::Menu, BT::List>>([&]
            {
                if (&m_AsioDevice.Device() != nullptr && m_AsioDevice.Device().id == _d.id)
                    return;

                m_AsioDevice.SaveRouting();
                m_AsioDevice.CloseStream();
                m_AsioDevice.Inputs().clear();
                m_AsioDevice.Outputs().clear();
                m_AsioDevice.Device(_d);
                _titleButton.Name(_d.info.name);
                m_AsioDevice.OpenStream();
                m_AsioDevice.StartStream();
                std::ofstream _out;
                _out.open("./settings/device");
                _out << std::to_string(_d.id);
                _out.close();
                _channelPanel.LoadChannels(); // Reload the channels to display any new ones
                LoadRouting();
            }, _d.info.name, _key);

        if (_d.id == _deviceId)
            _button.Selected(true);
    }

    _file.Emplace<MenuButton>([&]
        {
            if (!&m_AsioDevice.Device())
                return;

            m_AsioDevice.SaveRouting();
            m_AsioDevice.CloseStream();
            PaAsio_ShowControlPanel(m_AsioDevice.Device().id, mainWindow.GetWin32Handle());
            m_AsioDevice.Inputs().clear();
            m_AsioDevice.Outputs().clear();
            m_AsioDevice.OpenStream();
            m_AsioDevice.StartStream();
            _channelPanel.LoadChannels(); // Reload the channels to display any new ones
            LoadRouting();
        }, "ASIO Control Panel", Key::CTRL_O);

    _file.Emplace<MenuToggleButton>([&](bool a)
        {
            _channelPanel.Transparency(a);
            mainWindow.Aero(a);
        }, "Windows Aero Effect", Key::CTRL_T);

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
            m_AsioDevice.SaveRouting();
        }
    }

    m_AsioDevice.SaveRouting();
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
        std::string _idS = _rest.substr(0, p);
        int _id = std::atoi(_idS.c_str());
        _rest = _rest.substr(p + 1);

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

        if (_type == "in")
        {
            auto& _c = *m_List->Channels()[_id];
            if (&_c == nullptr)
                continue;

            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);

            _c.InputChannel()->Clear();
            auto& _out = m_AsioDevice.Outputs();

            while ((p = _rest.find_first_of(",")) != -1)
            {
                std::string _linkS = _rest.substr(0, p);
                int _link = std::atoi(_linkS.c_str());
                _rest = _rest.substr(p + 1);
                auto _it = std::find_if(_out.begin(), _out.end(), [&_link](const StereoOutputChannel& obj) {return obj.ID() == _link; });
                if (_it != _out.end())
                {
                    auto _index = std::distance(_out.begin(), _it);
                    _c.InputChannel()->Connect(&_out[_index]);
                }
            }
        }
        else
        {
            auto& _c = *m_List->Channels()[-_id - 2];
            if (&_c == nullptr)
                continue;

            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);
        }
    }
}