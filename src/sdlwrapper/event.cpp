#include "event.hpp"
#include "event_types.hpp"

#include <SDL3/SDL.h>

namespace sdl
{
namespace event
{
namespace
{
Event createKeyEvent(const SDL_Event &event)
{
    return Key{.direction = event.type == SDL_EVENT_KEY_DOWN ? Key::Direction::Down : Key::Direction::Up,
            .scancode  = event.key.keysym.scancode};
}

Event createMouseEvent(const SDL_Event &event)
{
    Mouse mouseEvent{};
    switch (event.type)
    {
        case SDL_EVENT_MOUSE_MOTION:
            mouseEvent.x = event.motion.x;
            mouseEvent.y = event.motion.y;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseEvent.x = event.button.x;
            mouseEvent.y = event.button.y;
            mouseEvent.button = event.button.button;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            mouseEvent.x = event.wheel.mouseX;
            mouseEvent.y = event.wheel.mouseY;
            mouseEvent.scroll = event.wheel.y;
    }
    return mouseEvent;
}
}
}

event::Event pollEvent()
{
    static SDL_Event event{};
    if (not SDL_PollEvent(&event))
    {
        return event::None{};
    }

    switch (event.type)
    {
    case SDL_EVENT_QUIT:
        return event::Quit{};
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        return event::createKeyEvent(event);
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_WHEEL:
        return event::createMouseEvent(event);
    }

    return event::None{};
}
}
