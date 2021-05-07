#pragma once
#include "pch.hpp"
#include "ui/Frame.hpp"

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

class SoundboardButton : public Button<G::Menu, BT::Normal>
{
public:
	SoundboardButton();

	float GetLevel(int channel);
	void LoadFile(const std::string& path, const std::string& filename);
	void PlayFile(bool forceOpen = false);
	void ShowMenu();
	void Rename();
	void Update(const Vec4<int>&) override;

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
	float m_MultiplicationFactor = 1.0F;
	SMXRTextBox& m_Name;
	::Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
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