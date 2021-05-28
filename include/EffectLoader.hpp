#pragma once
#include "pch.hpp"

/**
 * Define a custom directory where the effects are located based on if it
 * it is debug mode or not.
 */
#ifdef _DEBUG
#define EFFECTS_DIR "./debug"
#else
#define EFFECTS_DIR "./effects"
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
 * Wrapper for an effect DLL, stores the HMODULE and the CreateInstance function pointer
 * that was loaded from the DLL.
 */
class DynamicEffect
{
public:

	/**
	 * Make sure we FreeLibrary the HMODULE
	 */
	~DynamicEffect();

	/**
	 * Constructor
	 * @param name effect name
	 * @param h HMODULE
	 */
	DynamicEffect(const std::string& name, HMODULE h);

	/**
	 * Creates an instance of the effect using the InstanceFunction 
	 * loaded from the HMODULE
	 * @return pointer to effect in DLL
	 */
	SoundMixr::EffectBase* CreateInstance();

private:
	InstanceFunction instfunc;
	std::string m_Name;
	HMODULE m_Module;
};

/**
 * Singleton that loads all DynamicEffects using the DLLs found in
 * the EFFECTS_DIR.
 */
class EffectLoader
{
public:

	/**
	 * Loads all effect DLLs located in EFFECTS_DIR.
	 */
	static void LoadEffects();

	/**
	 * Returns an unordered map with the effect names as keys and a unique_ptr to
	 * the DynamicEffect as the value.
	 * @return effects
	 */
	static std::unordered_map<std::string, std::unique_ptr<DynamicEffect>>& Effects();

	/**
	 * Creates an instance of the effect with the given name
	 * @param name name of the effect to create an instance of
	 * @return EffectBase
	 */
	static SoundMixr::EffectBase* CreateInstance(const std::string& name);

private:
	static std::unordered_map<std::string, std::unique_ptr<DynamicEffect>> m_Effects;
};