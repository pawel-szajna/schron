#pragma once

#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace
{
struct GatheredSector;
}

namespace game
{
class Position;
}

namespace sdl
{
class Surface;
}

namespace world
{
class Level;
class Light;
class Sector;
class Sprite;
class Wall;
} // namespace world

namespace engine
{
class LightPoint
{
public:

    double r, g, b;

    LightPoint& operator+=(const LightPoint& other)
    {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    LightPoint& operator*=(double scalar)
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    friend LightPoint operator+(LightPoint lhs, const LightPoint& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    friend LightPoint operator*(LightPoint lhs, double rhs)
    {
        lhs *= rhs;
        return lhs;
    }

    friend LightPoint operator*(double lhs, LightPoint rhs)
    {
        rhs *= lhs;
        return rhs;
    }
};

class LightMap
{
public:

    int width, height;
    std::vector<LightPoint> map;
};

class OffsetLightMap : public LightMap
{
public:

    double x, y;
};

class Lighting
{
public:

    using TextureGetter  = std::function<sdl::Surface&(const std::string&)>;
    using LightAdder     = std::function<void(const world::Light&, const world::Sector&)>;
    using LightPredicate = std::function<bool()>;

    Lighting(const world::Level& level, TextureGetter textureGetter);
    ~Lighting();

    LightMap prepareWallMap(const world::Sector& sector, const world::Wall& wall, const game::Position& player);
    std::pair<OffsetLightMap, OffsetLightMap> prepareSurfaceMap(const world::Sector& sector,
                                                                const game::Position& player);
    LightPoint calculateWallLighting(double mapX, double mapY, const LightMap& lightMap);
    LightPoint calculateSurfaceLighting(double mapX, double mapY, const OffsetLightMap& lightMap);
    LightPoint
    calculateSpriteLighting(const world::Sector& sector, const world::Sprite& sprite, const game::Position& player);

private:

    void addLight(LightPoint& target,
                  const world::Sector& sector,
                  const world::Light& light,
                  const game::Position& player,
                  double worldX,
                  double worldY,
                  double worldZ);
    void gatherLights(std::queue<GatheredSector>& gatheringQueue,
                      double mapX,
                      double mapY,
                      const game::Position& player,
                      const world::Light& playerLight,
                      const LightAdder& lightAdder,
                      const LightPredicate& lightPredicate);

    const world::Level& level;
    TextureGetter getTexture;
};
} // namespace engine
