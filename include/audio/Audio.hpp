#pragma once
#include "pch.hpp"
#include "audio/Asio.hpp"
#include "ui/Graphics.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/EffectPanel.hpp"

/**
 * Channel Panel class, simply to make the Audio class have access to
 * the private member <code>m_Components</code>.
 */
class ChannelPanel : public Panel { friend class Audio; };

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
    auto Asio() -> ::Asio& { return m_Asio; }

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

    /**
     * Emplace a new channel.
     * @tparam Type type of channel
     * @tparam Args arguments to construct the channel
     */
    template<typename Type, typename ... Args>
    void EmplaceChannel(Args&& ... args)
    {
        auto _c = std::make_unique<Type>(std::forward<Args>(args)...);

        ChannelBase* _channel = nullptr;
        if (_c->Type() & ChannelBase::Type::Input)
            _channel = reinterpret_cast<ChannelBase*>(m_InputsPanel.m_Components.emplace_back(std::move(_c)).get());
        else if (_c->Type() & ChannelBase::Type::Output)
            _channel = reinterpret_cast<ChannelBase*>(m_OutputsPanel.m_Components.emplace_back(std::move(_c)).get());
    
        if (_channel)
           m_Channels.emplace_back(_channel);
    }

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
    SMXRTextComponent& m_DeviceName;
    SMXRScrollPanel& m_ChannelScrollPanel;
    ChannelPanel& m_InputsPanel;
    VerticalMenuDivider& m_Divider;
    ChannelPanel& m_OutputsPanel;
    EffectPanel& m_EffectPanel;

    /**
     * Callback for the Asio, processes all audio.
     */
    static void AsioCallback(Buffer<float>&, Buffer<float>&, CallbackInfo, Audio&);

    friend class Controller;
};