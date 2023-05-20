#pragma once

#include "sdlwrapper/event.hpp"

#include <memory>
#include <vector>

namespace sdl
{
class Renderer;
}

namespace ui
{
class Object;
using Widget = std::shared_ptr<Object>;
using Widgets = std::vector<Widget>;

class Object
{
public:

    virtual ~Object() = default;

    virtual void render(sdl::Renderer& target);

    virtual bool event(const sdl::event::Event& event);

    void attach(Widget child);
    void detach();
    void detachAll();

private:

    Object* parent;
    Widgets children;
};
} // namespace ui
