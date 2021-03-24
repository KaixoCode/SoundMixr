#pragma once
#include "pch.hpp"
#include "Graphics.hpp"

/**
 * A window with a menu and standard close/maximize/minimize buttons. Also has a base <code>Panel</code> to put content on.
 */
class SoundMixrFrame : public Window
{
public:
    /**
     * Constructor
     * @param name name
     * @param width width
     * @param height height
     * @param hideonclose when true this window will not exit the program but simply hide the window when closed.
     * @param r resizeable
     */
    SoundMixrFrame(const std::string& name, const int width, const int height, bool hideonclose = false, bool show = true, bool r = true)
        : Window(name, width, height, hideonclose, show, r), m_Resizable(r),
        m_CloseButton(&Emplace<Button<SoundMixrGraphics::CloseButton, ButtonType::Normal>>([&]() { this->Close(); }, "")),
        m_MaxiButton1(&Emplace<Button<SoundMixrGraphics::Maxi1Button, ButtonType::Normal>>([&]() { this->Restore(); }, "")),
        m_MaxiButton2(&Emplace<Button<SoundMixrGraphics::Maxi2Button, ButtonType::Normal>>([&]() { this->Maximize(); }, "")),
        m_MinimButton(&Emplace<Button<SoundMixrGraphics::MiniButton, ButtonType::Normal>>([&]() { this->Iconify(); }, "")),
        m_Menu(&Emplace<::Menu<SoundMixrGraphics::Horizontal, MenuType::ToggleHover>>()),
        m_Panel(&Emplace<::Panel>())
    {
        m_CloseButton->Size({ 46, 32 });
        m_MaxiButton1->Size({ 46, 32 });
        m_MaxiButton2->Size({ 46, 32 });
        m_MinimButton->Size({ 46, 32 });
    }

    void Update(const Vec4<int>& viewport) override
    {
        bool _maxi = IsMaximized(GetWin32Handle());
        int _offset = _maxi ? -8 : 0;
        m_Panel->Size({ Width(), Height() - 32 + _offset });
        m_Panel->Position({ 0, 0 });
        int _x = Width() - 46 + _offset;
        m_CloseButton->Position({ _x, Height() - 32 + _offset });
        if (m_Resizable)
        {
            _x -= 46;
            m_MaxiButton1->Visible(_maxi);
            m_MaxiButton2->Visible(!_maxi);
            m_MaxiButton1->Position({ _x, Height() - 32 + _offset });
            m_MaxiButton2->Position({ _x, Height() - 32 + _offset });
        }
        else
        {
            m_MaxiButton1->Visible(false);
            m_MaxiButton2->Visible(false);
        }
        m_MinimButton->Position({ _x -= 46, Height() - 32 + _offset });

        _x = -_offset + 45;
        int _y = Height() - 32 + _offset;
        if (m_Menu->Components().size() != 0)
            m_Menu->Position({ _x + 6, _y });
        

        Window::Update(viewport);
    }

    void Render(CommandCollection& d) override
    {
        using namespace Graphics;
        Window::Render(d);
        bool _maxi = IsMaximized(GetWin32Handle());
        int _offset = _maxi ? -8 : 0;
        int _x = -_offset;
        int _y = Height() - 32 + _offset;
        static const std::string _line = "|";

        d.Command<TexturedQuad>(Graphics::Textures::Logo, _x + 12, Height() - 26 + _offset, 20, 20);
        d.Command<Fill>(ThemeT::Get().window_title_text);
        d.Command<Font>(Fonts::Gidole14, 14.0f);
        d.Command<TextAlign>(Align::CENTER, Align::CENTER);
        d.Command<Text>(&_line, _x += 45, _y + 16);

        if (m_Menu->Components().size() != 0)
        {
            _x += m_Menu->Width() + 6;
            d.Command<TextAlign>(Align::CENTER, Align::CENTER);
            d.Command<Text>(&_line, _x += 6, _y + 16);
        }

        d.Command<Fill>(ThemeT::Get().window_title_text);
        d.Command<TextAlign>(Align::LEFT, Align::CENTER);
        d.Command<Text>(&m_Name, Vec2<int>{_x + 9, _y + 16});
    }

    /**
     * Returns the menu.
     * @return menu
     */
    MenuBase& Menu() { return *m_Menu; }

    /**
     * Returns the base panel.
     * @return panel
     */
    ::Panel& Panel() { return *m_Panel; }

private:
    const bool m_Resizable;
    ::Panel* m_Panel;

    ButtonBase* m_CloseButton;
    ButtonBase* m_MaxiButton1;
    ButtonBase* m_MaxiButton2;
    ButtonBase* m_MinimButton;
    MenuBase* m_Menu;
};