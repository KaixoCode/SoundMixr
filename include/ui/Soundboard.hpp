#pragma once
#include "pch.hpp"

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

class SoundboardButton : public Button<G::Menu, BT::Normal>
{
public:
	SoundboardButton();

	float GetLevel(int channel);
	void LoadFile(const std::string& path, const std::string& filename);
	void PlayFile();

	operator nlohmann::json()
	{
		nlohmann::json _json = nlohmann::json::object();
		_json["filepath"] = m_Filepath;
		_json["filename"] = m_Filename;

		return _json;
	}

	void operator = (const nlohmann::json& json)
	{
		//std::cout << "PATH: " << json.at("filepath");
		//std::cout << "NAME: " << json.at("filename");
		this->LoadFile(json.at("filepath"), json.at("filename"));
	}

	void Close() { }

private:
	std::string m_Filepath;
	std::string m_Filename;
	AudioFile<double> m_File;
	int m_SampleNum = -1;
	float m_MultiplicationFactor = 1.0F;
};

class Soundboard : public Frame
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