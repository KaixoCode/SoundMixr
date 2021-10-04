#pragma once
#include "pch.hpp"
#include "Audio/Channel.hpp"

namespace Audio
{
    struct Audio
    {
        std::mutex lock;
        Stream<Asio> stream;
        std::vector<Pointer<Endpoint>> inputs;
        std::vector<Pointer<Endpoint>> outputs;
        std::vector<Pointer<Channel>> channels;

        Audio();

        void push_back(const Pointer<Channel>& channel);
        void remove(const Pointer<Channel>& channel);

        bool Open(int id);
        void Start();
        void Stop();
        void Close();

        void SaveRouting(const std::string& path = "");
        void LoadRouting(const std::string& path = "");
        void DefaultRouting();
    };
}