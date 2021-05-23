#pragma once
#include "pch.hpp"
#include "ui/Frame.hpp"
#include <Midi.hpp>

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;
class Soundboard;

/**
 * Soundboard button, stores the audio file associated with it.
 */
class SoundboardButton : public Button<MuteButton, BT::Normal>
{
	using Parent = Button<MuteButton, BT::Normal>;
public:

	/**
	 * Constructor.
	 */
	SoundboardButton();

	/**
	 * Get the level in the given channel.
	 * @param channel channel
	 * @return level
	 */
	float GetLevel(int channel);

	/**
	 * Load an audio file given the path.
	 * @param path path
	 * @param filename filename
	 */
	void LoadFile(const std::string& path, const std::string& filename);

	/**
	 * Play the opened file, if no file is open it will open
	 * a dialog. If forceopen is true it will always open dialog
	 * if midi is true it will never open a dialog.
	 * @param forceOpen force new file dialog
	 * @param midi midi triggered, so don't open file dialog
	 */
	void PlayFile(bool forceOpen = false, bool midi = false);

	/**
	 * Show the RightClickMenu.
	 */
	void ShowMenu();

	/**
	 * Rename the button.
	 */
	void Rename();

	/**
	 * Remove the file from the button.
	 */
	void RemoveFile();

	/**
	 * Remove the linked hotkey.
	 */
	void RemoveHotKey();

	void Update(const Vec4<int>&) override;
	void Render(CommandCollection&) override;

	operator nlohmann::json();
	void operator = (const nlohmann::json& json);

private:
	std::string m_Filepath;

	AudioFile<double> m_File;

	int m_SampleNum = -1,
	    m_MaxSamples = -1,
	    m_HotkeyId = -1;

	bool m_MidiLinking = false,
		m_HotkeyLinking = false;
	
	float m_MultiplicationFactor = 1.0F;

	SMXRTextBox& m_Name;

	::Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;

	Vec3<int> m_MidiLink {-1, -1, -1};

	Key m_Hotkey = -1;

	// Midi events.
	Midi::EventStorage _1{ Midi::Get() += [this](Midi::Event::NoteOn& a)
	{
		if (m_MidiLinking)
		{
			m_MidiLink = { a.note, a.channel, a.device };
			m_MidiLinking = false;
		}

		if (m_MidiLink == Vec3<int>{ a.note, a.channel, a.device })
			Active(true), PlayFile(false, true);
	} };

	Midi::EventStorage _2{ Midi::Get() += [this](Midi::Event::NoteOff& a)
	{
		if (m_MidiLink == Vec3<int>{ a.note, a.channel, a.device })
			Active(false);
	} };
};

/**
 * Soundboard singleton.
 */
class Soundboard : public SoundMixrFrame
{
public:
	Soundboard();

	float GetLevel(int);
	void Save();
	void Load();

	void Update(const Vec4<int>& v) override
	{
		m_SubP->Background(ThemeT::Get().window_frame);
		SoundMixrFrame::Update(v);
	}

	static inline Soundboard& Get() 
	{ 
		return *instance;
	}
	
private:
	std::map<int, AudioFile<double>> m_files;
	std::vector<SoundboardButton*> m_Buttons;
	::Panel* m_SubP;

	static Soundboard* instance;
	friend class Controller;
};