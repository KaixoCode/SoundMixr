#pragma once
#include "pch.hpp"

template<typename Graphics, typename Type>
class SliderBase : public Type
{
public:
	virtual double SliderValue() { return Value(); };
	virtual void   SliderValue(double v) { Value(v); };
	virtual std::string& ValueText() = 0;

	void Render(CommandCollection& d) override
	{
		Graphics::Render(*this, d);
	};
};

