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

                    for (int out = 0; out < outputs.size(); out++)
                        outputs[out]->sample = 0;

                    // Mix levels
                    for (int in = 0; in < mix.size(); in++)
                        for (int out = 0; out < mix[in].size(); out++)
                            outputs[out]->sample += inputs[in]->sample * mix[in][out] * 0.01;

                    // Output the samples from the output endpoints to the output buffer.
                    for (int j = 0; j < info.outputChannels; j++)
                        output[i][j] = outputs[j]->sample;
                }
            });
    }

    bool Audio::Open(int id)
    {
        if (stream.Open({ .input = id, .output = id }) != NoError)
            return false;

        for (auto& i : stream.Device(id).Channels())
            if (i.input)
                inputs.emplace_back(new Endpoint{ i.name, i.id, true });
            else
                outputs.emplace_back(new Endpoint{ i.name, i.id, false });

        mix = { inputs.size(), std::vector<float>(outputs.size()) };
            
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
    }

    void Audio::SaveRouting(const std::string& path)
    {
        try
        {
            nlohmann::json _json = nlohmann::json::object();
            //_json["channels"] = nlohmann::json::array();

            //for (auto& i : channels)
            //    _json["channels"].push_back(*i);

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
            if (!_in.fail())
            {
                _error = false;
                try
                {
                    // Parse the json.
                    nlohmann::json _json;
                    _in >> _json;
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
                _in.close();// Close the file!! important!!
                return;
            }

            _in.close();// Close the file!! important!!
        }
        catch (...)
        {
            return;
        }
    }
}