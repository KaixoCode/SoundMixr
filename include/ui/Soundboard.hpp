#pragma once
#include "pch.hpp"
#include "ui/Frame.hpp"
#include <Midi.hpp>

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

class SoundboardButton : public Button<G::Menu, BT::Normal>
{
public:
	SoundboardButton();

	float GetLevel(int channel);
	void LoadFile(const std::string& path, const std::string& filename);
	void PlayFile(bool forceOpen = false, bool midi = false);
	void ShowMenu();
	void Rename();
	void Update(const Vec4<int>&) override;
	void RemoveFile();

	operator nlohmann::json()
	{
		nlohmann::json _json = nlohmann::json::object();
		_json["filepath"] = m_Filepath;
		_json["filename"] = m_Name.Content();

		return _json;
	}

	void operator = (const nlohmann::json& json)
	{
		this->LoadFile(json.at("filepath"), json.at("filename"));
	}

private:
	std::string m_Filepath;
	AudioFile<double> m_File;
	int m_SampleNum = -1;
	int m_MaxSamples = -1;
	bool m_MidiLinking = false;
	float m_MultiplicationFactor = 1.0F;
	SMXRTextBox& m_Name;
	::Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	Vec3<byte> m_MidiConf {-1, -1, -1};


	Midi::EventStorage _1{ Midi::Get() += [this](Midi::Event::NoteOn& a)
	{
		if (m_MidiLinking)
		{
			m_MidiConf = { a.channel, a.note, a.device };
			m_MidiLinking = false;
		}

		if (m_MidiConf == Vec3<byte> { a.channel, a.note, a.device })
			PlayFile();
	} };
};

class Soundboard : public SoundMixrFrame
{
public:
	Soundboard();

	float GetLevel(int);
	void Save();
	void Load();
	void Init();
	
private:
	std::map<int, AudioFile<double>> m_files;
	std::vector<SoundboardButton*> m_Buttons;
};