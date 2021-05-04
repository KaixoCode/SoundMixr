#include "audio/Audio.hpp"

Audio::Audio()
    :
    // This panel contains the channel scrollpanel in the center and the 
    // device name at the top. The effect panel will be displayed in the east.
    m_ChannelScrollPanel(Emplace<SMXRScrollPanel>(Layout::Hint::Center)),
    m_DeviceName(Emplace<TextComponent>(Layout::Hint::North, "Channels", Graphics::Fonts::Gidole, 24.0f)),
    m_EffectPanel(Emplace<EffectPanel>(Layout::Hint::East)),

    // The panel in the channel scrollpanel contains the input panel and output panel
    // also add a divider inbetween. The order off addition is important! First added will
    // be on the left of the panel.
    m_InputsPanel(m_ChannelScrollPanel.Panel<Panel>().Emplace<Panel>()),
    m_Divider(m_ChannelScrollPanel.Panel().Emplace<VerticalMenuDivider>(1, 2, 4, 0)),
    m_OutputsPanel(m_ChannelScrollPanel.Panel().Emplace<Panel>()),
    m_Divider2(m_ChannelScrollPanel.Panel().Emplace<VerticalMenuDivider>(1, 2, 4, 0)),
    m_GeneratorPanel(m_ChannelScrollPanel.Panel().Emplace<Panel>())
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
    m_GeneratorPanel.MinWidth(70);
    m_GeneratorPanel.Width(70);
    m_GeneratorPanel.Layout<Layout::SidewaysStack>(8, 8);
    m_GeneratorPanel.AutoResize(true, false);
    /*m_GeneratorPanel.Hide();
    m_Divider2.Hide();*/

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
        if (e.button == Event::MouseButton::LEFT && ChannelBase::selected && m_EffectPanel.Visible())
        {
            m_EffectPanel.EffectChain(&ChannelBase::selected->EffectChain());
            m_EffectPanel.Name(ChannelBase::selected->name.Content());
        }

        // If rightclick, check if hovering over a channel and show a rightclick
        // menu generated from that channel.
        if (e.button == Event::MouseButton::RIGHT && !RightClickMenu::Get().Opened())
        {
            if (m_InputsPanel.HoveringComponent())
                GenerateMenu(m_InputsPanel), RightClickMenu::Get().Open(&m_Menu);
            else if (m_OutputsPanel.HoveringComponent())
                GenerateMenu(m_OutputsPanel), RightClickMenu::Get().Open(&m_Menu);
            else if (m_GeneratorPanel.Hovering())
            {
                m_Menu.Clear();
                m_Menu.ButtonSize({ 180, 20 });
                m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>("Add Generator").Disable();
                m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
                m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
                    [&]
                    {
                        m_Lock.lock();

                        // Emplace a channelgroup to the list
                        auto& _c = m_GeneratorPanel.Emplace<GeneratorChannel>();
                        m_Channels.push_back(&_c);

                        m_Lock.unlock();
                    }, " + Test Thing");
                RightClickMenu::Get().Open(&m_Menu);

            }
            else
            {
                m_Menu.Clear();
                m_Menu.ButtonSize({ 180, 20 });
                m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>("Add Generator").Disable();
                m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
                m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
                    [&]
                    {
                        m_Lock.lock();

                        // Emplace a channelgroup to the list
                        auto& _c = m_GeneratorPanel.Emplace<GeneratorChannel>();
                        m_Channels.push_back(&_c);

                        m_Lock.unlock();
                    }, " + Test Thing");
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
        if (m_Click > 0 && m_PrevPos == Vec2<int>{ e.x, e.y }&& ChannelBase::selected&& ChannelBase::selected->Hovering())
        {
            // Also make sure we're not hovering over the volume or pan parameter, because
            // double clicking on those will reset their value and should not open the effect panel.
            if (!ChannelBase::selected->volume.Hovering() && !ChannelBase::selected->pan.Hovering() &&
                !ChannelBase::selected->mute.Hovering() && !ChannelBase::selected->mono.Hovering() &&
                !ChannelBase::selected->name.Hovering())
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

    m_Menu.Clear();
    m_Menu.ButtonSize({ 180, 20 });
    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(hover->name.Content()).Disable();
    m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
    m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>(
        [&, hover] {
            m_EffectPanel.EffectChain(&hover->EffectChain());
            m_EffectPanel.Name(hover->name.Content());
        }, "Show Effects");

    // A channel can split if it has more than 1 line, and is an endpoint
    bool canSplit = (hover->Type() & ChannelBase::Type::Endpoint) && hover->Lines() > 1;

    // Channels can combine if they are the same type, and not the same channel.
    // and an endpoint.
    bool canCombine = (hover->Type() & ChannelBase::Type::Endpoint) && focus && 
        focus->Type() == hover->Type() && focus != hover;

    bool canAdd = (hover->Type() & ChannelBase::Type::Generator);

    // Add divider if any of the 2 are possible
    if (canSplit || canCombine || canAdd)
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

                // Remove the hovering panel from both the panel and the channels.
                auto it = std::remove_if(panel.Components().begin(), panel.Components().end(),
                    [hover](std::unique_ptr<Component>& u) { return u.get() == hover; });
                panel.Erase(it);
                auto it2 = std::remove(m_Channels.begin(), m_Channels.end(), hover);
                m_Channels.erase(it2);
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
    // Don't open if already same device.
    if (m_Asio.DeviceId() == id)
        return false;

    // Make sure the effect chain being displayed is set to nullptr
    // since it will otherwise be undefined.
    m_EffectPanel.EffectChain(nullptr);

    // Close the stream in case there was one open.
    m_Asio.CloseStream();

    // Set the device and open the stream
    m_Asio.Device(id);
    if (!m_Asio.OpenStream(AsioCallback, this))
    {
        m_Asio.NoDevice();
        return false;
    }

    // Start stream
    LoadRouting();
    if (!m_Asio.StartStream())
    {
        m_Asio.CloseStream();
        m_Asio.NoDevice();
        return false;
    }

    // Set device name
    m_DeviceName.Content(m_Asio.Device().info.name);
    return true;
}

bool Audio::CloseDevice()
{
    // Don't close if already closed.
    if (m_Asio.DeviceId() == -1)
        return false;

    // Make sure the effect chain being displayed is set to nullptr
    // since it will otherwise be undefined.
    m_EffectPanel.EffectChain(nullptr);

    // Close the stream in case there was one open.
    m_Asio.CloseStream();
    SaveRouting();

    // Set to no device.
    m_Asio.NoDevice();

    // Clear the panel.
    Clear();
    m_DeviceName.Content("No Device");
    return true;
}

void Audio::ShowControlPanel()
{
    // Can't show control panel if no device.
    if (m_Asio.DeviceId() == -1)
        return;

    // Save the routing before closing.
    SaveRouting();

    // Make sure everything is cleared before closing, since it will
    // remove all input and output endpoints and leave them undefined
    // inside all channels.
    Clear();

    // Close the stream so we can open the control panel.
    m_Asio.CloseStream();

    // Show control panel.
    PaAsio_ShowControlPanel(m_Asio.Device().id, nullptr);

    // Open and start the stream
    m_Asio.OpenStream(AsioCallback, this);
    m_Asio.StartStream();

    // Load the routing again.
    LoadRouting();
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

    for (auto& c : m_GeneratorPanel.Components())
    {
        ChannelBase* s = dynamic_cast<ChannelBase*>(c.get());
        if (s)
            m_Channels.push_back(s);
    }

    //m_GeneratorPanel.Clear();
}

void Audio::LoadRouting()
{
    // If no device, no routing.
    if (m_Asio.DeviceId() == -1)
        return;

    // First make sure everything is cleared.
    Clear();

    // Keep track which channels have been loaded from the file so
    // we can later add the ones that weren't added separately.
    std::unordered_map<int, bool> _inputIdsLoaded, _outputIdsLoaded;
    for (int i = 0; i < m_Asio.Device().info.maxInputChannels; i++)
        _inputIdsLoaded.emplace(i, false);

    for (int i = 0; i < m_Asio.Device().info.maxOutputChannels; i++)
        _outputIdsLoaded.emplace(i, false);

    // Open the routing file for the current device.
    LOG("Loading Routing");
    std::ifstream _in;
    _in.open("./settings/routing" + std::to_string(m_Asio.Device().id));

    bool _error = true;
    if (!_in.fail())
    {
        _error = false;
        try
        {
            nlohmann::json _json;
            _in >> _json;

            // First load all the output channels
            auto _outputs = _json.at("output_channels");
            for (auto& i : _outputs)
            {
                // Emplace a channelgroup to the list
                auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(
                    ChannelBase::Type::Output | ChannelBase::Type::Endpoint);
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

                // Set the rest of the parameters
                _c = i;
            }

            // Load all the input channels
            auto _inputs = _json.at("input_channels");
            for (auto& i : _inputs)
            {
                // Emplace a channelgroup to the list
                auto& _c = m_InputsPanel.Emplace<EndpointChannel>(
                    ChannelBase::Type::Input | ChannelBase::Type::Endpoint);
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

                // Then add all the connections of this channelgroup
                nlohmann::json _connections = i.at("connections");
                for (int i : _connections)
                {
                    // Find the output channelgroup by id.
                    auto& _it = std::find_if(Channels().begin(), Channels().end(),
                        [=](ChannelBase* c)
                        {
                            bool output = (int)c->Type() & (int)ChannelBase::Type::Output;
                            return output && c->Id() == i;
                        }
                    );

                    // If it exists, connect with it.
                    if (_it != Channels().end())
                        _c.Connect(*_it);
                }

                _c = i;
            }
        }

        // If error occured (either file didn't exist or was parced incorrectly
        // load all the channels as stereo channels.
        catch (std::exception)
        {
            _error = true;
        }
    }

    // If error, just use default routing;
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
}

void Audio::DefaultRouting()
{
    Clear();
    int i = 0;
    for (i = 0; i < m_Asio.Device().info.maxInputChannels - 1; i += 2)
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
    if (i == m_Asio.Device().info.maxInputChannels - 1)
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

    for (i = 0; i < m_Asio.Device().info.maxOutputChannels - 1; i += 2)
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
    if (i == m_Asio.Device().info.maxOutputChannels - 1)
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
    if (m_Asio.DeviceId() == -1)
        return;

    LOG("Saving Routing");

    nlohmann::json _json;
    _json["input_channels"] = nlohmann::json::array();

    // Inputs
    for (auto& _ch : Channels())
    {
        if (_ch->Type() == (ChannelBase::Type::Input | ChannelBase::Type::Endpoint))
            _json["input_channels"].push_back(*_ch);
    }

    _json["output_channels"] = nlohmann::json::array();

    // Outputs
    for (auto& _ch : Channels())
    {
        if (_ch->Type() == (ChannelBase::Type::Output | ChannelBase::Type::Endpoint))
            _json["output_channels"].push_back(*_ch);
    }

    std::ofstream _out;
    _out.open("./settings/routing" + std::to_string(m_Asio.DeviceId()));
    _out << std::setw(4) << _json;
    _out.close();
}

void Audio::SortChannels()
{
    // Define sort algorithm
    static auto sort = [](Panel& p)
    {
        std::sort(p.Components().begin(), p.Components().end(),
            [](const std::unique_ptr<Component>& a, const std::unique_ptr<Component>& b) -> bool
            {
                auto channel1 = (ChannelBase*)a.get();
                auto channel2 = (ChannelBase*)b.get();

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
        i->EffectChain().UpdateEffects();
}

void Audio::Update(const Vec4<int>& v)
{
    if (m_Click)
        m_Click--;

    // Make sure all colors stay updated with the theme
    m_ChannelScrollPanel.Background(ThemeT::Get().window_panel);
    m_DeviceName.TextColor(ThemeT::Get().text);

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

int Audio::AsioCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    Audio& _this = *static_cast<Audio*>(userData);

    float* _inBuffer = (float*)inputBuffer;
    float* _outBuffer = (float*)outputBuffer;

    int _inChannelCount = _this.Asio().Device().info.maxInputChannels;
    int _outChannelCount = _this.Asio().Device().info.maxOutputChannels;

    auto& _inputs = _this.Asio().Inputs();
    auto& _outputs = _this.Asio().Outputs();

    auto& _channels = _this.Channels();

    for (int i = 0; i < nBufferFrames; i++)
    {
        // First input the samples from the input buffer to the input endpoints
        for (int j = 0; j < _inChannelCount; j++)
            _inputs[j].sample = _inBuffer[i * _inChannelCount + j];

        // Process all channels, this lock is here for splitting/combining channels
        _this.m_Lock.lock();
        for (auto& j : _channels)
            j->Process();
        _this.m_Lock.unlock();

        // Output the samples from the output endpoints to the output buffer.
        for (int j = 0; j < _outChannelCount; j++)
            _outBuffer[i * _outChannelCount + j] = _outputs[j].sample;
    }
    return 0;
}