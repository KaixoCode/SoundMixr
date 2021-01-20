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
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 800, 600))
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

    _panel.Layout<Layout::Grid>(2, 1, 8, 8);
    _panel.Background(Theme::Get(Theme::WINDOW_BACKGROUND));

    auto& _inputs = _panel.Emplace<ListPanel<true>>();
    auto& _outputs = _panel.Emplace<ListPanel<false>>();
    auto& _ipanel = _inputs.Component<Panel>();
    _ipanel.Background(Color{ 40, 40, 40, 255 });
    _ipanel.Layout<Layout::Stack>(8);
    _ipanel.AutoResize(false, true);
    auto& _b1 = _ipanel.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Inputs", Vec2<int>{100, 40});
    _b1.Disable();
    auto& _opanel = _outputs.Component<Panel>();
    _opanel.Background(Color{ 40, 40, 40, 255 });
    _opanel.Layout<Layout::Stack>(8);
    _opanel.AutoResize(false, true);
    auto& _b2 = _opanel.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Outputs", Vec2<int>{100, 40});
    _b2.Disable();

    auto& _file = _menu.Emplace<TitleMenuButton>("File", Vec2<int>{ 40, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&] { LOG("TEST"); }, "Test", Vec2<int>{ _width, _height }, Key::CTRL_Q);
    _file.Emplace<MenuButton>([&] { mainWindow.Close(); }, "Quit", Vec2<int>{ _width, _height }, Key::CTRL_Q);



    while (m_Gui.Loop());
}