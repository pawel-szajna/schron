#pragma once

#include "wrapped.hpp"

#include "common_types.hpp"

#include <optional>

struct SDL_Renderer;

namespace sdl
{
class Texture;
class Window;

class Renderer : public Wrapped<SDL_Renderer>
{
public:

    explicit Renderer(Window& window, int index = -1, uint32_t flags = 0);
    ~Renderer();

    void clear();
    void copy(Texture& texture,
              std::optional<Rectangle> source = std::nullopt,
              std::optional<Rectangle> target = std::nullopt);
    void present() noexcept;
};
}
