#pragma once
#include "pch.hpp"
#include "audio/AsioDevice.hpp"
#include "audio/EndpointChannel.hpp"
#include "ui/Graphics.hpp"
#include "audio/ChannelBase.hpp"

class AudioController : public Panel
{
public:
    AudioController()
        : m_ChannelScrollPanel(Emplace<SMXRScrollPanel>(Layout::Hint::Center)),
        m_DeviceName(Emplace<TextComponent>(Layout::Hint::North, "Channels", Graphics::Fonts::Gidole, 24.0f)),
        m_InputsPanel(m_ChannelScrollPanel.Panel<Panel>().Emplace<Panel>()),
        m_Divider(m_ChannelScrollPanel.Panel().Emplace<VerticalMenuDivider>(1, 2, 4, 0)),
        m_OutputsPanel(m_ChannelScrollPanel.Panel().Emplace<Panel>())
    {
        m_DeviceName.MinHeight(40);
        m_DeviceName.Height(40);
        m_DeviceName.AlignLines(Align::CENTER);

        Layout<Layout::Border>(0, 0, false, false, false, false);
        m_ChannelScrollPanel.EnableScrollbars(true, false);
        m_ChannelScrollPanel.Panel().Layout<Layout::SidewaysStack>(0, 0);
        m_ChannelScrollPanel.Panel().AutoResize(true, false);
        m_InputsPanel.Layout<Layout::SidewaysStack>(8, 8);
        m_InputsPanel.AutoResize(true, false);
        m_OutputsPanel.Layout<Layout::SidewaysStack>(8, 8);
        m_OutputsPanel.AutoResize(true, false);
    
        m_ChannelScrollPanel.Listener() += [this](Event::Unfocused& e)
        {
             ChannelBase::selected = nullptr;
        };
    }

    auto Channels() -> std::vector<ChannelBase*>& { return m_Channels; };
    auto Asio() -> Asio& { return m_Asio; }
    auto Devices() -> std::vector<Device>& { return m_Asio.Devices(); }
    auto Device() -> Device& { return m_Asio.Device(); }

    /**
     * Open the asio device with the given id.
     * @param id device id
     * @return true when successful
     */
    bool OpenDevice(int id)
    {
        // Don't open if already same device.
        if (m_Asio.DeviceId() == id)
            return false;

        // Close the stream in case there was one open.
        m_Asio.CloseStream();

        // Set the device and open the stream
        m_Asio.Device(id);
        if (!m_Asio.OpenStream(SarCallback, this))
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

        // Load the routing
        m_DeviceName.Content(m_Asio.Device().info.name);
        return true;
    }

    /**
     * Closes the currently opened asio device.
     * @return true when successful
     */
    bool CloseDevice()
    {
        // Don't close if already closed.
        if (m_Asio.DeviceId() == -1)
            return false;

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

    /**
     * Shows the asio control panel if a device is opened.
     */
    void ShowControlPanel()
    {
        // Can't show control panel if no device.
        if (m_Asio.DeviceId() == -1)
            return;

        m_Asio.CloseStream();
        PaAsio_ShowControlPanel(m_Asio.Device().id, nullptr);
        m_Asio.OpenStream(SarCallback, this);
        m_Asio.StartStream();
    }

    /**
     * Clear all the channels.
     */
    void Clear()
    {
        ChannelBase::selected = nullptr;
        m_Channels.clear();
        m_InputsPanel.Clear();
        m_OutputsPanel.Clear();
    }

    /**
     * Load the routing of the opened asio device.
     */
    void LoadRouting()
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
            catch (...)
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

    /**
     * Load the default routing of the asio device.
     */
    void DefaultRouting()
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

    /**
     * Save the current routing for the opened asio device.
     */
    void SaveRouting() 
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

    void Update(const Vec4<int>& v)
    {
        m_ChannelScrollPanel.Background(ThemeT::Get().window_panel);
        Background(ThemeT::Get().window_frame);
        m_DeviceName.TextColor(ThemeT::Get().text);

        Panel::Update(v);
    }

private:
    ::Asio m_Asio;

    std::vector<ChannelBase*> m_Channels;

    TextComponent& m_DeviceName;
    SMXRScrollPanel& m_ChannelScrollPanel;
    Panel& m_InputsPanel;
    VerticalMenuDivider& m_Divider;
    Panel& m_OutputsPanel;

    static int SarCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
    {
        AudioController& _this = *static_cast<AudioController*>(userData);

        float* _inBuffer = (float*)inputBuffer;
        float* _outBuffer = (float*)outputBuffer;

        int _inChannels = _this.Asio().Device().info.maxInputChannels;
        int _outChannels = _this.Asio().Device().info.maxOutputChannels;

        auto& _inputs = _this.Asio().Inputs();
        auto& _outputs = _this.Asio().Outputs();

        auto& _channels = _this.Channels();

        for (int i = 0; i < nBufferFrames; i++)
        {
            for (int j = 0; j < _inChannels; j++)
                _inputs[j].sample = _inBuffer[i * _inChannels + j];

            for (auto& j : _channels)
                j->Process();

            for (int j = 0; j < _outChannels; j++)
                _outBuffer[i * _outChannels + j] = _outputs[j].sample;
        }
        return 0;
    }
};