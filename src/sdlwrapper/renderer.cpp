#include "renderer.hpp"

#include "common_types.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "util/format.hpp"
#include "vertices.hpp"
#include "window.hpp"

#include <optional>
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace sdl
{
Renderer::Renderer(Window& window, int index, uint32_t flags)
{
    assign(SDL_CreateRenderer(*window, index, flags),
           "SDL renderer");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(wrapped, &info);
    spdlog::debug("Created renderer, type: {}, flags: {}", info.name, info.flags);
}

Renderer::Renderer(Surface& surface)
{
    assign(SDL_CreateSoftwareRenderer(*surface),
           "SDL software renderer");
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
    SDL_SetRenderDrawColor(wrapped, 0, 0, 0, 0);
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
        throw std::runtime_error{std::format("could not copy mainTexture: {}", SDL_GetError())};
    }
}

void Renderer::present() noexcept
{
    SDL_RenderPresent(wrapped);
}

Texture Renderer::createTexture(Texture::Access access, int width, int height)
{
    return {*this, access, width, height};
}

void Renderer::renderLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor(wrapped, r, g, b, a);
    SDL_RenderDrawLine(wrapped, x1, y1, x2, y2);
}

void Renderer::renderGeometry(const std::vector<Vertex>& vertices)
{
    SDL_SetRenderDrawBlendMode(wrapped, SDL_BLENDMODE_BLEND);
    if (vertices.size() < 3)
    {
        throw std::invalid_argument{"polygon has not enough edges"};
    }
    for (auto vertex = 1u; vertex < vertices.size() - 1; ++vertex)
    {
        auto converted = sdl::Vertices({vertices[0], vertices[vertex], vertices[vertex + 1]});
        auto result = SDL_RenderGeometry(wrapped, nullptr, converted.data(), converted.size(), nullptr, 0);
        if (result != Success)
        {
            throw std::runtime_error{std::format("render failed: {}", SDL_GetError())};
        }
    }
}
}
