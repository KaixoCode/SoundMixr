#pragma once
#include "pch.hpp"
#include "PluginLoader.hpp"
#include "Audio/Audio.hpp"
#include "Midi.hpp"


class Controller
{
public:
    static inline int DEVICE_DROPDOWN = 0xAC3891AB;
    static inline int ZOOM_SLIDER = 0xAC3891AC;
    static inline int MIDI_DEVICE_LIST = 0xAC3891AD;

    static inline Controller& Get()
    {
        static Controller controller;
        return controller;
    }
    
    void Run();
    void OpenDevice(int device);

    Audio::Audio audio{};

    Pointer<Frame> window;
    Pointer<Frame> settings;

private:
    Controller();

    void LinkParsers();

    friend int main();
    friend int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
};                            