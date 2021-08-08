#include "Controller.hpp"

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(
	HINSTANCE hInstance,     /* [input] handle to current instance */
	HINSTANCE hPrevInstance, /* [input] handle to previous instance */
	LPSTR lpCmdLine,         /* [input] pointer to command line */
	int nCmdShow             /* [input] show state of window */
)
#endif
{
	try
	{
		// Run SoundMixr
		Controller::Get().Run();
	}
	catch (...)
	{
		// Crash!! Exit normally and hope for the best.
		auto& a = Controller::Get();
		a.SaveSettings();
		a.m_Audio->SaveRouting();
		Soundboard::Get().Save();
		a.m_Audio->CloseDevice();
		a.m_Gui.Clear();
	}

	std::filesystem::create_directory("./logs");
	std::time_t result = std::time(nullptr);
	std::stringstream name;
	name << "log";
	name << std::put_time(std::localtime(&result), "%Y%m%d_%H%M%S");
	auto path = std::filesystem::absolute("./logs/" + name.str() + ".txt");
	std::ofstream of{ path };
	of << CrashLogger::crashlog.str();
	of.flush();
	of.close();

	return 0;
}