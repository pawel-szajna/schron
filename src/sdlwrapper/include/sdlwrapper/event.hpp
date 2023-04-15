#pragma once

#include "common_types.hpp"

#include <variant>

namespace sdl
{
namespace event
{
class Quit;
class Key;
class Mouse;

using None  = std::monostate;
using Event = std::variant<None, Quit, Key, Mouse>;
} // namespace event

/**
 * @brief Checks for an yet-unhandled event.
 * @return An Event representing the first event in queue.
 *
 * This function uses SDL_PollEvent to check if there are any unhandled events.
 * If an event is found, it is wrapped using the Event variant type. In case no
 * events have been encountered, an Event value of None is used.
 */
event::Event pollEvent();
} // namespace sdl
