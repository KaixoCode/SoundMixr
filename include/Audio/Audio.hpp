#pragma once
#include "pch.hpp"
#include "Audio/Channel.hpp"

struct Audio
{
    std::mutex lock;
    Stream<Asio> stream;
    std::vector<Pointer<Endpoint>> inputs;
    std::vector<Pointer<Endpoint>> outputs;
    std::vector<Pointer<ChannelBase>> channels;

    Audio();

    void push_back(const Pointer<ChannelBase>& channel);
    void remove(const Pointer<ChannelBase>& channel);

    bool Open(int id);
    void Start();
    void Stop();
    void Close();

    void SaveRouting(const std::string& path = "");
    void LoadRouting(const std::string& path = "");
    void DefaultRouting();
};