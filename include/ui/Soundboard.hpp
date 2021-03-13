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

	operator json()
	{
		json _json = json::object();
		_json["filepath"] = m_Filepath;
		_json["filename"] = m_Filename;

		return _json;
	}

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
	
private:
	std::map<int, AudioFile<double>> m_files;
	std::vector<SoundboardButton*> m_Buttons;
};