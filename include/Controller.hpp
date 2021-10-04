#pragma once
#include "pch.hpp"
#include "PluginLoader.hpp"
#include "Audio/Audio.hpp"
#include "Midi.hpp"


class Controller
{
public:
    static inline Controller& Get()
    {
        static Controller controller;
        return controller;
    }
    
    void Run();
    void OpenDevice(int device);

    Audio::Audio audio{};

    Frame window;

private:
    Controller();

    friend int main();
    friend int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
};                            