#include "texture.hpp"

#include "renderer.hpp"
#include "util/format.hpp"

#include <SDL.h>
#include <stdexcept>

namespace sdl
{
namespace
{
int convertAccess(Texture::Access access)
{
    switch(access)
    {
    case Texture::Access::Static:
        return SDL_TEXTUREACCESS_STATIC;
    case Texture::Access::Streaming:
        return SDL_TEXTUREACCESS_STREAMING;
    case Texture::Access::Target:
        return SDL_TEXTUREACCESS_TARGET;
    default:
        throw std::invalid_argument{"invalid enum value in Texture::Access converter"};
    }
}
}

Texture::Texture(Renderer& renderer, Access access, int width, int height) :
    width(width)
{
    assign(SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, convertAccess(access), width, height),
           "SDL texture");
}

Texture::~Texture()
{
    if (wrapped != nullptr)
    {
        SDL_DestroyTexture(wrapped);
    }
}

void Texture::update(uint32_t* pixels)
{
    auto result = SDL_UpdateTexture(wrapped, nullptr, pixels, width * static_cast<int>(sizeof(uint32_t)));
    if (result != Success)
    {
        throw std::runtime_error{std::format("could not update texture: {}", SDL_GetError())};
    }
}
}
