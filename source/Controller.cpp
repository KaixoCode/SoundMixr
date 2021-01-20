#include "Controller.hpp"
#include "audio/Audio.hpp"
#include "ui/ChannelPanel.hpp"
#include "ui/ListPanel.hpp"

/*
 * Input devices
 *  - Option to add or remove devices
 *  - Select to which output devices
 * 
 * Output devices
 *  - Option to add or remove devices
 * 
 */



// -------------------------------------------------------------------------- \\
// ---------------------------- Controller ---------------------------------- \\
// -------------------------------------------------------------------------- \\

Controller::Controller()
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 528, 400, false))
{}

void Controller::Run()
{
    Theme::Load("..\\libs\\GuiCode\\themes\\dark");

    Audio _audio;
    auto& _devices = _audio.Devices();

    namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
    using MenuButton = Button<BG::Menu, BT::Normal>;
    using TitleMenuButton = Button<BG::TitleMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;

    auto& _panel = mainWindow.Panel();
    auto& _menu = mainWindow.Menu();

    _panel.Layout<Layout::SidewaysStack>(8);
    _panel.Background(Theme::Get(Theme::WINDOW_BACKGROUND));

    auto& _p2 = _panel.Emplace<Panel>();
    _p2.Width(260);
    _p2.Layout<Layout::Border>(0);
    auto& _p21 = _p2.Emplace<Panel>(Layout::Hint::North);
    _p21.Layout<Layout::Grid>(1, 1, 0, 0);
    _p21.Height(40);
    _p21.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Outputs").Disable();
    auto& _outputs = _p2.Emplace<ListPanel<Type::Output>>(Layout::Hint::Center);
    auto& _opanel = _outputs.Component<Panel>();
    _opanel.Background(Color{ 40, 40, 40, 255 });
    _opanel.Layout<Layout::Stack>(8);
    _opanel.AutoResize(false, true);

    auto& _p1 = _panel.Emplace<Panel>();
    _p1.Width(260);
    _p1.Layout<Layout::Border>(0);
    auto& _p11 = _p1.Emplace<Panel>(Layout::Hint::North);
    _p11.Layout<Layout::Grid>(1, 1, 0, 0);
    _p11.Height(40);
    _p11.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Inputs").Disable();
    auto& _inputs = _p1.Emplace<ListPanel<Type::Input>>(Layout::Hint::Center);
    auto& _ipanel = _inputs.Component<Panel>();
    _ipanel.Background(Color{ 40, 40, 40, 255 });
    _ipanel.Layout<Layout::Stack>(8);
    _ipanel.AutoResize(false, true);


    

    auto& _file = _menu.Emplace<TitleMenuButton>("File", Vec2<int>{ 40, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&] { LOG("TEST"); }, "Test", Vec2<int>{ _width, _height }, Key::CTRL_Q);
    _file.Emplace<MenuButton>([&] { mainWindow.Close(); }, "Quit", Vec2<int>{ _width, _height }, Key::CTRL_Q);



    while (m_Gui.Loop());
}