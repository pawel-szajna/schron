#pragma once

#include <optional>

namespace sdl::event
{
class Quit
{
};

class Key
{
public:

    enum class Direction
    {
        Up,
        Down,
    };

    Direction direction;
    int scancode;
};

class Mouse
{
public:

    int x, y;
    std::optional<int> button;
    std::optional<int> scroll;
};
}
