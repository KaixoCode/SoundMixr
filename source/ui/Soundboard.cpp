#include "ui/Soundboard.hpp"
#include "FileDialog.hpp"
#include "audio/Asio.hpp"

SoundboardButton::SoundboardButton(Soundboard& soundboard)
	: Parent([&] {}, ""), m_Name(Emplace<SMXRTextBox>()), m_Soundboard(&soundboard)
{
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
	m_Name.Visible(false);

	m_Name.Listener() += [this](Event::Unfocused& e)
	{
		Enable();
		m_Name.Editable(false);
		m_Name.Visible(false);
	};

	m_Name.Listener() += [this](Event::KeyTyped& e)
	{
		if (e.key == Key::ENTER && Focused())
		{
			Event e{ Event::Type::Unfocused };
			m_Name.AddEvent(e);
			m_Name.Focused(false);
			m_Name.Hide();
			m_FocusedComponent = nullptr;

		};
	};

	Listener() += [this](Event::Unfocused& e)
	{
		// Stop linking if the button gets unfocussed
		m_MidiLinking = false;
		m_HotkeyLinking = false;
	};

	Listener() += [this](Event::KeyPressed& e)
	{
		if (m_HotkeyLinking && !(e.key == Key::CONTROL || e.key == Key::SHIFT || e.key == Key::ALT))
		{
			RemoveHotKey();
			Key k = e;
			m_Hotkey = k;
			m_HotkeyLinking = false;
			m_HotkeyId = m_Soundboard->AddHotKey(k, [this] { PlayFile(false, true); });
		}
	};
}

void SoundboardButton::Update(const Vec4<int>& v)
{
	m_Name.Position({ X(), Y() +(Height() - m_Name.Height()) / 2 });
	m_Name.Width(Width());
	m_Name.Height(25);
	Parent::Update(v);
	m_Name.Update(v);
}

void SoundboardButton::Render(CommandCollection& d)
{
	Parent::Render(d);
	m_Name.Render(d);
}

void SoundboardButton::RemoveFile()
{
	m_Name.Content("");
	m_SampleNum = -1;
	m_MaxSamples = -1;
	m_MultiplicationFactor = 1.0F;
}

void SoundboardButton::RemoveHotKey()
{
	if (m_HotkeyId != -1)
		m_Soundboard->RemoveHotKey(m_HotkeyId);
	m_Hotkey = -1;
	m_HotkeyId = -1;
}

void SoundboardButton::ShowMenu()
{
	m_Menu.Clear();
	m_Menu.ButtonSize({ 180, 20 });
	if (m_Name.Content().empty())
	{
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, "Soundboard").Disable();
		m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { PlayFile(true); }, "New Sound...");
		m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
	}
	else
	{
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([] {}, m_Name.Content()).Disable();
		m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { Rename(); }, "Rename Button");
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { PlayFile(true); }, "New Sound...");
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { RemoveFile(); }, "Remove Sound");
		m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);
	}

	// Midi linking
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_MidiLinking, m_MidiLink.x == -1 ? "Link Midi" :
		"Linked: " + std::to_string(m_MidiLink.x) + ":" + std::to_string(m_MidiLink.y) + ":" + std::to_string(m_MidiLink.z));
	if (m_MidiLink.x == -1)
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { m_MidiLink = { -1, -1, -1 }; }, "Remove Midi Link").Disable();
	else
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { m_MidiLink = { -1, -1, -1 }; }, "Remove Midi Link");

	m_Menu.Emplace<MenuDivider>(180, 1, 0, 2);

	// Hotkey linking
	m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Toggle>>(&m_HotkeyLinking, m_Hotkey == -1 ? "Link Hotkey" :
		"Linked: " + m_Hotkey.ToString());
	if (m_Hotkey == -1)
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { RemoveHotKey(); }, "Remove Hotkey").Disable();
	else
		m_Menu.Emplace<Button<SoundMixrGraphics::Menu, ButtonType::Normal>>([this] { RemoveHotKey(); }, "Remove Hotkey");

	RightClickMenu::Get().Open(&m_Menu);
}

void SoundboardButton::Rename()
{
	Disable();
	m_Name.Show();
	m_Name.Editable(true);
	m_Name.Displayer().Container().Select({0, (int)m_Name.Content().length()});
	m_FocusedComponent = &m_Name;
	m_Name.Focused(true);
}

float SoundboardButton::GetLevel(int channel)
{
	if (m_SampleNum < 0 || m_MaxSamples < 0)
		return 0;

	int curSample1 = std::ceil(m_SampleNum * m_MultiplicationFactor);
	int curSample2 = std::floor(m_SampleNum * m_MultiplicationFactor);

	if (curSample1 == m_MaxSamples)
	{
		m_SampleNum = -1;
		return 0;
	}

	if (m_SampleNum >= 0 && channel == 1)
		m_SampleNum++;
	if (curSample1 == curSample2)
		return m_File.samples[channel][curSample1];
	else
		return 0.5 * (m_File.samples[channel][curSample1] + m_File.samples[channel][curSample2]);
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
		m_MaxSamples = m_File.getNumSamplesPerChannel();
		}).detach();
}

void SoundboardButton::PlayFile(bool forceOpen, bool dontOpen)
{
	if (dontOpen && m_MaxSamples < 0)
		return;

	if (m_MaxSamples > 0 && !forceOpen)
	{
		m_MultiplicationFactor = (m_File.getSampleRate() / (double)Asio::SAMPLE_RATE);
		// A file is already loaded, play it if it isn't playing
		//if (m_SampleNum < 0)
			m_SampleNum = 0;
		//else
		//	m_SampleNum = -1;
	}
	else
	{
		std::string fileNameStr = FileDialog::OpenFile("WAV Files (*.wav)\0*.wav\0");
		std::filesystem::directory_entry loadedFile{ fileNameStr };

		LoadFile(fileNameStr, loadedFile.path().filename().string());
	}
};

Soundboard::Soundboard()
	: SoundMixrFrame(WindowData("Soundboard", Vec2<int> { 300, 300 }, false, false, true, false, true, true, nullptr))
{
	Panel().Layout<Layout::Grid>(1, 1, 8, 8);
	m_SubP = &Panel().Emplace<::Panel>();
	m_SubP->Layout<Layout::Grid>(4, 4, 8, 8);

	for (int i = 0; i < 16; i++)
		m_Buttons.push_back(&m_SubP->Emplace<SoundboardButton>(*this));	
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

			// Load all the buttons
			auto _data = _json.at("data");
			int index = 0;
			for (auto& cur : _data) {
				*m_Buttons[index] = cur;
				index++;
			}
		}
		catch (std::exception& e) { _error = true; }

		if (_error) {
			LOG("Failed to load the soundboard");
		}

		_in.close();
	}
}