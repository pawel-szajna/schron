#pragma once

#include "key_handler.hpp"

#include <memory>
#include <vector>

namespace sdl
{
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

    void add(std::unique_ptr<Object>&& object);
    void clear();

    void render();

    KeyHandler keyHandler{};

private:

    sdl::Renderer& renderer;
    std::vector<std::unique_ptr<Object>> objects;
};
}
