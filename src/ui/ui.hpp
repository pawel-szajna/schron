#pragma once

#include "key_handler.hpp"
#include "object.hpp"

#include <vector>

namespace sdl
{
class Surface;
}

namespace ui
{
class UI
{
public:

    UI(sdl::Surface& target);

    void render();

    KeyHandler keyHandler{};

private:

    sdl::Surface& target;
    std::vector<Object> objects{};
};
}
