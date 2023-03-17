#include "lighting.hpp"

#include "game/player.hpp"
#include "sdlwrapper/surface.hpp"
#include "utilities.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <algorithm>
#include <cmath>
#include <queue>

#if not defined(DISABLE_PARALLELISM)
#   include <boost/iterator/counting_iterator.hpp>
#   ifdef __APPLE__
#       include <oneapi/dpl/execution>
#       include <oneapi/dpl/algorithm>
#   else
#       include <execution>
#   endif
#endif

namespace
{
constexpr double invMapRes = 32;
constexpr double mapRes = 1 / invMapRes;

struct P { double x, y; };

constexpr auto ccw(P a, P b, P c) { return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x); };
constexpr auto intersects(P a, P b, P c, P d) { return ccw(a, c, d) != ccw(b, c, d) and ccw(a, b, c) != ccw(a, b, d); };
}

namespace engine
{
Lighting::Lighting(const world::Level& level,
                   TextureGetter textureGetter) :
    level(level),
    getTexture(std::move(textureGetter))
{}

Lighting::~Lighting() = default;

LightPoint Lighting::calculateWallLighting(double mapX, double mapY, const LightMap& lightMap)
{
    auto lightMapX = (int)(mapX);
    auto lightMapY = (int)(mapY);

    auto stepX = std::clamp(mapX - lightMapX, 0.0, 1.0);
    auto stepY = std::clamp(mapY - lightMapY, 0.0, 1.0);
    auto invStepX = 1.0 - stepX;
    auto invStepY = 1.0 - stepY;

    auto& lmXcYc = lightMap.map[lightMapX + lightMapY * lightMap.width];
    auto& lmXnYc = lightMap.map[(lightMapX + 1) + lightMapY * lightMap.width];
    auto& lmXcYn = lightMap.map[lightMapX + (lightMapY + 1) * lightMap.width];
    auto& lmXnYn = lightMap.map[(lightMapX + 1) + (lightMapY + 1) * lightMap.width];

    return lmXcYc * (invStepX * invStepY / 2) +
           lmXnYc * (stepX * invStepY / 2) +
           lmXcYn * (invStepX * stepY / 2) +
           lmXnYn * (stepX * stepY / 2);
}

LightPoint Lighting::calculateSurfaceLighting(double mapX, double mapY, const OffsetLightMap& lightMap)
{
    double nearestX = std::floor(mapX * invMapRes) * mapRes;
    double nearestY = std::floor(mapY * invMapRes) * mapRes;

    auto lightMapX = std::clamp((int)((nearestX - lightMap.x) * invMapRes), 0, lightMap.width - 2);
    auto lightMapY = std::clamp((int)((nearestY - lightMap.y) * invMapRes), 0, lightMap.height - 2);

    auto stepX = std::clamp((mapX - nearestX) * invMapRes, 0.0, 1.0);
    auto stepY = std::clamp((mapY - nearestY) * invMapRes, 0.0, 1.0);
    auto invStepX = 1.0 - stepX;
    auto invStepY = 1.0 - stepY;

    auto& lmXcYc = lightMap.map[lightMapX + lightMapY * lightMap.width];
    auto& lmXnYc = lightMap.map[(lightMapX + 1) + lightMapY * lightMap.width];
    auto& lmXcYn = lightMap.map[lightMapX + (lightMapY + 1) * lightMap.width];
    auto& lmXnYn = lightMap.map[(lightMapX + 1) + (lightMapY + 1) * lightMap.width];

    return lmXcYc * (invStepX * invStepY / 2) +
           lmXnYc * (stepX * invStepY / 2) +
           lmXcYn * (invStepX * stepY / 2) +
           lmXnYn * (stepX * stepY / 2);
}

LightMap Lighting::prepareWallMap(const world::Sector& sector, const world::Wall& wall, const game::Position& player)
{
    auto lightMapWidth = (int)(std::hypot(wall.xEnd - wall.xStart, wall.yEnd - wall.yStart) * invMapRes) + 1;
    auto lightMapHeight = (int)(invMapRes * (sector.ceiling - sector.floor)) + 1;

    double stepSize = 1.0 / (double)lightMapWidth;
    double stepX = (wall.xEnd - wall.xStart) * stepSize;
    double stepY = (wall.yEnd - wall.yStart) * stepSize;
    double stepZ = (sector.ceiling - sector.floor) / (double)lightMapHeight;

    LightMap lightMap{lightMapWidth, lightMapHeight, {(size_t)(lightMapWidth * lightMapHeight), {0, 0, 0}}};

#if defined(DISABLE_PARALLELISM)
    for (int j = 0; j < lightMapHeight; ++j)
#else
    std::for_each(std::execution::par,
                  boost::counting_iterator(0),
                  boost::counting_iterator(lightMapHeight),
                  [&](int j)
#endif
    {
        double z = sector.ceiling - stepZ * j;
        for (int i = 0; i < lightMapWidth; ++i)
        {
            double x = stepX * i + wall.xStart;
            double y = stepY * i + wall.yStart;
            LightPoint lightPoint{};

            addLight(lightPoint, sector,
                     world::Light{player.x, player.y, player.z, 0.3, 0.3, 0.375},
                     player, x, y, z);

            for (const auto& light : sector.lights)
            {
                addLight(lightPoint, sector, light, player, x, y, z);
            }

            for (const auto& w : sector.walls)
            {
                if (not w.portal)
                {
                    continue;
                }

                double nx1 = w.xStart, nx2 = w.xEnd, ny1 = w.yStart, ny2 = w.yEnd;
                for (const auto& light : level.sector(w.portal->sector).lights)
                {
                    // TODO: floor/ceiling collision
                    if ((x == nx1 or x == nx2) and (y == nx1 or y == ny2))
                    {
                        addLight(lightPoint, sector, light, player, x, y, z);
                        continue;
                    }
                    auto side1 = (nx1 - x) * (light.y - y) - (ny1 - y) * (light.x - x);
                    auto side2 = (x - nx2) * (light.y - ny2) - (y - ny2) * (light.x - nx2);
                    if (side1 > 0 and side2 > 0)
                    {
                        addLight(lightPoint, sector, light, player, x, y, z);
                    }
                }
            }

            lightMap.map[i + j * lightMapWidth] = lightPoint;
        }
#if defined(DISABLE_PARALLELISM)
    }
#else
    });
#endif

    return lightMap;
}

std::pair<OffsetLightMap, OffsetLightMap> Lighting::prepareSurfaceMap(const world::Sector& sector,
                                                                      const game::Position& player)
{
    auto wallLeftX =   *std::min_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.xStart < b.xStart; });
    auto wallRightX =  *std::max_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
    auto wallTopY =    *std::min_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.yStart < b.yStart; });
    auto wallBottomY = *std::max_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; });

    auto leftX = wallLeftX.xStart - mapRes;
    auto rightX = wallRightX.xEnd + 2 * mapRes;
    auto topY = wallTopY.yStart - mapRes;
    auto bottomY = wallBottomY.yEnd + 2 * mapRes;

    int width = (int)((rightX - leftX) * invMapRes) + 1;
    int height = (int)((bottomY - topY) * invMapRes) + 1;

    OffsetLightMap lightMapCeiling{{width, height, {(size_t)(width * height), {0, 0, 0}}}, leftX, topY};
    OffsetLightMap lightMapFloor{{width, height, {(size_t)(width * height), {0, 0, 0}}}, leftX, topY};

#if defined(DISABLE_PARALLELISM)
    for (int y = 0; y < height; ++y)
#else
    std::for_each(std::execution::par,
                  boost::counting_iterator(0),
                  boost::counting_iterator(height),
                  [&](int y)
#endif
    {
        for (int x = 0; x < width; ++x)
        {
            LightPoint top{ 0.0, 0.0, 0.0 };
            LightPoint bottom{ 0.0, 0.0, 0.0 };

            auto mapX = leftX + mapRes * x;
            auto mapY = topY + mapRes * y;

            auto playerLight = world::Light{ player.x, player.y, player.z, 0.1, 0.1, 0.125 };

            if (player.sector == sector.id)
            {
                addLight(top, sector, playerLight, player, mapX, mapY, sector.ceiling);
                addLight(bottom, sector, playerLight, player, mapX, mapY, sector.floor);
            }

            for (const auto& light : sector.lights)
            {
                addLight(top, sector, light, player, mapX, mapY, sector.ceiling);
                addLight(bottom, sector, light, player, mapX, mapY, sector.floor);
            }

            for (const auto& wall : sector.walls)
            {
                if (not wall.portal)
                {
                    continue;
                }

                const auto& neighbour = level.sector(wall.portal->sector);
                double nx1 = wall.xStart, nx2 = wall.xEnd, ny1 = wall.yStart, ny2 = wall.yEnd;
                if (player.sector == neighbour.id)
                {
                    addLight(top, sector, playerLight, player, mapX, mapY, sector.ceiling);
                    addLight(bottom, sector, playerLight, player, mapX, mapY, sector.floor);
                }
                for (const auto& light : neighbour.lights)
                {
                    if (x <= 1 or y <= 1 or x >= width - 2 or y >= height - 2)
                    {
                        if ((nx1 == mapX and nx2 == mapX) or (ny1 == mapY and ny2 == mapY))
                        {
                            addLight(top, sector, light, player, mapX, mapY, sector.ceiling);
                            addLight(bottom, sector, light, player, mapX, mapY, sector.floor);
                            continue;
                        }
                    }
                    // TODO: floor/ceiling collision
                    auto side1 = (nx1 - mapX) * (light.y - mapY) - (ny1 - mapY) * (light.x - mapX);
                    auto side2 = (mapX - nx2) * (light.y - ny2) - (mapY - ny2) * (light.x - nx2);
                    if (side1 > 0 and side2 > 0)
                    {
                        addLight(top, sector, light, player, mapX, mapY, sector.ceiling);
                        addLight(bottom, sector, light, player, mapX, mapY, sector.floor);
                    }
                }
            }

            lightMapCeiling.map[x + y * width] = top;
            lightMapFloor.map[x + y * width] = bottom;
        }
#if defined(DISABLE_PARALLELISM)
    }
#else
    });
#endif

    return std::make_pair(std::move(lightMapCeiling), std::move(lightMapFloor));
}

LightPoint Lighting::calculateSpriteLighting(const world::Sector& sector,
                                             const world::Sprite& sprite,
                                             const game::Position& player)
{
    LightPoint lightPoint{};

    auto addLight = [&lightPoint, &sprite](const auto& light)
    {
        double deltaX = sprite.x - light.x;
        double deltaY = sprite.y - light.y;
        double deltaZ = sprite.z - light.z;

        double distanceFactor = 1 / (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

        lightPoint += distanceFactor * LightPoint{light.r, light.g, light.b};
    };

    addLight(world::Light{player.x, player.y, player.z, 0.3, 0.3, 0.375});

    for (const auto& light : sector.lights)
    {
        addLight(light);
    }

    for (const auto& w : sector.walls)
    {
        if (not w.portal)
        {
            continue;
        }

        double nx1 = w.xStart, nx2 = w.xEnd, ny1 = w.yStart, ny2 = w.yEnd;

        for (const auto& light : level.sector(w.portal->sector).lights)
        {
            auto side1 = (nx1 - sprite.x) * (light.y - sprite.y) - (ny1 - sprite.y) * (light.x - sprite.x);
            auto side2 = (sprite.x - nx2) * (light.y - ny2) - (sprite.y - ny2) * (light.x - nx2);
            if (side1 > 0 and side2 > 0)
            {
                addLight(light);
            }
        }
    }

    return lightPoint;
}

void Lighting::addLight(LightPoint& target,
                        const world::Sector& sector,
                        const world::Light& light,
                        const game::Position& player,
                        double worldX, double worldY, double worldZ)
{
    double deltaX = worldX - light.x;
    double deltaY = worldY - light.y;
    double deltaZ = worldZ - light.z;
    double distancePart = deltaX * deltaX + deltaY * deltaY;

    auto castsShadows = [](const world::Sprite& sprite) { return sprite.shadows; };

    auto playerShadow = world::Sprite{-1, "sprites/shadow", player.x, player.y, player.z, 1.0, 1.0, 0, true};

    std::vector<std::reference_wrapper<const world::Sprite>> sprites{std::ref(playerShadow)};
    std::copy_if(sector.sprites.begin(), sector.sprites.end(), std::back_inserter(sprites), castsShadows);
    for (const auto& wall : sector.walls)
    {
        if (wall.portal)
        {
            const auto& neighbour = level.sector(wall.portal->sector);
            std::copy_if(neighbour.sprites.begin(), neighbour.sprites.end(), std::back_inserter(sprites), castsShadows);
        }
    }

    for (const auto& spriteRef : sprites)
    {
        const auto& sprite = spriteRef.get();

        P a{worldX, worldY};
        P b{light.x, light.y};

        double xDiff = light.x - sprite.x;
        double yDiff = light.y - sprite.y;
        double distance = 1 / (2 * std::hypot(xDiff, yDiff));

        P c{sprite.x - yDiff * distance, sprite.y + xDiff * distance};
        P d{sprite.x + yDiff * distance, sprite.y - xDiff * distance};

        if (intersects(a, b, c, d))
        {
            auto [intersectionX, intersectionY] = intersect(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y);
            auto intersectionDistance = std::hypot(light.x - intersectionX, light.y - intersectionY);
            auto wallDistance = std::sqrt(distancePart);
            auto ratio = intersectionDistance / wallDistance;
            auto intersectionZ = light.z + deltaZ * ratio;
            auto spriteTop = sprite.z + sprite.h / 2, spriteBottom = sprite.z - sprite.h / 2;
            if (intersectionZ > spriteTop or intersectionZ <= spriteBottom)
            {
                continue;
            }

            const auto& texture = this->getTexture(sprite.texture);
            int spriteX = std::clamp((int)((intersectionX - c.x + intersectionY - c.y) * (texture.width - 1) / (d.x - c.x + d.y - c.y)), 0, texture.width - 1);
            int spriteY = std::clamp((int)((sprite.h * (spriteTop - intersectionZ) / (spriteTop - spriteBottom)) * (texture.height - 1)), 0, texture.height - 1);

            // TODO: possibly just weaken the light based on alpha channel - transparent sprites could cast proper shadows
            if ((texture.pixels()[spriteX + spriteY * texture.width] & 0xff000000) >> 24 == 0xff)
            {
                return;
            }
        }
    }

    double distanceFactor = 1 / (distancePart + deltaZ * deltaZ);
    target += distanceFactor * LightPoint{light.r, light.g, light.b};
}
}
