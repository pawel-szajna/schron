#pragma once

#include "wrapped.hpp"

#include <cstdint>
#include <string>

struct SDL_Window;

namespace sdl
{
/**
 * @class Window
 * @brief An OS window.
 *
 * Wraps the SDL_Window object.
 */
class Window : public Wrapped<SDL_Window>
{
public:

    /**
     * @brief Creates a Window.
     * @param title Window title caption.
     * @param width
     * @param height
     * @param flags Window flags.
     */
    Window(const std::string& title, int width, int height, uint32_t flags);
    ~Window();

    void setTitle(const std::string& title) noexcept;

};
}