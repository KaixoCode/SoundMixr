#include "EffectLoader.hpp"

DynamicEffect::~DynamicEffect()
{
	FreeLibrary(m_Module);
}

DynamicEffect::DynamicEffect(const std::string& name, HMODULE h)
	: m_Module(h), m_Name(name)
{
	instfunc = reinterpret_cast<InstanceFunction>(GetProcAddress(m_Module, "NewInstance"));
}

Effects::EffectBase* DynamicEffect::CreateInstance()
{
	if (instfunc)
	{
		Effects::EffectBase* p = static_cast<Effects::EffectBase*>(instfunc());
		return p;
	}
	else
		return nullptr;
}

std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> EffectLoader::m_Effects;

void EffectLoader::LoadEffects()
{
	try
	{
		m_Effects.clear();

		std::string path = EFFECTS_DIR;

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

					LOG(loadpath);
					LOG(module);

					if (version == VERSION)
						m_Effects.emplace(name, std::make_unique<DynamicEffect>(name, module));
				}
			}
	}
	catch (std::exception e)
	{
		LOG(e.what());
		LOG("Something went wrong during Effect loading.");
	}
	for (auto& i : m_Effects)
		LOG(i.first);
}

std::unordered_map<std::string, std::unique_ptr<DynamicEffect>>& EffectLoader::Effects()
{
	return m_Effects;
}

Effects::EffectBase* EffectLoader::CreateInstance(const std::string& name)
{
	return m_Effects[name]->CreateInstance();
}