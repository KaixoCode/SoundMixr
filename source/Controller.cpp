#include "Controller.hpp"
#include "audio/Audio.hpp"
#include "ui/EndpointPanel.hpp"
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
    : mainWindow(m_Gui.AddWindow<Frame>("SoundMixr", 800, 400))
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

    _panel.Layout<Layout::Grid>(3, 1, 8, 8);
    _panel.Background(Theme::Get(Theme::WINDOW_BACKGROUND));

    auto& _p1 = _panel.Emplace<Panel>();
    _p1.Width(260);
    _p1.Layout<Layout::Border>(0);
    auto& _p11 = _p1.Emplace<Panel>(Layout::Hint::North);
    _p11.Layout<Layout::Grid>(1, 1, 0, 0);
    _p11.MinHeight(40);
    _p11.Height(40);
    _p11.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Inputs").Disable();
    auto& _p12 = _p1.Emplace<ListPanel<Type::Input>>(Layout::Hint::Center);
    auto& _p13 = _p12.Component<Panel>();
    _p13.Background(Color{ 40, 40, 40, 255 });
    _p13.Layout<Layout::Stack>(8);
    _p13.AutoResize(false, true);

    auto& _p2 = _panel.Emplace<Panel>();
    _p2.Width(260);
    _p2.Layout<Layout::Border>(0);
    auto& _p21 = _p2.Emplace<Panel>(Layout::Hint::North);
    _p21.Layout<Layout::Grid>(1, 1, 0, 0);
    _p21.MinHeight(40);
    _p21.Height(40);
    _p21.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Outputs").Disable();
    auto& _p22 = _p2.Emplace<ListPanel<Type::Output>>(Layout::Hint::Center);
    auto& _p23 = _p22.Component<Panel>();
    _p23.Background(Color{ 40, 40, 40, 255 });
    _p23.Layout<Layout::Stack>(8);
    _p23.AutoResize(false, true);

    Channel channel;

    std::vector<Audio*> _audios;

    std::thread([&] {
        for (auto& _d : _devices)
        {
            // Only inputs or outputs depending on type
            if (_d.inputChannels != 0)
            {
                //if (_d.id == 7)
                //{
                    auto& _ep = _p12.Component().Emplace<EndpointPanel>((bool)Type::Input);
                    _ep.SetDevice(Device{ _d });
                    _audios.push_back(&_ep.GetAudio());

                
                //    LOG("Input: " << _ep.GetAudio().Device().name);
                //    _ep.GetAudio().Connect(1, channel);
                //}
            }
            if (_d.outputChannels != 0)
            {
                //if (_d.id == 2)
                //{
                    auto& _ep = _p22.Component().Emplace<EndpointPanel>((bool)Type::Output);
                    _ep.SetDevice(Device{ _d });
                    _audios.push_back(&_ep.GetAudio());

                
                //    LOG("Output: " << _ep.GetAudio().Device().name);
                //    _ep.GetAudio().Connect(1, channel);
                //}
            }
        }
    }).detach();

    auto& _p3 = _panel.Emplace<Panel>();
    _p3.Width(260);
    _p3.Layout<Layout::Border>(0);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels").Disable();
    auto& _p32 = _p3.Emplace<ListPanel<Type::Channel>>(Layout::Hint::Center, &_audios);
    auto& _p33 = _p32.Component<Panel>();
    _p33.Background(Color{ 40, 40, 40, 255 });
    _p33.Layout<Layout::Stack>(8);
    _p33.AutoResize(false, true);

    auto& _file = _menu.Emplace<TitleMenuButton>("File", Vec2<int>{ 40, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&] { mainWindow.Close(); }, "Quit", Vec2<int>{ _width, _height }, Key::CTRL_Q);



    while (m_Gui.Loop());
}