#pragma once
#include "pch.hpp"

struct Units
{
	struct Unit
	{
		struct Range
		{
			Vec2<float> range;
			float mult = 1;
			bool pre = false;
			bool value = true;
			std::string unit;

			inline std::string Format(float v, int decimals)
			{
				float _value = v * mult;
				char s[30];
				std::sprintf(s, (std::string("%.") + std::to_string(decimals) + "f").c_str(), _value);

				if (!value)
					return unit;

				if (pre)
					return unit + s;

				else
					return s + unit;
			}
		};

		inline std::string Format(float value, int decimals)
		{
			// Find range
			for (auto& i : ranges)
				if (i.range.InRange(value))
					return i.Format(value, decimals);

			// Otherwise just print value
			char s[30];
			std::sprintf(s, (std::string("%.") + std::to_string(decimals) + "f").c_str(), value);
			return s;
		}

		std::vector<Range> ranges;
	};

	static inline int PAN = 0;
	static inline int DECIBEL = 1;
	static inline std::map<int, Unit> units
	{
		{ PAN, {
			{
				{.range{ 0, 0 }, .value = false, .unit = "C" },
				{.range{ -50, 0 }, .mult = -1, .unit = "L" },
				{.range{ 0, 50 }, .mult = 1, .unit = "R" },
			}
		} },
		{ DECIBEL, {
			{
				{.range{ -10000, -66 }, .value = false, .unit = "-inf dB" },
				{.range{ -66, 100000 }, .unit = " dB" },
			}
		} }
	};
};