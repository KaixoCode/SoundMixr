#include "Controller.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 728, 500))
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
    LOG(ASSET(textures/logo.png));
    mainWindow.Icon(ASSET(textures/logo.png));

    _panel.Layout<Layout::Grid>(1, 1, 8, 8);
    _panel.Background(Color{ 23, 23, 23, 255 });

    auto& _p3 = _panel.Emplace<Panel>();
    _p3.Width(260);
    _p3.Layout<Layout::Border>(0, 0, false, false, false, false);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels").Disable();
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
    for (auto& _d : m_AsioDevice.Devices())
    {
        _sub.Emplace<Button<BG::Menu, BT::List>>([&] 
            {
                if (&m_AsioDevice.Device() != nullptr && m_AsioDevice.Device().id == _d.id)
                    return;

                m_AsioDevice.SaveRouting();
                m_AsioDevice.CloseStream();
                m_AsioDevice.Inputs().clear();
                m_AsioDevice.Outputs().clear();
                m_AsioDevice.Device(_d);
                m_AsioDevice.OpenStream();
                m_AsioDevice.StartStream();
                _channelPanel.LoadChannels(); // Reload the channels to display any new ones
                LoadRouting();
            }, _d.info.name, _key);
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

    
    bool _aero = false, _vertical = false;;
    _file.Emplace<MenuToggleButton>(&_aero, "Windows Aero Effect", Key::CTRL_T);

    //_file.Emplace<MenuToggleButton>(&_vertical, "Vertical UI", Vec2<int>{ _width, _height }, Key::CTRL_L);

    int _saveCounter = 1000;
    bool _von = false, _var = false;
    while (m_Gui.Loop()) 
    {
        _p33.Background(Color{ 40, 40, 40, (_aero ? 245.0f : 255.0f) });
        
        if ((_var && !_aero) || (!_var && _aero))
        {
            _var = _aero;
            _channelPanel.Transparency(_aero);
            mainWindow.Aero(_aero);
        }
        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 60 * 60;
            m_AsioDevice.SaveRouting();
        }

        if (_vertical && !_von)
        {
            _von = true;
            _p33.Layout<Layout::Stack>(8);
            _p33.AutoResize(false, true);
            _channelPanel.Vertical();
        }
        else if (!_vertical && _von)
        {
            _von = false;
            _p33.Layout<Layout::SidewaysStack>(8);
            _p33.AutoResize(true, false);
            _channelPanel.Horizontal();
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
    _in.open("./routing" + std::to_string(m_AsioDevice.Device().id));
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

            _c.InputChannel()->Connections().clear();
            auto& _out = m_AsioDevice.Outputs();

            while ((p = _rest.find_first_of(",")) != -1)
            {
                std::string _linkS = _rest.substr(0, p);
                int _link = std::atoi(_linkS.c_str());
                _rest = _rest.substr(p + 1);
                auto _it = std::find_if(_out.begin(), _out.end(), [&_link](const StereoOutputChannel& obj) {return obj.ID() == _link;});
                if (_it != _out.end())
                {
                    auto _index = std::distance(_out.begin(), _it);
                    _c.InputChannel()->Connections().emplace(_link, &_out[_index]);
                }
            }
        }
        else
        {
            auto& _c = *m_List->Channels()[- _id - 2];
            if (&_c == nullptr)
                continue;

            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);
        }
    }
}