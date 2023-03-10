#include "surface.hpp"

#include "texture.hpp"

#include <SDL3/SDL.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>

namespace sdl
{
Surface::Surface(int width, int height) :
    width(width),
    height(height)
{
    assign(SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB8888),
           "SDL surface");
}

Surface::Surface(const std::string& filename)
{
    auto image = IMG_Load(filename.c_str());
    if (image == nullptr)
    {
        throw std::runtime_error{std::format("image {} loading failed: {}", filename, IMG_GetError())};
    }
    auto format = SDL_CreatePixelFormat(SDL_PIXELFORMAT_ARGB8888);

    assign(SDL_ConvertSurface(image, format),
           std::format("SDL surface from loaded image: {}", SDL_GetError()));

    SDL_DestroyPixelFormat(format);
    SDL_DestroySurface(image);

    width = wrapped->w;
    height = wrapped->h;
}

Surface::Surface(SDL_Surface* surfacePtr)
{
    assign(surfacePtr, "surface from an externally provided pointer");
    width = wrapped->w;
    height = wrapped->h;
}

Surface::~Surface()
{
    if (wrapped != nullptr)
    {
        SDL_DestroySurface(wrapped);
    }
}

void Surface::empty()
{
    SDL_FillSurfaceRect(wrapped, nullptr, 0);
}

void Surface::render(Texture& target)
{
    target.update(static_cast<uint32_t*>(wrapped->pixels));
}

void Surface::render(Surface& target, std::optional<Rectangle> where)
{
    SDL_BlitSurface(wrapped,
                    nullptr,
                    *target,
                    where.has_value() ? reinterpret_cast<SDL_Rect*>(&(*where)) : nullptr);
}

uint32_t* Surface::pixels()
{
    return static_cast<uint32_t*>(wrapped->pixels);
}

const uint32_t* Surface::pixels() const
{
    return static_cast<const uint32_t*>(wrapped->pixels);
}

uint32_t& Surface::operator[](int index)
{
    return (pixels()[index]);
}
}
