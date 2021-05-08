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
		_json["midi"] = nlohmann::json::array();
		_json["midi"][0] = m_MidiLink.x;
		_json["midi"][1] = m_MidiLink.y;
		_json["midi"][2] = m_MidiLink.z;

		return _json;
	}

	void operator = (const nlohmann::json& json)
	{
		this->LoadFile(json.at("filepath"), json.at("filename"));

		m_MidiLink = { json.at("midi")[0], json.at("midi")[1], json.at("midi")[2] };
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
	Vec3<int> m_MidiLink {-1, -1, -1};


	Midi::EventStorage _1{ Midi::Get() += [this](Midi::Event::NoteOn& a)
	{
		if (m_MidiLinking)
		{
			m_MidiLink = { a.channel, a.note, a.device };
			m_MidiLinking = false;
		}

		if (m_MidiLink == Vec3<int>{ a.channel, a.note, a.device })
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