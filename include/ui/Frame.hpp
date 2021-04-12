#pragma once
#include "pch.hpp"
#include "Graphics.hpp"

/**
 * Custom SoundMixr Frame, for easy recoloring according to theme
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
     * @param show show on creation
     * @param r resizeable
     */
    SoundMixrFrame(const std::string& name, const int width, const int height, bool hideonclose = false, bool show = true, bool r = true);

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

    void Update(const Vec4<int>& viewport) override;
    void Render(CommandCollection& d) override;

private:
    const bool m_Resizable;
    ::Panel* m_Panel;

    ButtonBase
        * m_CloseButton,
        * m_MaxiButton1,
        * m_MaxiButton2,
        * m_MinimButton;

    MenuBase* m_Menu;
};