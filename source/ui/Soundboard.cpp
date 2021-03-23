#include "ui/Soundboard.hpp"

SoundboardButton::SoundboardButton()
    : Button<G::Menu, BT::Normal>([&] { PlayFile(); }, "")
{ };

float SoundboardButton::GetLevel(int channel)
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

void SoundboardButton::LoadFile(const std::string& path, const std::string& filename) 
{ 
    m_Filepath = path;
    m_Filename = filename;
}

void SoundboardButton::PlayFile()
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

		// Set the name of the button to the filename
		std::filesystem::directory_entry loadedFile{ fileNameStr };
		ButtonBase::Name(loadedFile.path().filename().string());

		m_Filepath = fileNameStr;
		m_Filename = loadedFile.path().filename().string();
		m_File.load(fileNameStr);
		m_MultiplicationFactor = (m_File.getSampleRate() / 48000.0);
	}
};

Soundboard::Soundboard()
    : Frame("Soundboard", 1000, 500, true, false, false)
{
	Init();
}

void Soundboard::Init() {

	namespace G = ButtonGraphics; namespace BT = ButtonType; namespace MG = MenuGraphics; namespace MT = MenuType;

	auto& _panel = this->Panel();
	this->Icon(IDI_ICON1);
	_panel.Clear();

	_panel.Layout<Layout::Grid>(4, 4, 8, 8);

	for (int i = 0; i < 16; i++) {
		m_Buttons.push_back(&_panel.Emplace<SoundboardButton>());
	}
}

float Soundboard::GetLevel(int channel)
{
    float totalLevel = 0;
    for (int i = 0; i < 16; i++) {
        totalLevel += m_Buttons[i]->GetLevel(channel);
    }

    return totalLevel;
}

void Soundboard::Save()
{
	LOG("Saving Soundboard");
	nlohmann::json _json;
	_json["data"] = nlohmann::json::array();

	// Soundboard sounds
	for (auto& _btn : m_Buttons) {
		_json["data"].push_back(*_btn);
	}

	// Save the soundboard data
	std::ofstream _out;
	_out.open("./settings/soundboarddata-1.json");
	_out << std::setw(4) << _json;
	_out.close();
}

void Soundboard::Load()
{
	LOG("Loading Soundboard");
	std::ifstream _in;
	_in.open("./settings/soundboarddata.json");

	bool _error = _in.fail();
	if (!_error) {
		try {
			nlohmann::json _json;
			_in >> _json;

			// Clear the screen
			m_Buttons.clear();
			auto& _panel = this->Panel();
			_panel.Clear();

			// Load all the buttons
			auto _data = _json.at("data");
			for (auto& cur : _data) {
				auto& curBtn = _panel.Emplace<SoundboardButton>();
				curBtn = cur;
				m_Buttons.push_back(&curBtn);
			}
		}
		catch (std::exception& e) { _error = true;  }

		if (_error) {
			LOG("Failed loading soundboard");
			Init();
		}

		_in.close();
	}
}