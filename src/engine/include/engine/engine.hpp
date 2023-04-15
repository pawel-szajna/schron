#pragma once

#include "lighting.hpp"
#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/surface.hpp"
#include "sdlwrapper/texture.hpp"
#include "util/constants.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <queue>
#include <unordered_set>

namespace game
{
class Position;
}

namespace sdl
{
class Renderer;
}

namespace world
{
class Level;
class Sector;
class Wall;
} // namespace world

namespace engine
{
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

    Engine(sdl::Renderer& renderer, world::Level& level);

    void frame(const game::Position& player);
    void draw();

    void preload();

private:

    sdl::Surface& getTexture(const std::string& name);

    void renderWall(const world::Sector& sector,
                    const world::Wall& wall,
                    const game::Position& player,
                    double angleSin,
                    double angleCos,
                    const OffsetLightMap& ceilingLightMap,
                    const OffsetLightMap& floorLightMap);
    void renderCeilingAndFloor(const world::Sector& sector,
                               const game::Position& player,
                               int x,
                               int wallTop,
                               int wallBottom,
                               double ceilingY,
                               double floorY,
                               double angleSin,
                               double angleCos,
                               const OffsetLightMap& ceilingLightMap,
                               const OffsetLightMap& floorLightMap);
    void renderSprites(const world::Sector& sector, const game::Position& player, double angleSin, double angleCos);
    void lightedLine(int x,
                     double xProgress,
                     int wallTop,
                     int wallBottom,
                     int visibleWallTop,
                     int visibleWallBottom,
                     sdl::Surface& texture,
                     int textureX,
                     double distance,
                     const LightMap& lightMap);

    std::map<std::string, sdl::Surface> textures{};
    std::map<std::string, sdl::Surface> sprites{};
    std::array<int, c::renderWidth> limitTop{}, limitBottom{};
    std::array<sdl::Pixel, c::renderWidth * c::renderHeight> buffer{};
    std::array<double, c::renderWidth * c::renderHeight> zBuffer{};
    std::queue<SectorRenderParams> renderQueue{};

    sdl::Renderer& renderer;
    sdl::Texture view;

    world::Level& level;

    Lighting lighting;
};

extern uint64_t lightingTime;
extern uint64_t geometryTime;
extern uint64_t spritesTime;
} // namespace engine
