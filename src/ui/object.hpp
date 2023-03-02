#pragma once

#include "sdlwrapper/renderer.hpp"

namespace ui
{
class Object
{
    friend class UI;

public:

    Object(int id, int x, int y);
    virtual ~Object() = default;

    virtual void render(sdl::Renderer& target) = 0;

private:

    int id;
    int x, y;
};
}