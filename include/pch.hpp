#include <iostream>

#include <any>
#include <ranges>

#include "GuiCode2/pch.hpp"
#include "GuiCode2/Panel.hpp"
#include "GuiCode2/Menu.hpp"
#include "GuiCode2/Event.hpp"
#include "GuiCode2/Frame.hpp"
#include "GuiCode2/ContextMenu.hpp"
#include "GuiCode2/TextBox.hpp"
#include "GuiCode2/TextArea.hpp"

#define JSON_TRY_USER if(true)
#define JSON_CATCH_USER(exception) if(false)
#define JSON_THROW_USER(exception)                           \
    {std::clog << "Error in " << __FILE__ << ":" << __LINE__ \
               << " (function " << __FUNCTION__ << ") - "    \
               << (exception).what() << std::endl;           \
     throw exception; }

#include <nlohmann/json.hpp>

#include "Audijo/Audijo.hpp"
using namespace Audijo;

#include "RtMidi.h"

namespace AudioFile
{
#include <AudioFile.h>
}

#define _TESTLINK
#include "Base.hpp"
#include "Filters.hpp"
#include "Compressor.hpp"
#include "Oscillator.hpp"
#include "resource.h"

#define db2lin(db) std::powf(10.0f, 0.05 * (db))
#define lin2db(lin) (20.0f * std::log10f(lin))
#define myabs(f) if (f < 0) f = -f;

#define VERSION 13


using namespace GuiCode;
#include "CrashLogger.hpp"

#define CrashLog(x) \
std::cout << x << std::endl, \
CrashLogger::crashlog << CrashLogger::CrashLogPrefix() << x << std::endl