#pragma once

#include "fonts.hpp"
#include "sdlwrapper/event.hpp"

#include <memory>
#include <vector>

namespace sdl
{
class Font;
class Renderer;
}

namespace ui
{
class Object;

class UI
{
public:

    explicit UI(sdl::Renderer& renderer);
    ~UI();

    int add(std::unique_ptr<Object>&& object);
    Object& get(int id);
    void clear();

    void event(const sdl::event::Event& event);
    void render();

    Fonts fonts{};

private:

    sdl::Renderer& renderer;
    std::vector<std::unique_ptr<Object>> objects;
    int counter{0};
};
}
