#pragma once
#include "pch.hpp"

/**
 * Simple FileType class for filtering the FileDialog.
 */
class FileType
{
public:
	/**
	 * Some predefined filter types.
	 */
	enum Type
	{
		JSON, TXT, WAV, MP3, AIF
	};

	/**
	 * Constructor given a predefined type.
	 * @param t Type
	 */
	FileType(Type t)
		: m_Filter(m_Conversion.at(t) + "\0")
	{}

	/**
	 * Constructor given a user defined filter.
	 * @param name name that will be displayed in the dialog
	 * @param extension the actual extension ("*.json" for example)
	 */
	FileType(const std::string& name, const std::string& extension)
		: m_Filter(name + "\0" + extension + "\0")
	{}

	/**
	 * Append another filetype.
	 * @param other filetype
	 * @returns combination of both filters
	 */
	FileType operator|(const FileType& other) const
	{
		return FileType{ m_Filter + other.m_Filter };
	}

	/**
	 * Append another filetype.
	 * @param other filetype
	 * @returns combination of both filters
	 */
	FileType operator|(const Type& other) const
	{
		return FileType{ m_Filter + m_Conversion.at(other) };
	}

	operator LPCSTR() const { return m_Filter.c_str(); }

private:
	const std::string m_Filter;

	/**
	 * Constructor given a previously defined string filter
	 * @param filter filter string
	 */
	FileType(const std::string& filter)
		: m_Filter(filter + "\0")
	{}

	// Converson from enum to string filter
	static inline const std::unordered_map<Type, std::string> m_Conversion
	{
		{ Type::JSON, { "JSON Files (*.json)\0*.json", 27 } },
		{ Type::TXT, { "TXT Files (*.txt)\0*.txt", 24 } },
		{ Type::WAV, { "WAV Files (*.wav)\0*.wav", 24 } },
		{ Type::MP3, { "MP3 Files (*.mp3)\0*.mp3", 24 } },
		{ Type::AIF, { "AIF Files (*.aif)\0*.aif", 24 } },
	};
};

/**
 * Append another filetype.
 * @param other filetype
 * @returns combination of both filters
 */
static inline FileType operator|(const FileType::Type& mine, const FileType::Type& other)
{
	FileType t = mine;
	return t | other;
}

/**
 * Simple wrapper for the Windows file dialog.
 */
class FileDialog
{
public:
	static inline std::string OpenFile(const FileType& type)
	{
		// Load a file
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = type;
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = "";

		std::string fileNameStr;

		if (GetOpenFileName(&ofn))
			fileNameStr = fileName;

		return fileNameStr;
	}

	static inline std::string SaveFile(const FileType& type)
	{
		// Load a file
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = type;
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