#pragma once
#include "pch.hpp"

namespace Audio
{
    struct Endpoint
    {
        const std::string name;
        int id;
        bool input;
        float sample = 0;
    };

    struct Audio
    {
        std::mutex lock;
        Stream<Asio> stream;
        std::vector<Pointer<Endpoint>> inputs;
        std::vector<Pointer<Endpoint>> outputs;
        std::vector<std::vector<float>> mix;

        Audio();

        bool Open(int id);
        void Start();
        void Stop();
        void Close();

        void SaveRouting(const std::string& path = "");
        void LoadRouting(const std::string& path = "");
    };
}