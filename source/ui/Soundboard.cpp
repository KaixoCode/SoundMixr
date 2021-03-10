#include "ui/Soundboard.hpp"

Soundboard::Soundboard()
    : Frame("Soundboard", 1000, 500, true, false, false)
{
    namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

    auto& _panel = this->Panel();
    this->Icon(IDI_ICON1);

    _panel.Layout<Layout::Grid>(4, 4, 8, 8);

    for (int i = 0; i < 16; i++) {        
        m_Buttons.push_back(&_panel.Emplace<SoundboardButton>());
    }
}

float Soundboard::GetLevel(int channel)
{
    float totalLevel = 0;
    for (int i = 0; i < 16; i++) {
        totalLevel += m_Buttons[i]->GetLevel(channel);
    }

    return totalLevel;
}