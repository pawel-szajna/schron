#include "window.hpp"

#include <SDL.h>

namespace sdl
{
Window::Window(const std::string& title, int width, int height, uint32_t flags)
{
    assign(SDL_CreateWindow(title.c_str(),
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            width,
                            height,
                            flags),
           "SDL window");
}

Window::~Window()
{
    if (wrapped != nullptr)
    {
        SDL_DestroyWindow(wrapped);
    }
}

void Window::setTitle(const std::string& title) noexcept
{
    SDL_SetWindowTitle(wrapped, title.c_str());
}
}
