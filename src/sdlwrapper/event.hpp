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

using None = std::monostate;
using Event = std::variant<None, Quit, Key, Mouse>;
}

event::Event pollEvent();
}
