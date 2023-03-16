#pragma once

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
struct Position;
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
}

namespace engine
{
struct LightPoint
{
    double r, g, b;

    LightPoint& operator+=(const LightPoint& other) { r += other.r; g += other.g; b += other.b; return *this; }
    LightPoint& operator*=(double scalar) { r *= scalar; g *= scalar; b *= scalar; return *this; }

    friend LightPoint operator+(LightPoint lhs, const LightPoint& rhs) { lhs += rhs; return lhs; }
    friend LightPoint operator*(LightPoint lhs, double rhs) { lhs *= rhs; return lhs; }
    friend LightPoint operator*(double lhs, LightPoint rhs) { rhs *= lhs; return rhs; }
};

class Engine
{
    struct HorizontalLightMap
    {
        double x, y;
        int width, height;
        std::vector<std::pair<LightPoint, LightPoint>> map;
    };

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

private:

    sdl::Surface& getTexture(const std::string& name);

    void renderWall(const world::Sector& sector,
                    const world::Wall& wall,
                    const game::Position& player,
                    double angleSin, double angleCos,
                    const HorizontalLightMap& lightMap);
    void renderCeilingAndFloor(const world::Sector& sector,
                               const game::Position& player,
                               int x, int wallTop, int wallBottom,
                               double distance,
                               double ceilingY, double floorY,
                               double angleSin, double angleCos,
                               const HorizontalLightMap& lightMap);
    void renderSprites(const world::Sector& sector,
                       const game::Position& player,
                       double angleSin, double angleCos);
    void lightedLine(int x,
                     int wallTop, int wallBottom,
                     int visibleWallTop, int visibleWallBottom,
                     sdl::Surface& texture, int textureX,
                     double distance,
                     const LightPoint& lightTop,
                     const LightPoint& lightBottom);

    HorizontalLightMap prepareSurfaceMap(const world::Sector& sector);
    LightPoint calculateSurfaceLighting(double mapX, double mapY,
                                        bool isCeiling,
                                        const HorizontalLightMap& lightMap);

    std::map<std::string, sdl::Surface> textures{};
    std::map<std::string, sdl::Surface> sprites{};
    std::array<int, c::renderWidth> limitTop{}, limitBottom{};
    std::array<sdl::Pixel, c::renderWidth * c::renderHeight> buffer{};
    std::array<double, c::renderWidth * c::renderHeight> zBuffer{};
    std::queue<SectorRenderParams> renderQueue{};

    sdl::Renderer& renderer;
    sdl::Texture view;

    world::Level& level;
};
}
