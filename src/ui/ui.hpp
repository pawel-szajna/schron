#pragma once

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

private:

    sdl::Surface& target;
    std::vector<Object> objects{};
};
}
