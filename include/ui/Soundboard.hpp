#pragma once
#include "pch.hpp"

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

class SoundboardButton : public Button<G::Menu, BT::Normal>
{
public:
	SoundboardButton()
		: Button<G::Menu, BT::Normal>([&] { PlayFile(); }, "")
	{ };

	void LoadFile(const std::string& path) { m_Filepath = path; }
	void PlayFile()
	{
		if (m_File.getNumSamplesPerChannel() > 0) 
		{
			// A file is already loaded, play it
			m_SampleNum = 0;
		}
		else 
		{
			// Load a file
			OPENFILENAME ofn;
			char fileName[MAX_PATH] = "";
			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFilter = "WAV Files (*.wav)\0*.wav\0";
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "";

			std::string fileNameStr;

			if (GetOpenFileName(&ofn))
				fileNameStr = fileName;

			m_Filepath = fileNameStr;
			m_File.load(fileNameStr);

			// Set the name of the button to the filename
			std::filesystem::directory_entry loadedFile{ fileNameStr };
			ButtonBase::Name(loadedFile.path().filename().string());
		}
	};

private:
	std::string m_Filepath;
	AudioFile<double> m_File;
	int m_SampleNum = -1;
};

class Soundboard : public Frame
{
public:
	Soundboard();

	float GetLevel(int);
	
private:
	std::map<int, AudioFile<double>> m_files;
};