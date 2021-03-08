#include "ui/Soundboard.hpp"

Soundboard::Soundboard()
    : Frame("Soundboard", 1000, 1000, true, true, false)
{
    namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

    auto& _panel = this->Panel();
    this->Icon(ASSET("textures/logo.png"));

    _panel.Layout<Layout::Grid>(4, 4, 8, 8);

    for (int i = 0; i < 16; i++) {
        _panel.Emplace<SoundboardButton>();
    }
}

float Soundboard::GetLevel(int channel)
{
    return 0;
}