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
	try
	{
		m_Effects.clear();
		m_Generators.clear();

		std::string path = PLUGIN_DIR;

		std::filesystem::create_directory(path);

		std::filesystem::path temp = std::filesystem::absolute(std::filesystem::path(path + "/temp"));
		if (std::filesystem::exists(temp))
			std::filesystem::remove_all(temp);

		std::filesystem::create_directory(temp);
		BOOL result = SetFileAttributesW((LPCWSTR)temp.wstring().c_str(), GetFileAttributesW((LPCWSTR)temp.wstring().c_str()) & FILE_ATTRIBUTE_HIDDEN);

		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (entry.is_regular_file())
			{
				if (!entry.path().extension().compare(".dll"))
				{
					std::filesystem::path absentry = std::filesystem::absolute(entry.path());
					std::string name = entry.path().stem().string();
					std::filesystem::path newp = temp.string() + "/" + name + ".dll";
					std::filesystem::copy_file(absentry, newp);

					std::string loadpath = std::filesystem::absolute(newp).string();
					HMODULE module = LoadLibrary((LPCTSTR)loadpath.c_str());
					if (!module)
						continue;

					VersionFunction versfunc = reinterpret_cast<VersionFunction>(GetProcAddress(module, "Version"));
					int version = -1;
					if (versfunc)
						version = versfunc();

					TypeFunction typefunc = reinterpret_cast<TypeFunction>(GetProcAddress(module, "Type"));
					int type = -1;
					if (typefunc)
						type = typefunc();


					LOG(loadpath);
					LOG(module);

					if (version == VERSION)
					{
						if (type == EFFECT)
							m_Effects.emplace(name, std::make_unique<DynamicEffect>(name, module));
						else if (type == GENERATOR)
							m_Generators.emplace(name, std::make_unique<DynamicGenerator>(name, module));
					}
				}
			}
	}
	catch (std::exception e)
	{
		LOG(e.what());
		LOG("Something went wrong during Plugin loading.");
	}
	for (auto& i : m_Effects)
		LOG(i.first);
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
