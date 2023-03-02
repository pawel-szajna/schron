#pragma once

#include "wrapped.hpp"

#include <string>

struct SDL_Window;

namespace sdl
{
class Window : public Wrapped<SDL_Window>
{
public:

    Window(const std::string& title, int width, int height, uint32_t flags);
    ~Window();

    void setTitle(const std::string& title) noexcept;

};
}