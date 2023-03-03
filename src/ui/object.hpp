#pragma once

#include "sdlwrapper/renderer.hpp"

namespace ui
{
class Object
{
public:

    virtual ~Object() = default;
    virtual void render(sdl::Renderer& target) = 0;

};
}
