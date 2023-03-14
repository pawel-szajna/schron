#include "renderer.hpp"

#include "common_types.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "util/constants.hpp"
#include "util/format.hpp"
#include "vertices.hpp"
#include "window.hpp"

#include <optional>
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace sdl
{
Renderer::Renderer(Window& window, uint32_t flags)
{
    assign(SDL_CreateRenderer(*window, nullptr, flags),
           "SDL renderer");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(wrapped, &info);
    spdlog::debug("Created renderer, type: {}, flags: {}", info.name, info.flags);
    int renderWidth, renderHeight;
    SDL_GetRenderOutputSize(wrapped, &renderWidth, &renderHeight);
    if (renderWidth != c::windowWidth or renderHeight != c::windowHeight)
    {
        auto scaleWidth = static_cast<float>(renderWidth) / static_cast<float>(c::windowWidth);
        auto scaleHeight = static_cast<float>(renderHeight) / static_cast<float>(c::windowHeight);
        spdlog::debug("Requested resolution: {}x{}, actual resolution: {}x{}, applying {}x/{}x scaling factor",
                      c::windowWidth, c::windowHeight, renderWidth, renderHeight, scaleWidth, scaleHeight);
        if (SDL_SetRenderScale(wrapped, scaleWidth, scaleHeight) != Success)
        {
            spdlog::warn("Could not apply render scale factor of {}x/{}x, possible UI scaling issues: {}",
                         scaleWidth, scaleHeight, SDL_GetError());
        }
    }
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

void Renderer::copy(Texture& texture, std::optional<FRectangle> source, std::optional<FRectangle> target)
{
    auto result = SDL_RenderTexture(wrapped,
                                    *texture,
                                    source.has_value() ? reinterpret_cast<const SDL_FRect*>(&(*source)) : nullptr,
                                    target.has_value() ? reinterpret_cast<const SDL_FRect*>(&(*target)) : nullptr);
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
    setColor(r, g, b, a);
    SDL_RenderLine(wrapped, x1, y1, x2, y2);
}

void Renderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor(wrapped, r, g, b, a);
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

void Renderer::renderRectangle(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    setColor(r, g, b, a);
    SDL_FRect fRect{(float)x, (float)y, (float)width, (float)height};
    SDL_RenderFillRect(wrapped, &fRect);
}
}
