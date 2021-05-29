#pragma once
#include "pch.hpp"
#include "audio/Effect.hpp"
#include "audio/EffectChain.hpp"
#include "ui/Graphics.hpp"

class GeneratorChain : public EffectChain
{
public:

	void Update(const Vec4<int>& v) override { EffectChain::Update(v); };
	void Render(CommandCollection& d) override { EffectChain::Render(d); };
};