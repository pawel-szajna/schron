#pragma once

#include "common_types.hpp"

#include <initializer_list>

struct SDL_Vertex;

namespace sdl
{
class Vertices
{
public:

    Vertices(const std::initializer_list<Vertex>& source);
    ~Vertices();

    [[nodiscard]] int size() const;
    [[nodiscard]] const SDL_Vertex* data() const;

private:

    int count;
    SDL_Vertex* vertices;
};
} // namespace sdl
