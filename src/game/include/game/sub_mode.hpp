#pragma once

#include "sdlwrapper/event.hpp"

namespace sdl
{
class Renderer;
}

namespace game
{
class SubMode
{
public:

    virtual ~SubMode() = default;

    virtual void event(const sdl::event::Event& event) = 0;
    virtual void frame() = 0;
};
}