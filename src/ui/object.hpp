#pragma once

#include "sdlwrapper/sdlwrapper.hpp"

namespace ui
{
class Object
{
    friend class UI;

public:

    Object(int id, int x, int y);

private:

    int id;
    int x, y;
    sdl::Surface surface;
};
}