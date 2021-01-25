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
    _panel.Background(Theme::Get(Theme::WINDOW_BACKGROUND));

    auto& _p3 = _panel.Emplace<Panel>();
    _p3.Width(260);
    _p3.Layout<Layout::Border>(0, 0, false, false, false, false);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels").Disable();
    auto& _channelPanel = _p3.Emplace<ListPanel>(Layout::Hint::Center, m_SarAsio);
    m_List = &_channelPanel;
    auto& _p33 = _channelPanel.Component<Panel>();
    _p33.Background(Color{ 40, 40, 40, 245 });
    _p33.Layout<Layout::SidewaysStack>(8);
    _p33.AutoResize(true, false);

    auto& _file = _menu.Emplace<TitleMenuButton>("Options", Vec2<int>{ 54, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&]
        { 
            m_SarAsio.SaveRouting();
            m_SarAsio.CloseStream(); 
            PaAsio_ShowControlPanel(m_SarAsio.Device().id, mainWindow.GetWin32Handle()); 
            m_SarAsio.Inputs().clear();
            m_SarAsio.Outputs().clear();
            m_SarAsio.OpenStream();
            m_SarAsio.StartStream();
            _channelPanel.LoadChannels(); // Reload the channels to display any new ones
            LoadRouting();
        }, "SAR Control Panel", Vec2<int>{ _width, _height }, Key::CTRL_O);
    
    bool _aero = false;
    _file.Emplace<MenuToggleButton>(&_aero, "Windows Aero Effect", Vec2<int>{ _width, _height }, Key::CTRL_T);

    _channelPanel.LoadChannels();
    LoadRouting();
    int _saveCounter = 1000;
    while (m_Gui.Loop()) 
    {
        _p33.Background(Color{ 40, 40, 40, (_aero ? 245.0f : 255.0f) });
        _channelPanel.Transparency(_aero);
        mainWindow.Aero(_aero);

        _saveCounter--;
        if (_saveCounter <= 0)
        {
            _saveCounter = 60 * 60;
            m_SarAsio.SaveRouting();
        }
    }

    m_SarAsio.SaveRouting();
}

void Controller::LoadRouting()
{
    if (m_List == nullptr)
        return;

    LOG("Loading Routing");
    std::ifstream _in;
    _in.open("./routing");
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
            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);

            _c.InputChannel()->Connections().clear();
            auto& _out = m_SarAsio.Outputs();

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
            _c.mono.Active(_mono);
            _c.muted.Active(_muted);
            _c.pan.SliderValue(_pan);
            _c.volume.SliderValue(_volume);
        }
    }
}