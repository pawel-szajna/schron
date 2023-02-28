#pragma once

#include "sdlwrapper/sdlwrapper.hpp"

namespace ui
{
class Object
{
    friend class UI;

public:

    Object(int id, int x, int y);
    virtual ~Object() = default;

    virtual void render(sdl::Surface& target, SDL_Rect coords) = 0;

private:

    int id;
    int x, y;
};
}