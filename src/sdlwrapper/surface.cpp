#include "surface.hpp"

#include "texture.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <spdlog/spdlog.h>

namespace sdl
{
Surface::Surface(int width, int height, int alphaMask) :
    width(width),
    height(height)
{
    assign(SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, alphaMask),
           "SDL surface");
}

Surface::Surface(const std::string& filename)
{
    auto image = IMG_Load(filename.c_str());
    if (image == nullptr)
    {
        throw std::runtime_error{std::format("image {} loading failed: {}", filename, IMG_GetError())};
    }
    auto format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);

    assign(SDL_ConvertSurface(image, format, 0),
           std::format("SDL surface from loaded image: {}", SDL_GetError()));

    SDL_FreeFormat(format);
    SDL_FreeSurface(image);

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
        SDL_FreeSurface(wrapped);
    }
}

void Surface::empty()
{
    SDL_FillRect(wrapped, nullptr, 0);
}

void Surface::render(Texture& target)
{
    target.update(static_cast<uint32_t*>(wrapped->pixels));
}

void Surface::renderStretched(Surface& target)
{
    SDL_BlitScaled(wrapped, nullptr, *target, nullptr);
}

void Surface::render(Surface& target, std::optional<Rectangle> where)
{
    SDL_BlitSurface(wrapped,
                    nullptr,
                    *target,
                    where.has_value() ? reinterpret_cast<SDL_Rect*>(&(*where)) : nullptr);
}

void Surface::renderPart(Surface& target, Rectangle subset, std::optional<Rectangle> where)
{
    SDL_BlitSurface(wrapped,
                    reinterpret_cast<SDL_Rect*>(&subset),
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
