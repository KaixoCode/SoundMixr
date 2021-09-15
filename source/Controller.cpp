#include "Controller.hpp"

Controller::Controller()
{}

void Controller::Run()
{
    CrashLog("Initializing the Controller");

    Gui _gui;
    
    Frame _frame{ {.name = "SoundMixr" } };
    _gui.Add(_frame);

    while (_gui.Loop());
}