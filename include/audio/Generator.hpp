#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"
#include "PluginLoader.hpp"
#include "FileDialog.hpp"
#include "audio/Plugin.hpp"

class Generator : public Plugin
{
public:
	Generator(SoundMixr::GeneratorBase* generator) : generator(generator), Plugin(generator) {};

	/**
	 * Generate the next sample given the input sample and channel.
	 * @param s input sample
	 * @param c channel
	 */
	double Generate(int c) { return generator->Generate(c); };

	void Update(const Vec4<int>& v) override { Plugin::Update(v); };
	void Render(CommandCollection& d) override { Plugin::Render(d); };

	SoundMixr::GeneratorBase* generator;
};