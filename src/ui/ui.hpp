#pragma once

#include "key_handler.hpp"

#include <memory>
#include <vector>

namespace sdl
{
class Surface;
}

namespace ui
{
class Object;

class UI
{
public:

    UI();
    ~UI();

    void render();

    KeyHandler keyHandler{};

public: // TODO private

    std::vector<std::unique_ptr<Object>> objects;
};
}
