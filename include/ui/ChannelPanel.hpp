#pragma once
#include "pch.hpp"
#include "audio/Audio.hpp"
#include "ui/EndpointPanel.hpp"

class ChannelPanel : public Panel
{
public:

	ChannelPanel(std::vector<Audio*>* audios)
		: Panel(),
		m_IDeviceButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_IDeviceMenu);}, "Input Device", Vec2<int>{200, 24})),
		m_ODeviceButton(Emplace<Button<NiceButton, BT::Normal>>([&]() { RightClickMenu::Get().Open(&m_ODeviceMenu);}, "Output Device", Vec2<int>{200, 24})),
		m_Close(Emplace<Button<CloseButton, BT::Normal>>([&] { m_ClosePanel = true; LOG("CLOSE"); }, "", Vec2<int>{24, 24}))
	{
		// Styling of panel
		Background(Color{ 33, 33, 33, 255 });
		Height(48);

		// Location of buttons
		m_IDeviceButton.Position(Vec2<int>{20, Height() - 24});
		m_ODeviceButton.Position(Vec2<int>{20, Height() - 48});
		m_Close.Position(Vec2<int>{220, Height() - 24});
		m_Close.Visible(false);

		LOG(audios);
		// Add all available devices to the menu in other thread
		if (audios == nullptr)
			return;

		int listid1 = BT::List::NewKey();
		int listid2 = BT::List::NewKey();

		int l = audios->size();
		for (int k = 0; k < l; k++)
		{
			auto& _audio = audios->at(k);
			Device& _d = _audio->Device();
			std::string name = _d.name;
			int i = name.find_first_of('(');
			if (i < name.size()) name.resize(i, ' ');
			LOG(name);

			// Only inputs or outputs depending on type
			if (_d.inputChannels != 0)
				m_IDeviceMenu.Emplace<Button<BG::Menu, BT::List>>([&] { _audio->Connect(m_Id, m_Channel); }, name, Vec2<int>{220, 20}, listid1);

			if (_d.outputChannels != 0)
				m_ODeviceMenu.Emplace<Button<BG::Menu, BT::List>>([&] { _audio->Connect(m_Id, m_Channel); }, name, Vec2<int>{220, 20}, listid2);
		}
	}

	bool Close() { return m_ClosePanel; }

private:
	Button<NiceButton, BT::Normal>
		& m_IDeviceButton,
		& m_ODeviceButton;

	Button<CloseButton, BT::Normal>& m_Close;

	Menu<MenuGraphics::Vertical, MenuType::Normal>
		m_IDeviceMenu,
		m_ODeviceMenu;

	Channel m_Channel;

	static inline int idcounter = 0;
	int m_Id = idcounter++;

	bool m_SetSamplerate = false,
		m_ClosePanel = false;
};