#pragma once

#include "sdlwrapper/renderer.hpp"

namespace sdl
{
class Renderer;
}

namespace ui
{
class Object
{
public:

    virtual ~Object() = default;
    virtual void render(sdl::Renderer& target) = 0;

};
}
