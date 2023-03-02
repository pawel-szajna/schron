#include "renderer.hpp"

#include "common_types.hpp"
#include "texture.hpp"
#include "util/format.hpp"
#include "window.hpp"

#include <optional>
#include <SDL.h>

namespace sdl
{
Renderer::Renderer(Window& window, int index, uint32_t flags)
{
    assign(SDL_CreateRenderer(*window, index, flags),
           "SDL renderer");
}

Renderer::~Renderer()
{
    if (wrapped != nullptr)
    {
        SDL_DestroyRenderer(wrapped);
    }
}

void Renderer::clear()
{
    auto result = SDL_RenderClear(wrapped);
    if (result != Success)
    {
        throw std::runtime_error{std::format("could not clear renderer: {}", SDL_GetError())};
    }
}

void Renderer::copy(Texture& texture, std::optional<Rectangle> source, std::optional<Rectangle> target)
{
    auto result = SDL_RenderCopy(wrapped,
                                 *texture,
                                 source.has_value() ? reinterpret_cast<SDL_Rect*>(&(*source)) : nullptr,
                                 target.has_value() ? reinterpret_cast<SDL_Rect*>(&(*target)) : nullptr);
    if (result != Success)
    {
        throw std::runtime_error{std::format("could not copy texture: {}", SDL_GetError())};
    }
}

void Renderer::present() noexcept
{
    SDL_RenderPresent(wrapped);
}
}
