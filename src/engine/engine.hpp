#pragma once

#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"

#include <array>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace game
{
struct Position;
}

namespace sdl
{
class Surface;
}

namespace world
{
class Level;
class Sector;
class Wall;
}

namespace engine
{
constexpr auto TEXTURE_WIDTH{120};
constexpr auto TEXTURE_HEIGHT{120};

class Engine
{
    struct SectorRenderParams
    {
        int id;
        int leftXBoundary, rightXBoundary;
        int depth{0};
        double offsetX{0}, offsetY{0}, offsetZ{0}, offsetAngle{0};
    };

public:

    Engine(world::Level& level);

    void frame(const game::Position& player);
    void draw(sdl::Surface& target);

private:

    void renderWall(const world::Sector& sector,
                    const world::Wall& wall,
                    const game::Position& player);
    void line(int x, int yStart, int yEnd, int color);
    void flushBuffer();

    std::unordered_map<int, std::array<Uint32, TEXTURE_WIDTH * TEXTURE_HEIGHT>> textures;
    std::array<std::array<uint32_t, c::renderHeight>, c::renderWidth> buffer{};
    std::array<int, c::renderWidth> limitTop{}, limitBottom{};
    std::queue<SectorRenderParams> renderQueue{};

    sdl::Surface view;
    world::Level& level;
};
}
