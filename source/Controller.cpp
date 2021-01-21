#include "Controller.hpp"
#include "audio/Audio.hpp"
#include "ui/EndpointPanel.hpp"
#include "ui/ChannelPanel.hpp"
#include "ui/ListPanel.hpp"


// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 800, 400))
{}

void Controller::Run()
{
    Theme::Load("..\\libs\\GuiCode\\themes\\dark");


    std::vector<RtAudio::Api> apis;
    RtAudio::getCompiledApi(apis);
    for (auto& api : apis)
        LOG(RtAudio::getApiName(api));

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
    _p3.Layout<Layout::Border>(0);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels").Disable();
    auto& _p32 = _p3.Emplace<ChannelListPanel>(Layout::Hint::Center, m_AudioIO);
    auto& _p33 = _p32.Component<Panel>();
    _p33.Background(Color{ 40, 40, 40, 255 });
    _p33.Layout<Layout::Stack>(8);
    _p33.AutoResize(false, true);


    
    auto& _file = _menu.Emplace<TitleMenuButton>("File", Vec2<int>{ 40, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&] { mainWindow.Close(); }, "Quit", Vec2<int>{ _width, _height }, Key::CTRL_Q);

    std::thread([&] {
        m_AudioIO.LoadDevices();
        }).detach();
    while (m_Gui.Loop());
}