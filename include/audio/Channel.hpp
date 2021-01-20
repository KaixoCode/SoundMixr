#pragma once
#include "pch.hpp"

class Channel : public moodycamel::ConcurrentQueue<double>
{
	int id;



};