#include "audio/Audio.hpp"
#include "audio/EndpointChannel.hpp"
#include "audio/GeneratorChannel.hpp"
#include "audio/SoundboardChannel.hpp"
#include "audio/ForwardChannel.hpp"

Audio::Audio()
    :
    // This panel contains the channel scrollpanel in the center and the 
    // device name at the top. The effect panel will be displayed in the east.
    m_ChannelScrollPanel(Emplace<SMXRScrollPanel>(Layout::Hint::Center)),
    m_DeviceName(Emplace<SMXRTextComponent>(Layout::Hint::North, "Channels", 24.0f)),
    m_EffectPanel(Emplace<EffectPanel>(Layout::Hint::East)),

    // The panel in the channel scrollpanel contains the input panel and output panel
    // also add a divider inbetween. The order off addition is important! First added will
    // be on the left of the panel.
    m_InputsPanel(m_ChannelScrollPanel.Panel<Panel>().Emplace<ChannelPanel>()),
    m_Divider(m_ChannelScrollPanel.Panel().Emplace<VerticalMenuDivider>(1, 2, 4, 0)),
    m_OutputsPanel(m_ChannelScrollPanel.Panel().Emplace<ChannelPanel>())
{
    // This panel layout is border with no padding/resizing.
    Layout<Layout::Border>(0, false);

    // Device name, which is displayed at the top, has height of 40, also set minheight 
    // to make sure resizing the window doesn't resize the panel to something smaller.
    // also align the text to the center of the TextDisplayer.
    m_DeviceName.MinHeight(40);
    m_DeviceName.Height(40);
    m_DeviceName.AlignLines(Align::CENTER);

    // The main scrollpanel for all the channels, only scrolls in x-axis, means that y-axis will
    // autoresize all components to be Height().
    m_ChannelScrollPanel.EnableScrollbars(true, false);

    // The panel in the channel scrollpanel has sideways stack layout with no padding, this will
    // layout the inputs/outputs panel side by side with the divider inbetween. Also autoresize
    // this panel in the x-axis to make sure it fits all channels.
    m_ChannelScrollPanel.Panel().Layout<Layout::SidewaysStack>(0, 0);
    m_ChannelScrollPanel.Panel().AutoResize(true, false);

    // The inputs/outputs pannel once again has sideways stack layout, now with 8 pixels padding
    // which will leave some space inbetween each channel, also resize in x-axis to make sure]
    // the panels are big enough to fit all channels.
    m_InputsPanel.Layout<Layout::SidewaysStack>(8, 8);
    m_InputsPanel.AutoResize(true, false);
    m_OutputsPanel.Layout<Layout::SidewaysStack>(8, 8);
    m_OutputsPanel.AutoResize(true, false);

    // Initially hide the effect panel, will be shown when double click on channel.
    m_EffectPanel.Hide();

    /**
    * Add unfocused event listener to channel scrollpanel and set selected channel to nullptr
    * to unselect channel if clicked outside of channel area.
    */
    m_ChannelScrollPanel.Listener() += [this](Event::Unfocused& e)
    {
        ChannelBase::selected = nullptr;
    };

    m_ChannelScrollPanel.Listener() += [this](Event::MousePressed& e)
    {
        // if the effects panel is visible and pressed on channel, view the
        // selected channel's effect chain.
        if (e.button == Event::MouseButton::LEFT && ChannelBase::selected)
        {
            bool vis = m_EffectPanel.Visible();
            m_EffectPanel.EffectChain(&ChannelBase::selected->EffectChain());
            m_EffectPanel.Name(ChannelBase::selected->name.Content());
            m_EffectPanel.Visible(vis);
        }

        // If rightclick, check if hovering over a channel and show a rightclick
        if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
        {
            if (m_InputsPanel.HoveringComponent())
                GenerateMenu(m_InputsPanel), RightClickMenu::Get().Open(&m_Menu);
            else if (m_OutputsPanel.HoveringComponent())
                GenerateMenu(m_OutputsPanel), RightClickMenu::Get().Open(&m_Menu);
            else
            {
                m_Menu.Clear();
                m_Menu.ButtonSize({ 180, 20 });
                m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>("Channels").Disable();
                m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);

                // Sub-menu to show/hide input channels
                auto& _sub = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>
                    ("Show Inputs");
                _sub.MenuBase().ButtonSize({ 180, 20 });
                for (auto& _c : m_InputsPanel.Components())
                {
                    ChannelBase* s = dynamic_cast<ChannelBase*>(_c.get());
                    if (s)
                        _sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&s->m_Visible, s->name.Content());
                }

                // Sub-menu to show/hide output channels
                auto& _sub2 = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>
                    ("Show Outputs");
                _sub2.MenuBase().ButtonSize({ 180, 20 });
                for (auto& _c : m_OutputsPanel.Components())
                {
                    ChannelBase* s = dynamic_cast<ChannelBase*>(_c.get());
                    if (s)
                        _sub2.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&s->m_Visible, s->name.Content());
                }


                //m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
                //auto& _sub3 = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>
                //    ("Add Generator");
                //_sub3.MenuBase().ButtonSize({ 180, 20 });
                //for (auto& _c : PluginLoader::Generators())
                //{
                //    _sub3.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] {
                //        m_Lock.lock();
                //        EmplaceChannel<GeneratorChannel>(_c.second->CreateInstance());
                //        m_Lock.unlock();
                //        }, _c.first);
                //}

                RightClickMenu::Get().Open(&m_Menu);
            }
        }
    };

    // If double click, show effect panel.
    m_ChannelScrollPanel.Listener() += [this](Event::MouseClicked& e)
    {
        // Only if mousebutton is left, so return for other
        if (e.button != Event::MouseButton::LEFT)
            return;

        // If click twice in same position, open effectpanel.
        if (m_Click > 0 && m_PrevPos == Vec2<int>{ e.x, e.y } && ChannelBase::selected && ChannelBase::selected->Hovering())
        {
            // Also make sure we're not hovering over the volume or pan parameter, because
            // double clicking on those will reset their value and should not open the effect panel.
            if (ChannelBase::selected->Hovering())
            {
                m_EffectPanel.EffectChain(&ChannelBase::selected->EffectChain());
                m_EffectPanel.Name(ChannelBase::selected->name.Content());
            }
        }
        m_PrevPos = { e.x, e.y };
        m_Click = 20;
    };
}

void Audio::GenerateMenu(Panel& panel)
{
    // First get pointers to both the hovering and the focused channel.
    auto hover = (ChannelBase*)panel.HoveringComponent();
    auto focus = ChannelBase::selected;

    // Get any subchannel hover, but store original hover as well.
    auto orighover = hover;
    if (hover->Type() & ChannelBase::Type::Endpoint)
    {
        auto hovera = (EndpointChannel*)hover;
        if (hovera->HoveringComponent() == &hovera->m_SubChannels)
        {
           ChannelBase* s = (ChannelBase*)hovera->m_SubChannels.HoveringComponent();
           if (s)
               hover = s;
        }
    }

    m_Menu.Clear();
    m_Menu.ButtonSize({ 180, 20 });
    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(hover->name.Content()).Disable();
    m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
        [&, hover] {
            m_EffectPanel.EffectChain(&hover->EffectChain());
            m_EffectPanel.Name(hover->name.Content());
        }, "Show Effects");
    if (!(hover->Type() & ChannelBase::Type::Forward))
        m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
            [&, hover] {
                hover->Visible(false);
            }, "Hide Channel");

    // A channel can split if it has more than 1 line, and is an endpoint
    bool canSplit = (hover->Type() & ChannelBase::Type::Endpoint) && hover->Lines() > 1;

    // Channels can combine if they are the same type, and not the same channel.
    // and an endpoint.
    bool canCombine = (hover->Type() & ChannelBase::Type::Endpoint) && focus && 
        focus->Type() == hover->Type() && focus != hover;

    bool canAdd = (hover->Type() & ChannelBase::Type::Generator);

    // An endpoint channel can be virtualized, but choosing its respective WASAPI device.
    bool canVirtual = (hover->Type() & ChannelBase::Type::Endpoint);

    // A forward channel.
    bool canForward = (hover->Type() & ChannelBase::Type::Forward);

    // Add divider if any of the 2 are possible
    if (canSplit || canCombine || canAdd || canVirtual || canForward)
        m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);

    // If it's a forward channel
    if (canForward)
    {
        auto hovere = (ForwardChannel*)hover;
        auto orighovere = (EndpointChannel*)orighover;
        // First add a delete option
        m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&, hovere, orighovere]() 
            {   
                // if it was selected, set to nullptr and remove effect chain
                if (ChannelBase::selected == hovere)
                {
                    ChannelBase::selected = nullptr;
                    m_EffectPanel.EffectChain(nullptr);
                }
                // Erase it from the subchannels of the original hovering.
                orighovere->m_SubChannels.Erase(*(Component*)hovere);
                auto& a = orighovere->m_SubChannels.m_Channels;
                a.erase(std::find(a.begin(), a.end(), hovere));
            }, "Delete");

        // Sub menu for selecting device to forward from/to.
        auto& _sub = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<
            SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>("Forward");
        _sub.MenuBase().ButtonSize({ 220, 20 });
        
        // Generate key for button List
        int _key = ButtonType::List::NewKey();

        // None option, to close device.
        auto& _nb = _sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([&, hovere]() {
            hovere->CloseStream();
            hovere->m_PhysicalDevice = -1;
            }, "None", _key);

        // Also auto select this one if it's the selected device.
        if (hovere->m_PhysicalDevice == -1)
            _nb.Selected(true);

        // For each device
        for (int i = 0; i < hovere->m_Audio.DeviceCount(); i++)
        {
            auto& _info = hovere->m_Audio.Device(i);

            // If this device is not input/output depending on channeltype, continue.
            if ((_info.outputChannels > 0 && (orighovere->m_Type & ChannelBase::Type::Input))
                || _info.inputChannels > 0 && (orighovere->m_Type & ChannelBase::Type::Output))
                continue;

            // Otherwise add a button to select the device.
            auto& _button = _sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([&, i, hovere]() {
                hovere->m_PhysicalDevice = i;
                hovere->OpenStream();
                }, _info.name, _key);
            // And select button if it's the currently selected device.
            if (hovere->m_PhysicalDevice == i)
                _button.Selected(true);
        }
    }

    // If the channel can become a virtual channel
    if (canVirtual)
    {
        auto hovere = (EndpointChannel*)hover;
        bool isVirtual = hovere->m_SubChannels.m_VirtualChannel != -1;

        // Option to add a new forward channel if the channel has been made virtual
        if (isVirtual)
            m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([hovere]() 
                {
                    auto& a = hovere->m_SubChannels.EmplaceChannel<ForwardChannel>(hovere->m_SubChannels.m_Type); 
                    a.m_VirtualDevice = hovere->m_SubChannels.m_VirtualChannel;
                }, "Add Forward Channel");

        // Submenu for selecting a WASAPI device.
        auto& _sub = m_Menu.Emplace<Button<SoundMixrGraphics::SubMenu, ButtonType::Menu<
            SoundMixrGraphics::Vertical, MenuType::Normal, ButtonType::Hover, Align::RIGHT>>>("Set Forward Device");
        _sub.MenuBase().ButtonSize({ 220, 20 });
        int _key = ButtonType::List::NewKey();
        
        // None, for non-virtualized channels.
        auto& _nb = _sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([&, hovere]() 
            {
                hovere->m_SubChannels.m_VirtualChannel = -1;
                try
                {
                    // Close all the streams.
                    for (auto& i : hovere->m_SubChannels.m_Channels)
                    {
                        ((ForwardChannel*)i)->m_VirtualDevice = -1;
                        ((ForwardChannel*)i)->CloseStream();
                    }
                }
                catch (...)
                {
                    CrashLog("Failed to close virtual device");
                }
                // Hide and width = 0, and force redraw of Layoutmanager.
                hovere->m_SubChannels.Hide();
                hovere->m_SubChannels.Width(0);
                hovere->LayoutManager().Refresh();
            }, "None", _key);

        // Select if current device
        if (hovere->m_SubChannels.m_VirtualChannel == -1)
            _nb.Selected(true);

        // Go through devices.
        int biggestSize = 0;
        for (int i = 0; i < hovere->m_Physical.DeviceCount(); i++)
        {
            auto _info = hovere->m_Physical.Device(i);

            // If this device is not input/output depending on channeltype, continue.
            if ((_info.inputChannels > 0 && (hovere->m_Type & ChannelBase::Type::Input)) 
                || _info.outputChannels > 0 && (hovere->m_Type & ChannelBase::Type::Output))
                continue;

            // Add button to select this device.
            auto& _button = _sub.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::List>>([&, i, hovere]() 
                {
                    // Set the virtual channel, and add a forward channel if none is present.
                    hovere->m_SubChannels.m_VirtualChannel = i;
                    if (hovere->m_SubChannels.m_Channels.size() == 0)
                        hovere->m_SubChannels.EmplaceChannel<ForwardChannel>(hovere->m_SubChannels.m_Type);
                    try
                    {
                        // Also set the virtual device for any existing channels, and open if possible.
                        for (auto& a : hovere->m_SubChannels.m_Channels)
                        {
                            ((ForwardChannel*)a)->m_VirtualDevice = i;
                            ((ForwardChannel*)a)->OpenStream();
                        }
                    }
                    catch (...)
                    {
                        CrashLog("Failed to open virtual device: " << i);
                    }

                    // Show Subchannels
                    hovere->m_SubChannels.Show();
                }, _info.name, _key);

            // Select if it's currently selected device.
            if (hovere->m_SubChannels.m_VirtualChannel == i)
                _button.Selected(true);

            int textWidth = Graphics::StringWidth(_info.name, ThemeT::Get().font, 16);
            if (textWidth > biggestSize)
                biggestSize = textWidth;
        }
        // TODO: implement dynamic buttonSize feature.
        _sub.MenuBase().ButtonSize({ biggestSize + 40, 20 });
    }

    if ((canCombine || canSplit) && (canVirtual || canForward))
        m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);

    // If can combine, add combine button
    if (canCombine)
    {
        m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
            [&, hover, focus] {

                // Lock for good measure
                m_Lock.lock();

                // Cast focus to endpoint channel
                auto focuse = (EndpointChannel*)focus;
                auto hovere = (EndpointChannel*)hover;

                // Move over all endpoints from hover to focus
                for (auto& endpoint : hovere->Endpoints())
                    focuse->AddEndpoint(endpoint);

                // Make sure the effect panel is not displaying the effect chain
                // because we are going to remove 'hover'
                if (m_EffectPanel.EffectChain() == &hover->EffectChain())
                    m_EffectPanel.EffectChain(nullptr);

                // If output is removed, remove connection from all inputs
                if (hover->Type() & ChannelBase::Type::Output)
                    for (auto& _c : m_Channels)
                        if (_c->Type() & ChannelBase::Type::Input)
                        {
                            _c->m_Lock.lock();
                            auto _it = std::remove(_c->Connections().begin(), _c->Connections().end(), hover);
                            if (_it != _c->Connections().end())
                                _c->Connections().erase(_it);
                            _c->m_Lock.unlock();
                        }
                // Remove the hovering panel from both the panel and the channels.
                auto it2 = std::remove(m_Channels.begin(), m_Channels.end(), hover);
                m_Channels.erase(it2);
                auto it = std::remove_if(panel.Components().begin(), panel.Components().end(),
                    [hover](std::unique_ptr<Component>& u) { return u.get() == hover; });
                panel.Erase(it);
                m_Lock.unlock();
            }, "Combine");
    }

    // if can split, add split button
    if (canSplit)
        m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
            [&, hover] {

                // Lock for good measure
                m_Lock.lock();

                // Add new channel
                auto& a = panel.Emplace<EndpointChannel>(hover->Type());
                m_Channels.push_back(&a);

                // Cast focus to endpoint channel
                auto hovere = (EndpointChannel*)hover;

                // move endpoints to new channel
                int size = hover->Lines() / 2;
                for (int i = 0; i < size; i++)
                {
                    auto endpoint = hovere->Endpoints()[0];
                    hovere->RemoveEndpoint(endpoint);
                    a.AddEndpoint(endpoint);
                }
                m_Lock.unlock();

                // Sort the channels
                SortChannels();
            }, "Split");

    m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
        [hover](bool s) {
            hover->mute.Active(s);
        }, "Mute").Active(hover->mute.Active());

    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(
        [hover](bool s) {
            hover->mono.Active(s);
        }, "Mono").Active(hover->mono.Active());
}

bool Audio::OpenDevice(int id)
{
    // Make sure the effect chain being displayed is set to nullptr
    // since it will otherwise be undefined.
    m_EffectPanel.EffectChain(nullptr);

    // Close the stream in case there was one open.
    m_Asio.CloseStream();

    // Set the device and open the stream
    m_Asio.Callback(AsioCallback);
    m_Asio.UserData(*this);
    if (!m_Asio.OpenStream({ .input = id, .output = id }))
        return false;

    // Start stream
    LoadRouting();
    if (!m_Asio.StartStream())
    {
        m_Asio.CloseStream();
        return false;
    }

    // Set device name
    m_DeviceName.Content(m_Asio.Device(id).name);
    return true;
}

bool Audio::CloseDevice()
{
    // Make sure the effect chain being displayed is set to nullptr
    // since it will otherwise be undefined.
    m_EffectPanel.EffectChain(nullptr);

    // Open any sub-channels
    for (auto& i : m_Channels)
        if (i->Type() & ChannelBase::Type::Endpoint)
            for (auto& k : ((EndpointChannel*)i)->m_SubChannels.m_Channels)
                ((ForwardChannel*)k)->CloseStream();

    // Close the stream in case there was one open.
    m_Asio.CloseStream();
    SaveRouting();

    // Clear the panel.
    Clear();
    m_DeviceName.Content("No Device");
    return true;
}

void Audio::ShowControlPanel()
{
    m_Asio.OpenControlPanel();
}

void Audio::Clear()
{
    // Select nullptr, also for effect chain
    ChannelBase::selected = nullptr;
    m_EffectPanel.EffectChain(nullptr);

    // Clear all channels
    m_Channels.clear();
    m_InputsPanel.Clear();
    m_OutputsPanel.Clear();

    //m_GeneratorPanel.Clear();
}

void Audio::LoadRouting()
{
    // First make sure everything is cleared.
    Clear();

    // Keep track which channels have been loaded from the file so
    // we can later add the ones that weren't added separately.
    bool _soundBoardLoaded = false;
    std::unordered_map<int, bool> _inputIdsLoaded, _outputIdsLoaded;
    for (int i = 0; i < m_Asio.Information().inputChannels; i++)
        _inputIdsLoaded.emplace(i, false);

    for (int i = 0; i < m_Asio.Information().outputChannels; i++)
        _outputIdsLoaded.emplace(i, false);

    // Open the routing file for the current device.
    CrashLog("Loading Routing");
    std::ifstream _in;
    _in.open("./settings/routing" + std::to_string(m_Asio.Information().input));

    // Start the loading process if the file opened successfully
    // otherwise we got an error!
    bool _error = true;
    if (!_in.fail())
    {
        _error = false;
        try
        {
            // Parse the json.
            nlohmann::json _json;
            _in >> _json;

            // Get the channels from the json
            auto _channels = _json.at("channels");

            // First load all the output channels
            for (auto& i : _channels)
            {
                // Skip input channels
                int _type = i["type"].get<int>();
                if (_type & ChannelBase::Type::Input)
                    continue;

                ChannelBase* _base = nullptr;

                // If type is Endpoint, add endpoint to outputspanel
                if (_type & ChannelBase::Type::Endpoint)
                {
                    auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(_type);
                    _base = &_c;
                    m_Channels.push_back(&_c);

                    // Add all channels that are in this channelgroup
                    nlohmann::json _channels = i.at("channels");
                    for (int i : _channels)
                    {
                        if (i >= _outputIdsLoaded.size())
                            break;
                        _outputIdsLoaded[i] = true;
                        _c.AddEndpoint(&m_Asio.Outputs()[i]);
                    }
                }

                // If no compatible type was found, continue.
                if (!_base)
                    continue;

                // Send json to the channel to load channel settings.
                *_base = i;
            }

            // Then go through all input channels
            for (auto& i : _channels)
            {
                // Skip output channels
                int _type = i["type"].get<int>();
                if (_type & ChannelBase::Type::Output)
                    continue;

                ChannelBase* _base = nullptr;

                // If type is Endpoint, add endpoint channel to inputspanel
                if (_type & ChannelBase::Type::Endpoint)
                {
                    auto& _c = m_InputsPanel.Emplace<EndpointChannel>(_type);
                    _base = &_c;
                    m_Channels.push_back(&_c);

                    // Add all channels that are in this channelgroup
                    nlohmann::json _channels = i.at("channels");
                    for (int i : _channels)
                    {
                        if (i >= _inputIdsLoaded.size())
                            break;

                        _inputIdsLoaded[i] = true;
                        _c.AddEndpoint(&m_Asio.Inputs()[i]);
                    }
                }

                // If type is soundboard, add soundboard channel to inputspanel
                else if (_type & ChannelBase::Type::SoundBoard)
                {
                    _soundBoardLoaded = true;
                    auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
                    _base = &_c;
                    m_Channels.push_back(&_c);
                }

                // If no compatible type was found, continue.
                if (!_base)
                    continue;

                // Then add all the connections to the channel
                nlohmann::json _connections = i.at("connections");
                for (int i : _connections)
                {
                    // Find the outputchannel using its id.
                    auto _it = std::find_if(Channels().begin(), Channels().end(),
                        [=](ChannelBase* c)
                        {
                            bool output = (int)c->Type() & (int)ChannelBase::Type::Output;
                            return output && c->Id() == i;
                        }
                    );

                    // If it exists, connect with it.
                    if (_it != Channels().end())
                        _base->Connect(*_it);
                }

                // Send json to the channel to load channel settings.
                *_base = i;
            }
        }

        // If error occured (either file didn't exist or was parced incorrectly
        // load all the channels as stereo channels.
        catch (std::exception e)
        {
            CrashLog("Error loading routing: " << e.what());
            _error = true;
        }
        catch (...)
        {
            CrashLog("Error loading routing");
        }
    }

    // If error, just use default routing and return
    if (_error)
    {
        DefaultRouting();
        _in.close();// Close the file!! important!!
        return;
    }

    _in.close();// Close the file!! important!!

    // If a couple channels were loaded then just load all the 
    // channels from the ASIO that weren't loaded from the file
    // as mono channels.
    for (auto& i : _inputIdsLoaded)
    {
        if (!i.second)
        {
            // Add a ChannelPanel with the input
            auto& _c = m_InputsPanel.Emplace<EndpointChannel>(
                ChannelBase::Type::Input | ChannelBase::Type::Endpoint);
            _c.AddEndpoint(&m_Asio.Inputs()[i.first]);
            m_Channels.push_back(&_c);
        }
    }

    for (auto& i : _outputIdsLoaded)
    {
        if (!i.second)
        {
            // Add a ChannelPanel with the output
            auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(
                ChannelBase::Type::Output | ChannelBase::Type::Endpoint);
            _c.AddEndpoint(&m_Asio.Outputs()[i.first]);
            m_Channels.push_back(&_c);
        }
    }

    if (!_soundBoardLoaded)
    {
        auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
        m_Channels.push_back(&_c);
    }
}

void Audio::DefaultRouting()
{
    Clear();

    // Soundboard channel
    auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
    m_Channels.push_back(&_c);

    // Endpoint channels
    int i = 0;
    for (i = 0; i < m_Asio.Information().inputChannels - 1; i += 2)
    {
        // Add a ChannelPanel with all the inputs
        auto& _c = m_InputsPanel.Emplace<EndpointChannel>(
            ChannelBase::Type::Input | ChannelBase::Type::Endpoint);
        _c.AddEndpoint(&m_Asio.Inputs()[i]);
        _c.AddEndpoint(&m_Asio.Inputs()[i + 1]);
        m_Channels.push_back(&_c);

        // Set all parameters of this Channel
        _c.mono.Active(false);
        _c.mute.Active(false);
        _c.pan.Value(0);
        _c.volume.Value(1);
    }

    // if there were an uneven amount of channels, add one last mono channel
    if (i == m_Asio.Information().inputChannels - 1)
    {
        // Add a ChannelPanel with all the inputs
        auto& _c = m_InputsPanel.Emplace<EndpointChannel>(
            ChannelBase::Type::Input | ChannelBase::Type::Endpoint);
        _c.AddEndpoint(&m_Asio.Inputs()[i]);
        m_Channels.push_back(&_c);

        // Set all parameters of this Channel
        _c.mono.Active(false);
        _c.mute.Active(false);
        _c.pan.Value(0);
        _c.volume.Value(1);
    }

    for (i = 0; i < m_Asio.Information().outputChannels - 1; i += 2)
    {
        // Add a ChannelPanel with all the outputs
        auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(
            ChannelBase::Type::Output | ChannelBase::Type::Endpoint);
        _c.AddEndpoint(&m_Asio.Outputs()[i]);
        _c.AddEndpoint(&m_Asio.Outputs()[i + 1]);
        m_Channels.push_back(&_c);

        // Set all parameters of this Channel
        _c.mono.Active(false);
        _c.mute.Active(false);
        _c.pan.Value(0);
        _c.volume.Value(1);
    }

    // if there were an uneven amount of channels, add one last mono channel
    if (i == m_Asio.Information().outputChannels - 1)
    {
        // Add a ChannelPanel with all the outputs
        auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(
            ChannelBase::Type::Output | ChannelBase::Type::Endpoint);
        _c.AddEndpoint(&m_Asio.Outputs()[i]);
        m_Channels.push_back(&_c);

        // Set all parameters of this Channel
        _c.mono.Active(false);
        _c.mute.Active(false);
        _c.pan.Value(0);
        _c.volume.Value(1);
    }
}

void Audio::SaveRouting()
{
    // If no device is loaded there is nothing to save.
    if (m_Asio.Information().input == -1)
        return;

    CrashLog("Saving Routing");
    try
    {
        nlohmann::json _json;
        _json["channels"] = nlohmann::json::array();

        // Put all channels in the json (channels have json operator)
        for (auto& _ch : Channels())
            _json["channels"].push_back(*_ch);

        // Save to routing file.
        std::ofstream _out;
        _out.open("./settings/routing" + std::to_string(m_Asio.Information().input));
        _out << std::setw(4) << _json; // Pretty print
        _out.close();
    }
    catch (...)
    {
        CrashLog("Failed to save routing");
    }
}

void Audio::SortChannels()
{
    // Define sort algorithm
    static auto sort = [](Panel& p)
    {
        std::sort(p.Components().begin(), p.Components().end(),
            [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
            {
                auto channel1 = dynamic_cast<ChannelBase*>(a.get());
                auto channel2 = dynamic_cast<ChannelBase*>(b.get());

                if (channel1 != nullptr && channel2 != nullptr)
                    return channel1->Id() < channel2->Id();

                return -1;
            });
    };

    // Sort all panels
    sort(m_InputsPanel);
    sort(m_OutputsPanel);
}

void Audio::UpdateEffects()
{
    for (auto& i : m_Channels)
    {
        i->EffectChain().UpdateEffects();
        if (i->Type() & ChannelBase::Type::Endpoint)
            ((EndpointChannel*)i)->m_SubChannels.UpdateEffects();
    }
}

void Audio::Update(const Vec4<int>& v)
{
    if (m_Click)
        m_Click--;

    // Make sure all colors stay updated with the theme
    m_ChannelScrollPanel.Background(ThemeT::Get().window_panel);
    m_DeviceName.TextColor(ThemeT::Get().text);

    // Make sure the panels and dividers are visible when any channel is visible in them
    bool ivis = false; for (auto& _c : m_InputsPanel.Components()) if (_c->Visible()) { ivis = true; break; }
    bool ovis = false; for (auto& _c : m_OutputsPanel.Components()) if (_c->Visible()) { ovis = true; break; }
    m_InputsPanel.Visible(ivis);
    m_OutputsPanel.Visible(ovis);
    m_Divider.Visible(ovis && ivis);

    Panel::Update(v);
}

void Audio::Render(CommandCollection& d)
{
    d.Command<Graphics::PushMatrix>();
    d.Command<Graphics::Translate>(Position());

    // Background as panel
    d.Command<Graphics::Fill>(ThemeT::Get().window_panel);
    d.Command<Graphics::Quad>(Vec4<int>{ 0, 0, Size() });

    // Title bar as frame color
    d.Command<Graphics::Fill>(ThemeT::Get().window_frame);
    d.Command<Graphics::Quad>(Vec4<int>{ 0, Height() - 40, Width(), 40 });
    d.Command<Graphics::PopMatrix>();

    Panel::Render(d);
}

void Audio::AsioCallback(Buffer<float>& input, Buffer<float>& output, CallbackInfo info, Audio& _this)
{
    auto& _inputs = _this.Asio().Inputs();
    auto& _outputs = _this.Asio().Outputs();

    auto& _channels = _this.Channels();

    for (int i = 0; i < info.bufferSize; i++)
    {
        // First input the samples from the input buffer to the input endpoints
        for (int j = 0; j < info.inputChannels; j++)
            _inputs[j].sample = input[i][j];

        // Process all channels, this lock is here for splitting/combining channels
        _this.m_Lock.lock();
        for (auto& j : _channels)
            j->Process();
        _this.m_Lock.unlock();

        // Output the samples from the output endpoints to the output buffer.
        for (int j = 0; j < info.outputChannels; j++)
            output[i][j] = _outputs[j].sample;
    }
}