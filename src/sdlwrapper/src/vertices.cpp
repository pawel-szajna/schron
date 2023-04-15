#include "vertices.hpp"

#include <algorithm>
#include <SDL3/SDL.h>
#include <stdexcept>

namespace sdl
{
Vertices::Vertices(const std::initializer_list<Vertex>& source)
    : count(source.size())
{
    vertices = new SDL_Vertex[count];
    if (source.size() < 3)
    {
        throw std::invalid_argument{"not enough vertices in polygon"};
    }
    std::transform(source.begin(),
                   source.end(),
                   vertices,
                   [](const auto& v) {
                       return SDL_Vertex{{v.x, v.y}, {v.r, v.g, v.b, v.a}, {v.texX, v.texY}};
                   });
}

Vertices::~Vertices()
{
    delete[] vertices;
}

int Vertices::size() const
{
    return count;
}

const SDL_Vertex* Vertices::data() const
{
    return vertices;
}
} // namespace sdl
