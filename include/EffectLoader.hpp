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
		for (auto& i : m_Effects)
			delete i, i = nullptr;

		m_Effects.clear();

		FreeLibrary(m_Module);
	}

	DynamicEffect(const std::string& name, HMODULE h)
		: m_Module(h), m_Name(name)
	{
		instfunc = (inst_func)GetProcAddress(m_Module, "NewInstance");
	}

	EffectBase* CreateInstance()
	{
		if (instfunc)
		{
			EffectBase* p = static_cast<EffectBase*>(instfunc());
			m_Effects.push_back(p);
			return p;
		}
		else 
			return nullptr;
	}

private:
	inst_func instfunc;
	std::string m_Name;
	HMODULE m_Module;
	std::vector<EffectBase*> m_Effects;
};

class EffectLoader
{
public:
	static inline void LoadEffects()
	{
		std::string path = EFFECTS_DIR;
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (entry.is_regular_file())
			{
				if (!entry.path().extension().compare(".dll"))
				{
					std::string name = entry.path().stem().string();
					//if (m_Effects.find(name) != m_Effects.end())
					//	continue;

					std::string path = std::filesystem::absolute(entry.path()).string();
					HMODULE module = LoadLibrary((LPCTSTR)path.c_str());
					LOG(path);
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

	static inline EffectBase* CreateInstance(const std::string& name)
	{
		return m_Effects[name]->CreateInstance();
	}

public:
	static inline std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> m_Effects;
};