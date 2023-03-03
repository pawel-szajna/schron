#pragma once

#include "common_types.hpp"
#include "texture.hpp"
#include "wrapped.hpp"

#include <optional>
#include <vector>

struct SDL_Renderer;

namespace sdl
{
class Surface;
class Vertices;
class Window;

class Renderer : public Wrapped<SDL_Renderer>
{
public:

    explicit Renderer(Window& window, int index = -1, uint32_t flags = 0);
    explicit Renderer(Surface& surface);
    ~Renderer();

    void clear();
    void copy(Texture& texture,
              std::optional<Rectangle> source = std::nullopt,
              std::optional<Rectangle> target = std::nullopt);
    void present() noexcept;

    void renderGeometry(const std::vector<Vertex>& vertices);

    Texture createTexture(Texture::Access access, int width, int height);
};
}
