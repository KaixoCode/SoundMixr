#include "Audio.hpp"

Audio::Audio()
{
    stream.Callback(
        [this](Buffer<float>& input, Buffer<float>& output, CallbackInfo info) {
            for (int i = 0; i < info.bufferSize; i++)
            {
                // First input the samples from the input buffer to the input endpoints
                for (int j = 0; j < info.inputChannels; j++)
                    inputs[j]->sample = input[i][j];

                // Process all channels
                lock.lock();
                for (auto& j : channels) // Prepare all channels for the next cycle
                    j->NextCycle();

                for (auto& j : channels) // Then do all input processing
                    if (j->type & ChannelBase::Type::Input) 
                        j->Process();

                for (auto& j : channels) // Then do all output processing
                    if (j->type & ChannelBase::Type::Output)
                        j->Process();
                lock.unlock();

                // Output the samples from the output endpoints to the output buffer.
                for (int j = 0; j < info.outputChannels; j++)
                    output[i][j] = outputs[j]->sample;
            }
        });
}

void Audio::push_back(const Pointer<ChannelBase>& channel)
{
    std::lock_guard _{ lock };
    channels.push_back(channel);
}

void Audio::remove(const Pointer<ChannelBase>& channel)
{
    std::lock_guard _{ lock };
    channels.erase(std::remove(channels.begin(), channels.end(), channel));
}

bool Audio::Open(int id)
{
    if (stream.Open({ .input = id, .output = id }) != NoError)
        return false;

    for (auto& i : stream.Device(id).Channels())
        if (i.input)
            inputs.push_back(new Endpoint{ .name = i.name, .id = i.id, .input = true });
        else
            outputs.push_back(new Endpoint{ .name = i.name, .id = i.id, .input = false });

    return true;
}

void Audio::Start()
{
    stream.Start();
}

void Audio::Stop()
{
    stream.Stop();
}

void Audio::Close()
{
    stream.Close();

    inputs.clear();
    outputs.clear();
    channels.clear();
}

void Audio::SaveRouting()
{
    nlohmann::json _json = nlohmann::json::object();
    _json["channels"] = nlohmann::json::array();

    for (auto& i : channels)
        _json["channels"].push_back(*i);
}

    //void LoadRouting(const std::string& path)
    //{
    //    try
    //    {
    //        // Keep track which channels have been loaded from the file so
    //        // we can later add the ones that weren't added separately.
    //        bool _soundBoardLoaded = false;
    //        std::unordered_map<int, bool> _inputIdsLoaded, _outputIdsLoaded;
    //        for (int i = 0; i < stream.Information().inputChannels; i++)
    //            _inputIdsLoaded.emplace(i, false);

    //        for (int i = 0; i < stream.Information().outputChannels; i++)
    //            _outputIdsLoaded.emplace(i, false);

    //        // Open the routing file for the current device.
    //        CrashLog("Loading Routing");
    //        std::ifstream _in{ path };

    //        // Start the loading process if the file opened successfully
    //        // otherwise we got an error!
    //        bool _error = true;
    //        if (!_in.fail())
    //        {
    //            _error = false;
    //            try
    //            {
    //                // Parse the json.
    //                nlohmann::json _json;
    //                _in >> _json;

    //                // Get the channels from the json
    //                auto _channels = _json.at("channels");

    //                // First load all the output channels
    //                for (auto& i : _channels)
    //                {
    //                    // Skip input channels
    //                    int _type = i["type"].get<int>();
    //                    if (_type & ChannelBase::Type::Input)
    //                        continue;

    //                    ChannelBase* _base = nullptr;

    //                    // If type is Endpoint, add endpoint to outputspanel
    //                    if (_type & ChannelBase::Type::Endpoint)
    //                    {
    //                        auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(_type);
    //                        _base = &_c;
    //                        m_Channels.push_back(&_c);

    //                        // Add all channels that are in this channelgroup
    //                        nlohmann::json _channels = i.at("channels");
    //                        for (int i : _channels)
    //                        {
    //                            if (i >= _outputIdsLoaded.size())
    //                                break;
    //                            _outputIdsLoaded[i] = true;
    //                            _c.AddEndpoint(&m_Asio.Outputs()[i]);
    //                        }
    //                    }

    //                    // If no compatible type was found, continue.
    //                    if (!_base)
    //                        continue;

    //                    // Send json to the channel to load channel settings.
    //                    *_base = i;
    //                }

    //                // Then go through all input channels
    //                for (auto& i : _channels)
    //                {
    //                    // Skip output channels
    //                    int _type = i["type"].get<int>();
    //                    if (_type & ChannelBase::Type::Output)
    //                        continue;

    //                    ChannelBase* _base = nullptr;

    //                    // If type is Endpoint, add endpoint channel to inputspanel
    //                    if (_type & ChannelBase::Type::Endpoint)
    //                    {
    //                        auto& _c = m_InputsPanel.Emplace<EndpointChannel>(_type);
    //                        _base = &_c;
    //                        m_Channels.push_back(&_c);

    //                        // Add all channels that are in this channelgroup
    //                        nlohmann::json _channels = i.at("channels");
    //                        for (int i : _channels)
    //                        {
    //                            if (i >= _inputIdsLoaded.size())
    //                                break;

    //                            _inputIdsLoaded[i] = true;
    //                            _c.AddEndpoint(&m_Asio.Inputs()[i]);
    //                        }
    //                    }

    //                    // If type is soundboard, add soundboard channel to inputspanel
    //                    else if (_type & ChannelBase::Type::SoundBoard)
    //                    {
    //                        _soundBoardLoaded = true;
    //                        auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
    //                        _base = &_c;
    //                        m_Channels.push_back(&_c);
    //                    }

    //                    // If no compatible type was found, continue.
    //                    if (!_base)
    //                        continue;

    //                    // Then add all the connections to the channel
    //                    nlohmann::json _connections = i.at("connections");
    //                    for (int i : _connections)
    //                    {
    //                        // Find the outputchannel using its id.
    //                        auto _it = std::find_if(Channels().begin(), Channels().end(),
    //                            [=](ChannelBase* c)
    //                            {
    //                                bool output = (int)c->Type() & (int)ChannelBase::Type::Output;
    //                                return output && c->Id() == i;
    //                            }
    //                        );

    //                        // If it exists, connect with it.
    //                        if (_it != Channels().end())
    //                            _base->Connect(*_it);
    //                    }

    //                    // Send json to the channel to load channel settings.
    //                    *_base = i;
    //                }
    //            }

    //            // If error occured (either file didn't exist or was parced incorrectly
    //            // load all the channels as stereo channels.
    //            catch (std::exception e)
    //            {
    //                CrashLog("Error loading routing: " << e.what());
    //                _error = true;
    //            }
    //            catch (...)
    //            {
    //                CrashLog("Error loading routing");
    //            }
    //        }

    //        // If error, just use default routing and return
    //        if (_error)
    //        {
    //            DefaultRouting();
    //            _in.close();// Close the file!! important!!
    //            return;
    //        }

    //        _in.close();// Close the file!! important!!

    //        // If a couple channels were loaded then just load all the 
    //        // channels from the ASIO that weren't loaded from the file
    //        // as mono channels.
    //        for (auto& i : _inputIdsLoaded)
    //        {
    //            if (!i.second)
    //            {
    //                // Add a ChannelPanel with the input
    //                auto& _c = m_InputsPanel.Emplace<EndpointChannel>(
    //                    ChannelBase::Type::Input | ChannelBase::Type::Endpoint);
    //                _c.AddEndpoint(&m_Asio.Inputs()[i.first]);
    //                m_Channels.push_back(&_c);
    //            }
    //        }

    //        for (auto& i : _outputIdsLoaded)
    //        {
    //            if (!i.second)
    //            {
    //                // Add a ChannelPanel with the output
    //                auto& _c = m_OutputsPanel.Emplace<EndpointChannel>(
    //                    ChannelBase::Type::Output | ChannelBase::Type::Endpoint);
    //                _c.AddEndpoint(&m_Asio.Outputs()[i.first]);
    //                m_Channels.push_back(&_c);
    //            }
    //        }

    //        if (!_soundBoardLoaded)
    //        {
    //            auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
    //            m_Channels.push_back(&_c);
    //        }
    //    }
    //    catch (...)
    //    {
    //        return;
    //    }
    //}
