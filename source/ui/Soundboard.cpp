#include "ui/Soundboard.hpp"

Soundboard::Soundboard()
    : Frame("Soundboard", 500, 500, true, false)
{
    namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

    auto& _panel = this->Panel();
    this->Icon(ASSET("textures/logo.png"));

    _panel.Layout<Layout::Grid>(4, 4, 8, 8);

    AudioFile<double> file;
    file.load("./audio/audio.wav");

    for (int i = 0; i < 16; i++) {
        _panel.Emplace<Button<G::Menu, BT::Normal>>([=] { LOG(i); }, std::to_string(i));
    }
}