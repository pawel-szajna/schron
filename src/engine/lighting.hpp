#pragma once

#include <functional>
#include <string>
#include <vector>

namespace sdl
{
class Surface;
}

namespace world
{
class Level;
class Light;
class Sector;
class Wall;
}

namespace engine
{
class LightPoint
{
public:

    double r, g, b;

    LightPoint& operator+=(const LightPoint& other) { r += other.r; g += other.g; b += other.b; return *this; }
    LightPoint& operator*=(double scalar) { r *= scalar; g *= scalar; b *= scalar; return *this; }

    friend LightPoint operator+(LightPoint lhs, const LightPoint& rhs) { lhs += rhs; return lhs; }
    friend LightPoint operator*(LightPoint lhs, double rhs) { lhs *= rhs; return lhs; }
    friend LightPoint operator*(double lhs, LightPoint rhs) { rhs *= lhs; return rhs; }
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
    using TextureGetter = std::function<sdl::Surface&(const std::string&)>;

    Lighting(const world::Level& level,
             TextureGetter textureGetter);
    ~Lighting();

    LightMap prepareWallMap(const world::Sector& sector, const world::Wall& wall);
    std::pair<OffsetLightMap, OffsetLightMap> prepareSurfaceMap(const world::Sector& sector);
    LightPoint calculateWallLighting(double mapX, double mapY, const LightMap& lightMap);
    LightPoint calculateSurfaceLighting(double mapX, double mapY, const OffsetLightMap& lightMap);

private:

    void addLight(LightPoint& target,
                  const world::Sector& sector,
                  const world::Light& light,
                  double worldX, double worldY, double worldZ);

    const world::Level& level;
    TextureGetter getTexture;
};
}
