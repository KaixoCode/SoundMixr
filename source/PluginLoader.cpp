#include "PluginLoader.hpp"
#include "Audio/Asio.hpp"

DynamicPlugin::~DynamicPlugin()
{
	FreeLibrary(m_Module);
}

DynamicPlugin::DynamicPlugin(const std::string& name, HMODULE h)
	: m_Module(h), m_Name(name)
{
	instfunc = reinterpret_cast<InstanceFunction>(GetProcAddress(m_Module, "NewInstance"));
}

SoundMixr::EffectBase* DynamicEffect::CreateInstance()
{
	if (instfunc)
	{
		SoundMixr::EffectBase* p = static_cast<SoundMixr::EffectBase*>(instfunc());
		p->SampleRate(Asio::SAMPLE_RATE);
		return p;
	}
	else
		return nullptr;
}

SoundMixr::GeneratorBase* DynamicGenerator::CreateInstance()
{
	if (instfunc)
	{
		SoundMixr::GeneratorBase* p = static_cast<SoundMixr::GeneratorBase*>(instfunc());
		p->SampleRate(Asio::SAMPLE_RATE);
		return p;
	}
	else
		return nullptr;
}

std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> PluginLoader::m_Effects;
std::unordered_map<std::string, std::unique_ptr<DynamicGenerator>> PluginLoader::m_Generators;

void PluginLoader::LoadPlugins()
{
	CrashLog("==============================");
	CrashLog("Loading plugins...");
	try
	{
		m_Effects.clear();
		m_Generators.clear();

		std::string path = PLUGIN_DIR;
		CrashLog("Plugin directory: " << path);

		std::filesystem::create_directory(path);

		std::filesystem::path temp = std::filesystem::absolute(std::filesystem::path(path + "/temp"));
		CrashLog("Creating temporary path: " << temp);
		if (std::filesystem::exists(temp))
		{
			CrashLog("Path already exists, removing all files.");
			std::filesystem::remove_all(temp);
		}
		std::filesystem::create_directory(temp);

		CrashLog("Making temporary directory hidden");
		BOOL result = SetFileAttributesW((LPCWSTR)temp.wstring().c_str(), GetFileAttributesW((LPCWSTR)temp.wstring().c_str()) & FILE_ATTRIBUTE_HIDDEN);

		CrashLog("Looping plugin directory:");
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_regular_file())
			{
				if (!entry.path().extension().compare(".dll"))
				{
					CrashLog("-----------------------------");
					CrashLog("Found DLL entry: " << entry.path());
					std::filesystem::path absentry = std::filesystem::absolute(entry.path());
					CrashLog("Absolute path: " << absentry);
					std::string name = entry.path().stem().string();
					CrashLog("Extracted name: " << name);
					std::filesystem::path newp = temp.string() + "/" + name + ".dll";
					CrashLog("Copy to temporary file at: " << newp);
					std::filesystem::copy_file(absentry, newp);

					CrashLog("Loading temporary file as HMODULE.");
					std::string loadpath = std::filesystem::absolute(newp).string();
					HMODULE module = LoadLibrary((LPCTSTR)loadpath.c_str());
					CrashLog("HMODULE: " << module);
					if (!module)
						continue;

					VersionFunction versfunc = reinterpret_cast<VersionFunction>(GetProcAddress(module, "Version"));
					CrashLog("Contains version function: " << versfunc);
					int version = -1;
					if (versfunc)
						version = versfunc();
					CrashLog("Plugin version: " << version);

					TypeFunction typefunc = reinterpret_cast<TypeFunction>(GetProcAddress(module, "Type"));
					CrashLog("Contains type function: " << typefunc);
					int type = -1;
					if (typefunc)
						type = typefunc();
					if (type == EFFECT)
						CrashLog("Plugin type: Effect");
					else if (type == GENERATOR)
						CrashLog("Plugin type: Generator");
					else						
						CrashLog("Plugin type: -1");

					if (version == VERSION)
					{
						if (type == EFFECT)
							m_Effects.emplace(name, std::make_unique<DynamicEffect>(name, module));
						else if (type == GENERATOR)
							m_Generators.emplace(name, std::make_unique<DynamicGenerator>(name, module));
					} else
					{
						CrashLog("Plugin version is incompatible with this version of SoundMixr");
					}
				}
			}
		}
	}
	catch (std::exception e)
	{
		CrashLog(e.what());
		CrashLog("Something went wrong during Plugin loading.");
	}

	CrashLog("Loaded all effects:");
	std::stringstream ss;
	bool first = true;
	for (auto& i : m_Effects)
		if (!first) ss << ", ", first = false; else ss << i.first;
	CrashLog(ss.str());
}

std::unordered_map<std::string, std::unique_ptr<DynamicEffect>>& PluginLoader::Effects()
{
	return m_Effects;
}

std::unordered_map<std::string, std::unique_ptr<DynamicGenerator>>& PluginLoader::Generators()
{
	return m_Generators;
}

//SoundMixr::EffectBase* EffectLoader::CreateInstance(const std::string& name)
//{
//	return m_Effects[name]->CreateInstance();
//}
