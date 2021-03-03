#include <iostream>

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
#include "GuiCode/ui/window/MenuFrame.hpp"
#include "GuiCode/ui/window/SmallFrame.hpp"
#include "GuiCode/ui/window/Window.hpp"

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "portaudio.h"
#include "pa_asio.h"

#include <AudioFile.h>

#define db2lin(db) std::powf(10.0f, 0.05 * (db))
#define lin2db(lin) (20.0f * std::log10f(lin))
#define myabs(f) if (f < 0) f = -f;
