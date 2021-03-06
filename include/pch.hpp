#include <iostream>

#include <any>

#include "GuiCode/pch.hpp"
#include "GuiCode/Gui.hpp"
#include "GuiCode/ui/components/panels/Panel.hpp"
#include "GuiCode/ui/components/panels/ScrollPanel.hpp"
#include "GuiCode/ui/components/menus/MenuAccessories.hpp"
#include "GuiCode/ui/events/Event.hpp"
#include "GuiCode/ui/graphics/Collection.hpp"
#include "GuiCode/ui/graphics/Command.hpp"
#include "GuiCode/ui/graphics/Graphics.hpp"
#include "GuiCode/ui/window/Frame.hpp"
#include "GuiCode/ui/window/ComponentFrame.hpp"
#include "GuiCode/ui/window/SmallFrame.hpp"
#include "GuiCode/ui/window/Window.hpp"
#include "GuiCode/ui/components/text/TextComponent.hpp"
#include "GuiCode/ui/components/text/TextBox.hpp"
#include "GuiCode/ui/components/text/TextArea.hpp"

#define JSON_TRY_USER if(true)
#define JSON_CATCH_USER(exception) if(false)
#define JSON_THROW_USER(exception)                           \
    {std::clog << "Error in " << __FILE__ << ":" << __LINE__ \
               << " (function " << __FUNCTION__ << ") - "    \
               << (exception).what() << std::endl;           \
     throw exception; }

#include <nlohmann/json.hpp>

#include "portaudio.h"
#include "pa_asio.h"

#include "RtMidi.h"

#include <AudioFile.h>

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


#include "CrashLogger.hpp"

#define CrashLog(x) \
std::cout << x << std::endl, \
CrashLogger::crashlog << CrashLogger::CrashLogPrefix() << x << std::endl