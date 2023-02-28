#include "ui.hpp"

#include "object.hpp"
#include "sdlwrapper/sdlwrapper.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
UI::UI(sdl::Surface& target) :
    target(target)
{
    spdlog::debug("UI initialization");
}

UI::~UI() = default;

void UI::render()
{
    keyHandler.check();

    /* for (auto& object : objects)
    {
        SDL_Rect coords({(int16_t)object->x, (int16_t)object->y, 0, 0}); // TODO: usage of raw SDL type outside wrapper
        object->render(target, coords);
    } */
}
}
