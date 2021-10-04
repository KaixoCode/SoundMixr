#include "Audio/Audio.hpp"
namespace Audio
{
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
                        if (j->type & Channel::Type::Input)
                            j->Process();

                    for (auto& j : channels) // Then do all output processing
                        if (j->type & Channel::Type::Output)
                            j->Process();
                    lock.unlock();

                    // Output the samples from the output endpoints to the output buffer.
                    for (int j = 0; j < info.outputChannels; j++)
                        output[i][j] = outputs[j]->sample;
                }
            });
    }

    void Audio::push_back(const Pointer<Channel>& channel)
    {
        std::lock_guard _{ lock };
        channels.push_back(channel);
    }

    void Audio::remove(const Pointer<Channel>& channel)
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

        LoadRouting();

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
        SaveRouting();
        stream.Close();

        inputs.clear();
        outputs.clear();
        channels.clear();
    }

    void Audio::SaveRouting(const std::string& path)
    {
        try
        {
            nlohmann::json _json = nlohmann::json::object();
            _json["channels"] = nlohmann::json::array();

            for (auto& i : channels)
                _json["channels"].push_back(*i);

            std::ofstream _out;
            if (path.empty())
                _out.open("./settings/routing" + std::to_string(stream.Information().input));
            else
                _out.open(path);

            _out << std::setw(4) << _json;
        }
        catch (...)
        {

        }
    }

    void Audio::LoadRouting(const std::string& path)
    {
        try
        {
            // Keep track which channels have been loaded from the file so
            // we can later add the ones that weren't added separately.
            bool _soundBoardLoaded = false;
            std::unordered_map<int, bool> _inputIdsLoaded, _outputIdsLoaded;
            for (int i = 0; i < stream.Information().inputChannels; i++)
                _inputIdsLoaded.emplace(i, false);

            for (int i = 0; i < stream.Information().outputChannels; i++)
                _outputIdsLoaded.emplace(i, false);

            // Open the routing file for the current device.
            CrashLog("Loading Routing");
            std::ifstream _in;
            if (path.empty())
                _in.open("./settings/routing" + std::to_string(stream.Information().input));
            else
                _in.open(path);

            // Start the loading process if the file opened successfully
            // otherwise we got an error!
            bool _error = true;
            Channel::id_counter = 0;
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
                        if (_type & Channel::Type::Input)
                            continue;

                        // If type is Endpoint, add endpoint to outputspanel
                        if (_type & Channel::Type::Endpoint)
                        {
                            auto _ptr = new EndpointChannel{ false };
                            push_back(_ptr); // Takes ownership
                            *_ptr = i;
                            if (_ptr->id > Channel::id_counter)
                                Channel::id_counter = _ptr->id;
                        }
                    }

                    // Then go through all input channels
                    for (auto& i : _channels)
                    {
                        // Skip output channels
                        int _type = i.at("type").get<int>();
                        if (_type & Channel::Type::Output)
                            continue;

                        Channel* _base = nullptr;

                        // If type is Endpoint, add endpoint channel to inputspanel
                        if (_type & Channel::Type::Endpoint)
                        {
                            auto _ptr = new EndpointChannel{ true };
                            push_back(_ptr); // Takes ownership
                            _base = _ptr;
                            *_ptr = i;
                            if (_ptr->id > Channel::id_counter)
                                Channel::id_counter = _ptr->id;
                        }

                        // If no compatible type was found, continue.
                        if (!_base)
                            continue;

                        // Then add all the connections to the channel
                        nlohmann::json _connections = i.at("connections");
                        for (int i : _connections)
                        {
                            // Find the outputchannel using its id.
                            auto _it = std::find_if(channels.begin(), channels.end(),
                                [=](Channel* c)
                                {
                                    bool output = (int)c->type & (int)Channel::Type::Output;
                                    return output && c->id == i;
                                }
                            );

                            // If it exists, connect with it.
                            if (_it != channels.end())
                                _base->Connect(*_it);
                        }
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

            if (!_soundBoardLoaded)
            {
                //auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
                //m_Channels.push_back(&_c);
            }
        }
        catch (...)
        {
            return;
        }
    }

    void Audio::DefaultRouting()
    {
        // Soundboard channel
        //auto& _c = m_InputsPanel.Emplace<SoundboardChannel>();
        //m_Channels.push_back(&_c);

        // Endpoint channels
        int i = 0;
        for (i = 0; i < stream.Information().inputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the inputs
            auto _ptr = new EndpointChannel{ true };
            push_back(_ptr); // Takes ownership
            _ptr->Add(i);
            _ptr->Add(i + 1);

            // Set all parameters of this Channel
            _ptr->mono = false;
            _ptr->mute = false;
            _ptr->pan = 0;
            _ptr->volume = 1;
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == stream.Information().inputChannels - 1)
        {
            // Add a ChannelPanel with all the inputs
            auto _ptr = new EndpointChannel{ true };
            push_back(_ptr); // Takes ownership
            _ptr->Add(i);

            // Set all parameters of this Channel
            _ptr->mono = false;
            _ptr->mute = false;
            _ptr->pan = 0;
            _ptr->volume = 1;
        }

        for (i = 0; i < stream.Information().outputChannels - 1; i += 2)
        {
            // Add a ChannelPanel with all the outputs
            auto _ptr = new EndpointChannel{ false };
            push_back(_ptr); // Takes ownership
            _ptr->Add(i);
            _ptr->Add(i + 1);

            // Set all parameters of this Channel
            _ptr->mono = false;
            _ptr->mute = false;
            _ptr->pan = 0;
            _ptr->volume = 1;
        }

        // if there were an uneven amount of channels, add one last mono channel
        if (i == stream.Information().outputChannels - 1)
        {
            // Add a ChannelPanel with all the outputs
            auto _ptr = new EndpointChannel{ false };
            push_back(_ptr); // Takes ownership
            _ptr->Add(i);

            // Set all parameters of this Channel
            _ptr->mono = false;
            _ptr->mute = false;
            _ptr->pan = 0;
            _ptr->volume = 1;
        }
    }
}