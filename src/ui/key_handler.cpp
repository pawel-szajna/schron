#include "key_handler.hpp"

#include "sdlwrapper/sdlwrapper.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
void KeyHandler::reset()
{
    keyBindings.clear();
}

void KeyHandler::check()
{
    for (const auto& [keycode, function] : keyBindings)
    {
        if (sdl::keyPressed(keycode))
        {
            function();
        }
    }
}

void KeyHandler::map(int keycode, KeyCallback action)
{
    spdlog::debug("Registered callback for keycode {}", keycode);
    keyBindings.emplace(keycode, std::move(action));
}

void KeyHandler::unmap(int keycode)
{
    keyBindings.erase(keycode);
}
}
