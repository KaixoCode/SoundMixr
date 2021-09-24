#pragma once
#include "pch.hpp"
#include "Audio.hpp"
#include "ChannelBase.hpp"

struct Audio
{
    std::mutex lock;
    Stream<Asio> stream;
    std::vector<Pointer<Endpoint>> inputs;
    std::vector<Pointer<Endpoint>> outputs;
    std::list<Pointer<ChannelBase>> channels;

    Audio()
    {
        stream.Callback(
            [&](Buffer<float>& input, Buffer<float>& output, CallbackInfo info) {
                for (int i = 0; i < info.bufferSize; i++)
                {
                    // First input the samples from the input buffer to the input endpoints
                    for (int j = 0; j < info.inputChannels; j++)
                        inputs[j]->sample = input[i][j];

                    // Process all channels, this lock is here for splitting/combining channels
                    lock.lock();
                    for (auto& j : channels)
                        j->Process();
                    lock.unlock();

                    // Output the samples from the output endpoints to the output buffer.
                    for (int j = 0; j < info.outputChannels; j++)
                        output[i][j] = outputs[j]->sample;
                }
            });
    }

    void push_back(const Pointer<ChannelBase>& channel)
    {
        std::lock_guard _{ lock };
        channels.push_back(channel);
    }

    void remove(const Pointer<ChannelBase>& channel)
    {
        std::lock_guard _{ lock };
        channels.remove(channel);
    }


    bool Open(int id)
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

    void Start()
    {
        stream.Start();
    }

    void Stop()
    {
        stream.Stop();
    }

    void Close()
    {
        stream.Close();

        inputs.clear();
        outputs.clear();
        channels.clear();
    }
};