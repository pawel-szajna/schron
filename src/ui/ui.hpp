#pragma once

#include "key_handler.hpp"
#include "fonts.hpp"

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

    void render();

    KeyHandler keyHandler{};
    Fonts fonts{};

private:

    sdl::Renderer& renderer;
    std::vector<std::unique_ptr<Object>> objects;
    int counter{0};
};
}
