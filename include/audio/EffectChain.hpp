#pragma once
#include "pch.hpp"

class EffectChain
{
public:

	float Apply(float s) 
	{
		return s;
	}


	virtual operator nlohmann::json() 
	{
		return {};
	};

	virtual void operator=(const nlohmann::json&) {};
};