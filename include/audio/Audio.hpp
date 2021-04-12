#pragma once
#include "pch.hpp"
#include "audio/Asio.hpp"
#include "audio/EndpointChannel.hpp"
#include "audio/GeneratorChannel.hpp"
#include "ui/Graphics.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/EffectPanel.hpp"

/**
 * The main panel for displaying any audio channel and their effect chain.
 * Also used by the main Controller to open/close asio streams/devices.
 * All Channels will always be displayed in a sorted order, sorted on their ids.
 */
class Audio : public Panel
{
public:

    /**
     * Constructor.
     * Emplaces all panels and sets all panel layouts.
     */
    Audio();

    /**
     * Generate a rightclick menu given the panel of channels.
     * @param panel containing channels
     */
    void GenerateMenu(Panel& panel);

    /**
     * Get all channels.
     * @return channels
     */
    auto Channels() -> std::vector<ChannelBase*>& { return m_Channels; };

    /**
     * Get the Asio.
     * @return asio
     */
    auto Asio() -> Asio& { return m_Asio; }

    /**
     * Open the asio device with the given id.
     * @param id device id
     * @return true when successful
     */
    bool OpenDevice(int id);

    /**
     * Closes the currently opened asio device.
     * @return true when successful
     */
    bool CloseDevice();

    /**
     * Shows the asio control panel if a device is opened.
     */
    void ShowControlPanel();

    /**
     * Clear all the channels.
     */
    void Clear();

    /**
     * Load the routing of the opened asio device.
     */
    void LoadRouting();

    /**
     * Load the default routing of the asio device.
     */
    void DefaultRouting();

    /**
     * Save the current routing for the opened asio device.
     */
    void SaveRouting();

    /**
     * Sorts all the channels on their id, so ordering is always consistent
     */
    void SortChannels();

    /**
     * Update all the effects in all the channels.
     */
    void UpdateEffects();

    void Update(const Vec4<int>& v) override;
    void Render(CommandCollection& d) override;

private:
    ::Asio m_Asio;

    int m_Click = 0;

    mutable std::mutex m_Lock;

    Vec2<int> 
        m_PrevPos{ 0, 0 },
        m_PDim{ 0, 0 };

    // Rightclick menu
    Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

    // Vector of pointers to all channels
    std::vector<ChannelBase*> m_Channels;

    // Order of the components are important for initializating
    // them in the constructor.
    TextComponent& m_DeviceName;
    SMXRScrollPanel& m_ChannelScrollPanel;
    Panel& m_InputsPanel;
    VerticalMenuDivider& m_Divider;
    Panel& m_OutputsPanel;
    VerticalMenuDivider& m_Divider2;
    Panel& m_GeneratorPanel;
    EffectPanel& m_EffectPanel;

    /**
     * Callback for the Asio, processes all audio.
     */
    static int AsioCallback(const void* inputBuffer, void* outputBuffer, unsigned long nBufferFrames,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};