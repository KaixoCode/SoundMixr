#include "ui/Soundboard.hpp"
#include <FileDialog.hpp>

SoundboardButton::SoundboardButton()
	: Button<G::Menu, BT::Normal>([&] {}, ""), m_Name(Emplace<SMXRTextBox>())
{
	// Initialise the right click menu
	m_Menu.ButtonSize({ 180, 20 });
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([&] { Rename(); }, "Rename");

	// Add an event listener for mouse click events
	m_Listener += [this](Event::MousePressed& e)
	{
		if (e.button == Event::MouseButton::LEFT)
			PlayFile();
		else if (e.button == Event::MouseButton::MIDDLE)
			PlayFile(true);		// Force selecting a new file
		else if (e.button == Event::MouseButton::RIGHT)
			ShowMenu();
	};

	m_Name.AlignLines(Align::CENTER);
	m_Name.Background({ 0, 0, 0, 0 });
	m_Name.TextColor({ 255, 255, 255, 255 });
	m_Name.Editable(false);

	m_Name.Listener() += [this](Event::Unfocused& e)
	{
		Enable();
		m_Name.Editable(false);
	};

	m_Name.Listener() += [this](Event::KeyTyped& e)
	{
		if (e.key == Key::ENTER && Focused())
		{
			Event e{ Event::Type::Unfocused };
			m_Name.AddEvent(e);
			m_Name.Focused(false);
			m_FocusedComponent = nullptr;
		};
	};
};

void SoundboardButton::Update(const Vec4<int>& v)
{
	m_Name.Position({ X(), Y() +(Height() + m_Name.Height()) / 2 });
	m_Name.Width(Width());
	m_Name.Height(25);
	Button<G::Menu, BT::Normal>::Update(v);
}

void SoundboardButton::ShowMenu()
{
	if (!m_Name.Content().empty())
		RightClickMenu::Get().Open(&m_Menu);
}

void SoundboardButton::Rename()
{
	Disable();
	m_Name.Editable(true);
	m_Name.Displayer().Container().Select({0, (int)m_Name.Content().length()});
	m_FocusedComponent = &m_Name;
	m_Name.Focused(true);
}

float SoundboardButton::GetLevel(int channel)
{
	if (m_SampleNum < 0 || m_MaxSamples < 0)
		return 0;

	int curSample = std::floor(m_SampleNum * m_MultiplicationFactor);

	if (curSample == m_MaxSamples)
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
	// Check if the file exists
	if (!std::filesystem::exists(path))
	{
		if (path != "")
			LOG("File doesn't exist anymore, skipping file: " + path);
		return;
	}

	// Set the name of the button to the filename
	m_Name.Content(filename);

	m_Filepath = path;

	// Create a new thread as not to delay the main thread
	std::thread([&] {
		m_MaxSamples = 0;
		m_File.load(m_Filepath);
		m_MultiplicationFactor = (m_File.getSampleRate() / 48000.0);
		m_MaxSamples = m_File.getNumSamplesPerChannel();
		}).detach();
}

void SoundboardButton::PlayFile(bool forceOpen)
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
		std::string fileNameStr = FileDialog::OpenFile("WAV Files (*.wav)\0*.wav\0");
		std::filesystem::directory_entry loadedFile{ fileNameStr };

		LoadFile(fileNameStr, loadedFile.path().filename().string());
	}
};

Soundboard::Soundboard()
	: SoundMixrFrame(WindowData("Soundboard", Vec2<int> { 1000, 500 }, false, false, true, false, true, true, nullptr))
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
	for (int i = 0; i < m_Buttons.size(); i++) {
		totalLevel += m_Buttons[i]->GetLevel(channel);
	}

	return totalLevel;
}

void Soundboard::Save()
{
	try {
		LOG("Saving Soundboard");
		nlohmann::json _json;
		_json["data"] = nlohmann::json::array();

		// Soundboard sounds
		for (auto& _btn : m_Buttons)
			_json["data"] += *_btn;

		// Save the soundboard data
		std::ofstream _out;

		LOG(std::filesystem::current_path().string());
		_out.open("./settings/soundboarddata.json", std::ios::out);
		if (_out.is_open())
		{
			_out << std::setw(4) << _json;
			_out.close();
		}
		else
			LOG("Couldn't open the soundboard save file.");
	}
	catch (const std::exception& ex) {
		LOG("Failed to save SoundBoard.");
	}
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
		catch (std::exception& e) { _error = true; }

		if (_error) {
			LOG("Failed to load the soundboard");

			// Initialise an empty soundboard
			Init();
		}

		_in.close();
	}
}