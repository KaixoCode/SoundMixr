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

    namespace BG = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
    using MenuButton = Button<BG::Menu, BT::Normal>;
    using TitleMenuButton = Button<BG::TitleMenu, BT::Menu<MG::Vertical, MT::Normal, BT::FocusToggle, Align::BOTTOM>>;
    using SubMenuButton = Button<BG::SubMenu, BT::Menu<MG::Vertical, MT::Normal, BT::Hover, Align::RIGHT>>;
    
    auto& _panel = mainWindow.Panel();
    auto& _menu = mainWindow.Menu();

    _panel.Layout<Layout::Grid>(1, 1, 8, 8);
    _panel.Background(Theme::Get(Theme::WINDOW_BACKGROUND));

    std::vector<AudioIO> _inputs, _outputs;
    std::vector<Channel> _channels;

    auto& _d = Devices();
    for (auto& _device : _d)
    {
        if (_device.inputChannels != 0)
            _inputs.emplace_back(_device, true);

        if (_device.outputChannels != 0)
            _outputs.emplace_back(_device, false);
    }

    auto& _p3 = _panel.Emplace<Panel>();
    _p3.Width(260);
    _p3.Layout<Layout::Border>(0);
    auto& _p31 = _p3.Emplace<Panel>(Layout::Hint::North);
    _p31.Layout<Layout::Grid>(1, 1, 0, 0);
    _p31.MinHeight(40);
    _p31.Height(40);
    _p31.Emplace<Button<TitleText, BT::Normal>>([]() {}, "Channels").Disable();
    auto& _p32 = _p3.Emplace<ListPanel<Type::Channel>>(Layout::Hint::Center);
    auto& _p33 = _p32.Component<Panel>();
    _p33.Background(Color{ 40, 40, 40, 255 });
    _p33.Layout<Layout::Stack>(8);
    _p33.AutoResize(false, true);


    int listid1 = BT::List::NewKey();
	int listid2 = BT::List::NewKey();

	int l = audios->size();
	for (int k = 0; k < l; k++)
	{
		auto& _audio = audios->at(k);
		Device& _d = _audio->Device();
		std::string name = _d.name;
		int i = name.find_first_of('(');
		if (i < name.size()) name.resize(i, ' ');

		// Only inputs or outputs depending on type
		if (_d.inputChannels != 0)
			m_IDeviceMenu.Emplace<Button<BG::Menu, BT::List>>([&] { _audio->Connect(m_Id, m_Channel); }, name, Vec2<int>{220, 20}, listid1);

		if (_d.outputChannels != 0)
			m_ODeviceMenu.Emplace<Button<BG::Menu, BT::List>>([&] { _audio->Connect(m_Id, m_Channel); }, name, Vec2<int>{220, 20}, listid2);
	}
    
    auto& _file = _menu.Emplace<TitleMenuButton>("File", Vec2<int>{ 40, 32 });
    int _height = 20, _width = 200;

    _file.Emplace<MenuButton>([&] { mainWindow.Close(); }, "Quit", Vec2<int>{ _width, _height }, Key::CTRL_Q);

    while (m_Gui.Loop());
}