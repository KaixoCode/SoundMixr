#include "Controller.hpp"
#include "audio/Audio.hpp"
#include "ui/ChannelPanel.hpp"
#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 548, 350))
{}

void Controller::Run()
{

    namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
    using MenuButton = Button<BG::Menu, BT::Normal>;
    using TitleMenuButton = Button<BG::TitleMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;
    
    auto& _panel = mainWindow.Panel();
    auto& _menu = mainWindow.Menu();

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
    auto& _p33 = _channelPanel.Component<Panel>();
    _p33.Background(Color{ 40, 40, 40, 255 });
    _p33.Layout<Layout::SidewaysStack>(8);
    _p33.AutoResize(true, false);

    auto& _file = _menu.Emplace<TitleMenuButton>("Options", Vec2<int>{ 54, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&]
        { 
            m_SarAsio.CloseStream(); 
            PaAsio_ShowControlPanel(m_SarAsio.Device().id, mainWindow.GetWin32Handle()); 
            m_SarAsio.Inputs().clear();
            m_SarAsio.Outputs().clear();
            m_SarAsio.OpenStream();
            m_SarAsio.StartStream();
            _channelPanel.LoadChannels(); // Reload the channels to display any new ones
        }, "SAR Control Panel", Vec2<int>{ _width, _height }, Key::CTRL_O);

    _channelPanel.LoadChannels();


    while (m_Gui.Loop());
}