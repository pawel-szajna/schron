#pragma once

#include "common_types.hpp"
#include "wrapped.hpp"

#include <optional>

struct SDL_Surface;

namespace sdl
{
class Texture;

class Surface : public Wrapped<SDL_Surface>
{
public:

    Surface(int width, int height, int alphaMask = 0xff000000);
    ~Surface();

    void empty();

    void render(Texture& target);
    void render(Surface& target,
                std::optional<Rectangle> where = std::nullopt);
    void renderPart(Surface& target,
                    Rectangle subset,
                    std::optional<Rectangle> where = std::nullopt);
    void renderStretched(Surface& target);

    uint32_t* pixels();
    uint32_t& operator[](int index);

    const int width, height;
};
}