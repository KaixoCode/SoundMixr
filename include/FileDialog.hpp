#pragma once
#include "pch.hpp"

/**
 * Simple wrapper for the Windows file dialog.
 */
class FileDialog
{
public:
	static inline std::string OpenFile()
	{
		// Load a file
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0";
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = "";

		std::string fileNameStr;

		if (GetOpenFileName(&ofn))
			fileNameStr = fileName;

		return fileNameStr;
	}

	static inline std::string SaveFile()
	{
		// Load a file
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0";
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
		ofn.lpstrDefExt = "";

		std::string fileNameStr;

		if (GetSaveFileName(&ofn))
			fileNameStr = fileName;

		return fileNameStr;
	}


};