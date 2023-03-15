#pragma once

#include "common_types.hpp"
#include "wrapped.hpp"

#include <optional>
#include <string>

struct SDL_Surface;

namespace sdl
{
class Texture;

class Surface : public Wrapped<SDL_Surface>
{
    friend class Font;

public:

    Surface(int width, int height);
    explicit Surface(const std::string& filename);
    explicit Surface(SDL_Surface* surfacePtr);
    Surface(Surface&& other) noexcept;
    Surface& operator=(Surface&& other) noexcept;
    ~Surface();

    void empty();

    void render(Texture& target);
    void render(Surface& target,
                std::optional<Rectangle> where = std::nullopt);

    uint32_t* pixels();
    [[nodiscard]] const uint32_t* pixels() const;
    uint32_t& operator[](int index);

    int width, height;
};
}