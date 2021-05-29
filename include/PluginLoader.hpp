#pragma once
#include "pch.hpp"

/**
 * Define a custom directory where the effects are located based on if it
 * it is debug mode or not.
 */
#ifdef _DEBUG
#define PLUGIN_DIR "./debug"
#else
#define PLUGIN_DIR "./plugins"
#endif

/**
 * Type of the NewInstance function in the effect DLL
 */
typedef void* (__cdecl* InstanceFunction)();

/**
 * Type of the version function in the effect DLL
 */
typedef int (__cdecl* VersionFunction)();

/**
 * Type of the version function in the effect DLL
 */
typedef int(__cdecl* TypeFunction)();

/**
 * Wrapper for a plugin DLL, stores the HMODULE and the CreateInstance function pointer
 * that was loaded from the DLL.
 */
class DynamicPlugin
{
public:

	/**
	 * Make sure we FreeLibrary the HMODULE
	 */
	virtual ~DynamicPlugin();

	/**
	 * Constructor
	 * @param name plugin name
	 * @param h HMODULE
	 */
	DynamicPlugin(const std::string& name, HMODULE h);

protected:
	InstanceFunction instfunc;
	std::string m_Name;
	HMODULE m_Module;
};

/**
 * Wrapper for the Effect DLL
 */
class DynamicEffect : public DynamicPlugin
{
public:
	using DynamicPlugin::DynamicPlugin;

	/**
	 * Creates an instance of the effect using the InstanceFunction 
	 * loaded from the HMODULE
	 * @return pointer to effect in DLL
	 */
	SoundMixr::EffectBase* CreateInstance();
};

/**
 * Wrapper for the Generator DLL
 */
class DynamicGenerator : public DynamicPlugin
{
public:

	using DynamicPlugin::DynamicPlugin;

	/**
	 * Creates an instance of the generator using the InstanceFunction
	 * loaded from the HMODULE
	 * @return pointer to effect in DLL
	 */
	SoundMixr::GeneratorBase* CreateInstance();
};

/**
 * Singleton that loads all DynamicPlugins using the DLLs found in
 * the PLUGIN_DIR.
 */
class PluginLoader
{
public:

	/**
	 * Loads all plugins DLLs located in PLUGIN_DIR.
	 */
	static void LoadPlugins();

	/**
	 * Returns an unordered map with the effect names as keys and a unique_ptr to
	 * the DynamicEffect as the value.
	 * @return effects
	 */
	static std::unordered_map<std::string, std::unique_ptr<DynamicEffect>>& Effects();

	/**
	 * Returns an unordered map with the generator names as keys and a unique_ptr to
	 * the DynamicGenerator as the value.
	 * @return generators
	 */
	static std::unordered_map<std::string, std::unique_ptr<DynamicGenerator>>& Generators();

private:
	static std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> m_Effects;
	static std::unordered_map<std::string, std::unique_ptr<DynamicGenerator>> m_Generators;
};