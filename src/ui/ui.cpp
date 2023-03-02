#include "ui.hpp"

#include "object.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
UI::UI()
{
    spdlog::debug("UI initialization");
}

UI::~UI() = default;

void UI::render()
{
    keyHandler.check();
}
}
