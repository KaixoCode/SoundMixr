#pragma once
#include "pch.hpp"
#include "audio/Effects.hpp"

#ifdef _DEBUG
#define EFFECTS_DIR "./debug"
#else
#define EFFECTS_DIR "./effects"
#endif

typedef void* (__stdcall* inst_func)();

class DynamicEffect
{
public:
	DynamicEffect()
	{}

	~DynamicEffect()
	{
		FreeLibrary(m_Module);
	}

	DynamicEffect(const std::string& name, HMODULE h)
		: m_Module(h), m_Name(name)
	{
		instfunc = (inst_func)GetProcAddress(m_Module, "NewInstance");
	}

	Effects::EffectBase* CreateInstance()
	{
		if (instfunc)
		{
			Effects::EffectBase* p = static_cast<Effects::EffectBase*>(instfunc());
			return p;
		}
		else 
			return nullptr;
	}

private:
	inst_func instfunc;
	std::string m_Name;
	HMODULE m_Module;
};

class EffectLoader
{
public:
	static inline void LoadEffects()
	{
		m_Effects.clear();

		std::string path = EFFECTS_DIR;
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
					
					//if (m_Effects.find(name) != m_Effects.end())
					//	continue;

					std::string loadpath = std::filesystem::absolute(newp).string();
					HMODULE module = LoadLibrary((LPCTSTR)loadpath.c_str());
					LOG(loadpath);
					LOG(module);
					m_Effects.emplace(name, std::make_unique<DynamicEffect>(name, module));
				}
			}

		for (auto& i : m_Effects)
			LOG(i.first);
	}

	static inline std::unordered_map<std::string, std::unique_ptr<DynamicEffect>>& Effects()
	{
		return m_Effects;
	}

	static inline Effects::EffectBase* CreateInstance(const std::string& name)
	{
		return m_Effects[name]->CreateInstance();
	}

public:
	static inline std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> m_Effects;
};