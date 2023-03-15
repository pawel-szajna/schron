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

    explicit Renderer(Window& window, uint32_t flags = 0);
    explicit Renderer(Surface& surface);
    ~Renderer();

    void clear();
    void copy(Texture& texture,
              std::optional<FRectangle> source = std::nullopt,
              std::optional<FRectangle> target = std::nullopt);
    void present() noexcept;

    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void renderGeometry(const std::vector<Vertex>& vertices);
    void renderLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void renderRectangle(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void renderBox(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    Texture createTexture(Texture::Access access, int width, int height);
};
}
