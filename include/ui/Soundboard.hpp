#pragma once
#include "pch.hpp"

namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

class SoundboardButton : public Button<G::Menu, BT::Normal>
{
public:
	SoundboardButton()
		: Button<G::Menu, BT::Normal>([&] { PlayFile(); }, "")
	{ };

	float GetLevel(int channel)
	{
		if (m_SampleNum < 0)
			return 0;

		int curSample = std::floor(m_SampleNum * m_MultiplicationFactor);

		if (curSample == m_File.getNumSamplesPerChannel())
		{
			m_SampleNum = -1;
			return 0;
		}

		if (m_SampleNum >= 0 && channel == 0)
			m_SampleNum++;

		return m_File.samples[channel][curSample];
	}

	void LoadFile(const std::string& path) { m_Filepath = path; }
	void PlayFile()
	{
		if (m_File.getNumSamplesPerChannel() > 0) 
		{
			// A file is already loaded, play it if it isn't playing
			if (m_SampleNum < 0)
				m_SampleNum = 0;
			else
				m_SampleNum = -1;
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
			m_MultiplicationFactor = (m_File.getSampleRate() / 48000.0);

			// Set the name of the button to the filename
			std::filesystem::directory_entry loadedFile{ fileNameStr };
			ButtonBase::Name(loadedFile.path().filename().string());
		}
	};

private:
	std::string m_Filepath;
	AudioFile<double> m_File;
	int m_SampleNum = -1;
	float m_MultiplicationFactor = 1.0F;
};

class Soundboard : public Frame
{
public:
	Soundboard();

	float GetLevel(int);
	
private:
	std::map<int, AudioFile<double>> m_files;
	std::vector<SoundboardButton*> m_Buttons;
};