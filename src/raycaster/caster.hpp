#pragma once

#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"
#include "util/position.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>

namespace sdl
{
class Surface;
}

namespace raycaster
{
constexpr auto TEXTURE_WIDTH{120};
constexpr auto TEXTURE_HEIGHT{120};

class Caster
{
public:
    Caster();

    void frame();
    void draw(sdl::Surface& target);

    void changeVisibility(double fullRange, double visibleRange);

    int addSprite(double x, double y, int texture);
    void updateSprite(int id, double x, double y, int texture);
    void removeSprite(int id);

private:
    void fadePixel(uint32_t color, double distance, uint32_t& pixel) const;

    void renderWalls(const util::Position& position);
    void renderSprites(const util::Position& position);

    void loadTexture(int id, const std::string& filename);

    double fadeStart{}, fadeEnd{};

    std::unordered_map<int, std::array<Uint32, TEXTURE_WIDTH * TEXTURE_HEIGHT>> textures;
    std::array<std::array<uint32_t, c::renderHeight>, c::renderWidth> buffer{};
    std::array<double, c::renderWidth> zBuffer{};
    sdl::Surface world;
    int dynamicSpritesCounter{};
};
}
