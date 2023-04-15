#pragma once

#include "sdlwrapper/event.hpp"
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

    virtual ~Object()                                  = default;
    virtual void render(sdl::Renderer& target)         = 0;
    virtual void event(const sdl::event::Event& event) = 0;
};
} // namespace ui
