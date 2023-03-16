#include "engine.hpp"

#include "game/player.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "world/level.hpp"

#include <algorithm>
#include <cmath>

namespace engine
{
namespace
{
struct P { double x, y; };

constexpr double fovH = c::renderHeight * 0.65;
constexpr double fovV = c::renderWidth * 0.22;

constexpr double invMapRes = 64;
constexpr double mapRes = 1 / invMapRes;

constexpr auto cross(double x1, double y1, double x2, double y2) { return x1 * y2 - x2 * y1; }
constexpr auto intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    auto c1 = cross(x1, y1, x2, y2);
    auto c2 = cross(x3, y3, x4, y4);
    auto m = cross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    return std::make_pair(cross(c1, x1 - x2, c2, x3 - x4) / m, cross(c1, y1 - y2, c2, y3 - y4) / m);
}

constexpr auto shadeRgb(uint32_t pixel, double r, double g, double b)
{
    auto pR = (pixel & 0x00ff0000) >> 16;
    auto pG = (pixel & 0x0000ff00) >> 8;
    auto pB = (pixel & 0x000000ff) >> 0;
    pR = std::clamp((int)((double)pR * r), 0, (int)pR);
    pG = std::clamp((int)((double)pG * g), 0, (int)pG);
    pB = std::clamp((int)((double)pB * b), 0, (int)pB);
    return (pR << 16) | (pG << 8) | (pB << 0);
}

constexpr auto shadeRgb(uint32_t pixel, const LightPoint& color)
{
    return shadeRgb(pixel, color.r, color.g, color.b);
}
}

Engine::Engine(sdl::Renderer& renderer, world::Level& level) :
    renderer(renderer),
    view(renderer.createTexture(sdl::Texture::Access::Streaming, c::renderWidth, c::renderHeight)),
    level(level)
{
    spdlog::info("Initialized engine");
}

sdl::Surface& Engine::getTexture(const std::string& name)
{
    if (not textures.contains(name))
    {
        spdlog::debug("Texture {} not loaded yet", name);
        textures.emplace(name, std::format("res/gfx/{}.png", name));
    }

    return textures.at(name);
}

void Engine::frame(const game::Position& player)
{
    constexpr static auto renderStart = 0;
    constexpr static auto renderEnd = c::renderWidth - 1;
    constexpr static auto initialDepth = 32;

    buffer.fill(0);
    zBuffer.fill(100);

    limitTop.fill(0);
    limitBottom.fill(c::renderHeight - 1);

    renderQueue.push(SectorRenderParams{player.sector, renderStart, renderEnd, initialDepth});

    while (not renderQueue.empty())
    {
        auto id = renderQueue.front().id;

        auto renderedAngle = player.angle - renderQueue.front().offsetAngle;
        auto angleSin = std::sin(renderedAngle);
        auto angleCos = std::cos(renderedAngle);

        const world::Sector& sector = level.sector(id);

        auto fcmap = prepareSurfaceMap(sector);

        for (const auto& wall : sector.walls)
        {
            renderWall(sector, wall, player, angleSin, angleCos, fcmap);
        }

        renderSprites(sector, player, angleSin, angleCos);

        renderQueue.pop();
    }
}

Engine::HorizontalLightMap Engine::prepareSurfaceMap(const world::Sector& sector)
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

    HorizontalLightMap fcmap{.x = leftX,
            .y = topY,
            .width = width,
            .height = height};
    fcmap.map.reserve(fcmap.width * fcmap.height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            LightPoint top{0.0, 0.0, 0.0};
            LightPoint bottom{0.0, 0.0, 0.0};

            auto mapX = leftX + mapRes * x;
            auto mapY = topY + mapRes * y;

            auto addLight = [this, mapX, mapY, ceiling = sector.ceiling, floor = sector.floor, &top, &bottom, &sprites = sector.sprites](const auto& light)
            {
                bool blockedTop{}, blockedBottom{};

                double deltaX = mapX - light.x;
                double deltaY = mapY - light.y;
                double distancePart = deltaX * deltaX + deltaY * deltaY;

                for (const auto& sprite : sprites)
                {
                    if (not sprite.shadows)
                    {
                        continue;
                    }
                    P a{mapX, mapY};
                    P b{light.x, light.y};
                    double xDiff = light.x - sprite.x;
                    double yDiff = light.y - sprite.y;
                    double distance = 1 / (2 * std::hypot(xDiff, yDiff));
                    P c{sprite.x - yDiff * distance, sprite.y + xDiff * distance};
                    P d{sprite.x + yDiff * distance, sprite.y - xDiff * distance};
                    auto ccw = [](P a, P b, P c)
                    {
                        return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
                    };
                    if (ccw(a, c, d) != ccw(b, c, d) and ccw(a, b, c) != ccw(a, b, d))
                    {
                        auto [intersectionX, intersectionY] = intersect(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y);
                        auto intersectionDistance = std::hypot(light.x - intersectionX, light.y - intersectionY);
                        auto wallDistance = std::sqrt(distancePart);
                        auto ratio = intersectionDistance / wallDistance;
                        auto intersectionZCeiling = light.z + (ceiling - light.z) * ratio;
                        auto intersectionZFloor = light.z + (floor - light.z) * ratio;

                        const auto& texture = this->getTexture(sprite.texture);
                        int spriteX = (int)((intersectionX - c.x) * (texture.width - 1) / (d.x - c.x));
                        int spriteYFloor = (int)(intersectionZCeiling * (texture.height - 1));
                        int spriteYCeiling = (int)(intersectionZFloor * (texture.height - 1));

                        blockedTop = (texture.pixels()[spriteX + spriteYCeiling * texture.width] & 0xff000000) >> 24 == 0xff;
                        blockedBottom = (texture.pixels()[spriteX + spriteYFloor * texture.width] & 0xff000000) >> 24 == 0xff;

                        if (blockedTop and blockedBottom)
                        {
                            return;
                        }
                    }
                }

                double deltaZTop = ceiling - light.z;
                double deltaZBottom = floor - light.z;

                if (not blockedTop)
                {
                    double distanceFactorTop = 1 / (distancePart + deltaZTop * deltaZTop);
                    top += distanceFactorTop * LightPoint{ light.r, light.g, light.b };
                }
                if (not blockedBottom)
                {
                    double distanceFactorBottom = 1 / (distancePart + deltaZBottom * deltaZBottom);
                    bottom += distanceFactorBottom * LightPoint{ light.r, light.g, light.b };
                }
            };

            for (const auto& light : sector.lights)
            {
                addLight(light);
            }

            for (const auto& wall : sector.walls)
            {
                if (not wall.portal)
                {
                    continue;
                }

                const auto& neighbour = level.sector(wall.portal->sector);
                double nx1 = wall.xStart, nx2 = wall.xEnd, ny1 = wall.yStart, ny2 = wall.yEnd;
                for (const auto& light : neighbour.lights)
                {
                    if (x <= 1 or y <= 1 or x >= width - 2 or y >= height - 2)
                    {
                        if ((nx1 == mapX and nx2 == mapX) or (ny1 == mapY and ny2 == mapY))
                        {
                            addLight(light);
                            continue;
                        }
                    }
                    // TODO: floor/ceiling collision
                    auto side1 = (nx1 - mapX) * (light.y - mapY) - (ny1 - mapY) * (light.x - mapX);
                    auto side2 = (mapX - nx2) * (light.y - ny2) - (mapY - ny2) * (light.x - nx2);
                    if (side1 > 0 and side2 > 0)
                    {
                        addLight(light);
                    }
                }
            }

            fcmap.map.emplace_back(top, bottom);
        }
    }

    return fcmap;
}

void Engine::renderWall(const world::Sector& sector,
                        const world::Wall& wall,
                        const game::Position& player,
                        double angleSin, double angleCos,
                        const HorizontalLightMap& lightMap)
{
    const auto& renderParameters = renderQueue.front();

    std::vector<std::pair<LightPoint, LightPoint>> lightPoints;
    auto lightPointsCount = (int)(std::hypot(wall.xEnd - wall.xStart, wall.yEnd - wall.yStart) * invMapRes) + 1;
    double stepSize = 1.0 / (double)lightPointsCount;
    double stepX = (wall.xEnd - wall.xStart) * stepSize;
    double stepY = (wall.yEnd - wall.yStart) * stepSize;
    lightPoints.reserve(lightPointsCount);
    for (int i = 0; i < lightPointsCount; ++i)
    {
        double x = stepX * i + wall.xStart;
        double y = stepY * i + wall.yStart;
        LightPoint light1{}, light2{};

        auto addLight = [this, x, y, ceiling = sector.ceiling, floor = sector.floor, &light1, &light2, &sprites = sector.sprites](const auto& light)
        {
            double deltaX = x - light.x;
            double deltaY = y - light.y;
            double distancePart = deltaX * deltaX + deltaY * deltaY;

            bool blockedTop{false}, blockedBottom{false};

            for (const auto& sprite : sprites)
            {
                if (not sprite.shadows)
                {
                    continue;
                }
                P a{x, y};
                P b{light.x, light.y};
                double xDiff = light.x - sprite.x;
                double yDiff = light.y - sprite.y;
                double distance = 1 / (2 * std::hypot(xDiff, yDiff));
                P c{sprite.x - yDiff * distance, sprite.y + xDiff * distance};
                P d{sprite.x + yDiff * distance, sprite.y - xDiff * distance};
                auto ccw = [](P a, P b, P c)
                {
                    return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
                };
                if (ccw(a, c, d) != ccw(b, c, d) and ccw(a, b, c) != ccw(a, b, d))
                {
                    auto [intersectionX, intersectionY] = intersect(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y);
                    auto intersectionDistance = std::hypot(light.x - intersectionX, light.y - intersectionY);
                    auto wallDistance = std::sqrt(distancePart);
                    auto ratio = intersectionDistance / wallDistance;
                    auto intersectionZCeiling = light.z + (ceiling - light.z) * ratio;
                    auto intersectionZFloor = light.z + (floor - light.z) * ratio;

                    const auto& texture = this->getTexture(sprite.texture);
                    int spriteX = (int)((intersectionX - c.x) * (texture.width - 1) / (d.x - c.x));
                    int spriteYCeiling = (int)(intersectionZCeiling * (texture.height - 1));
                    int spriteYFloor = (int)(intersectionZFloor * (texture.height - 1));

                    blockedTop = (texture.pixels()[spriteX + spriteYCeiling * texture.width] & 0xff000000) >> 24 == 0xff;
                    blockedBottom = (texture.pixels()[spriteX + spriteYFloor * texture.width] & 0xff000000) >> 24 == 0xff;

                    if (blockedTop and blockedBottom)
                    {
                        return;
                    }
                }
            }

            double deltaZTop = ceiling - light.z;
            double deltaZBottom = floor - light.z;

            if (not blockedTop)
            {
                double distanceFactorTop = 1 / (distancePart + deltaZTop * deltaZTop);
                light1 += distanceFactorTop * LightPoint{ light.r, light.g, light.b };
            }
            if (not blockedBottom)
            {
                double distanceFactorBottom = 1 / (distancePart + deltaZBottom * deltaZBottom);
                light2 += distanceFactorBottom * LightPoint{ light.r, light.g, light.b };
            }
        };

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
                // TODO: floor/ceiling collision
                if ((x == nx1 or x == nx2) and (y == nx1 or y == ny2))
                {
                    addLight(light);
                    continue;
                }
                auto side1 = (nx1 - x) * (light.y - y) - (ny1 - y) * (light.x - x);
                auto side2 = (x - nx2) * (light.y - ny2) - (y - ny2) * (light.x - nx2);
                if (side1 > 0 and side2 > 0)
                {
                    addLight(light);
                }
            }
        }

        lightPoints.emplace_back(light1, light2);
    }

    auto wallStartX = wall.xStart - player.x - renderParameters.offsetX;
    auto wallStartY = wall.yStart - player.y - renderParameters.offsetY;
    auto wallEndX = wall.xEnd - player.x - renderParameters.offsetX;
    auto wallEndY = wall.yEnd - player.y - renderParameters.offsetY;

    auto transformedLeftX  = wallStartX * angleSin - wallStartY * angleCos;
    auto transformedRightX =   wallEndX * angleSin -   wallEndY * angleCos;
    auto transformedLeftZ  = wallStartX * angleCos + wallStartY * angleSin;
    auto transformedRightZ =   wallEndX * angleCos +   wallEndY * angleSin;

    auto& t = getTexture(wall.texture);
    int textureBoundaryLeft = 0;
    int textureBoundaryRight = t.width - 1;

    if (transformedLeftZ <= 0 and transformedRightZ <= 0)
    {
        return;
    }

    if (transformedLeftZ <= 0 or transformedRightZ <= 0)
    {
        auto nearZ = 1e-4, farZ = 5.0, nearSide = 1e-5, farSide = 20.0;

        auto [i1x, i1y] = intersect(transformedLeftX, transformedLeftZ, transformedRightX, transformedRightZ,
                                    -nearSide, nearZ, -farSide, farZ);
        auto [i2x, i2y] = intersect(transformedLeftX, transformedLeftZ, transformedRightX, transformedRightZ,
                                    nearSide, nearZ, farSide, farZ);

        auto oldLeftX = transformedLeftX;
        auto oldLeftZ = transformedLeftZ;
        auto oldRightX = transformedRightX;
        auto oldRightZ = transformedRightZ;

        if (transformedLeftZ < nearZ)
        {
            transformedLeftX = i1y > 0 ? i1x : i2x;
            transformedLeftZ = i1y > 0 ? i1y : i2y;
        }

        if (transformedRightZ < nearZ)
        {
            transformedRightX = i1y > 0 ? i1x : i2x;
            transformedRightZ = i1y > 0 ? i1y : i2y;
        }

        if (std::abs(transformedRightX - transformedLeftX) >
            std::abs(transformedRightZ - transformedLeftZ))
        {
            textureBoundaryLeft = (transformedLeftX - oldLeftX) * (t.width - 1) / (oldRightX - oldLeftX);
            textureBoundaryRight = (transformedRightX - oldRightX) * (t.width - 1) / (oldRightX - oldLeftX);
        }
        else
        {
            textureBoundaryLeft = (transformedLeftZ - oldLeftZ) * (t.width - 1) / (oldRightZ - oldLeftZ);
            textureBoundaryRight = (transformedRightZ - oldLeftZ) * (t.width - 1) / (oldRightZ - oldLeftZ);
        }
    }

    double scaleX1 = fovH / transformedLeftZ;
    double scaleY1 = fovV / transformedLeftZ;
    double scaleX2 = fovH / transformedRightZ;
    double scaleY2 = fovV / transformedRightZ;

    int leftX  = c::renderWidth / 2 - (int)(transformedLeftX * scaleX1);
    int rightX = c::renderWidth / 2 - (int)(transformedRightX * scaleX2);

    if (leftX >= rightX or rightX < renderParameters.leftXBoundary or leftX > renderParameters.rightXBoundary)
    {
        return;
    }

    double ceilingY = sector.ceiling - player.z - renderParameters.offsetZ;
    double floorY   = sector.floor   - player.z - renderParameters.offsetZ;

    double neighbourCeilingY = -renderParameters.offsetZ;
    double neighbourFloorY   = -renderParameters.offsetZ;

    if (wall.portal.has_value())
    {
        const auto& neighbourSector = level.sector(wall.portal->sector);
        neighbourCeilingY += neighbourSector.ceiling - player.z;
        neighbourFloorY   += neighbourSector.floor - player.z;
        if (wall.portal->transform.has_value())
        {
            neighbourCeilingY -= wall.portal->transform->z;
            neighbourFloorY -= wall.portal->transform->z;
        }
    }

    int leftYTop     = c::renderHeight / 2 - (int)(ceilingY * scaleY1);
    int leftYBottom  = c::renderHeight / 2 - (int)(floorY * scaleY1);
    int rightYTop    = c::renderHeight / 2 - (int)(ceilingY * scaleY2);
    int rightYBottom = c::renderHeight / 2 - (int)(floorY * scaleY2);

    int neighbourLeftYTop     = c::renderHeight / 2 - (int)(neighbourCeilingY * scaleY1);
    int neighbourLeftYBottom  = c::renderHeight / 2 - (int)(neighbourFloorY * scaleY1);
    int neighbourRightYTop    = c::renderHeight / 2 - (int)(neighbourCeilingY * scaleY2);
    int neighbourRightYBottom = c::renderHeight / 2 - (int)(neighbourFloorY * scaleY2);

    int beginX = std::max(leftX, renderParameters.leftXBoundary);
    int endX   = std::min(rightX, renderParameters.rightXBoundary);

    if (beginX > endX)
    {
        return;
    }

    auto wallLength = std::hypot(wallStartX - wallEndX, wallStartY - wallEndY);

    auto distanceLeft = std::hypot(transformedLeftX, transformedLeftZ, ceilingY - floorY);
    auto distanceRight = std::hypot(transformedRightX, transformedRightZ, ceilingY - floorY);

    textureBoundaryRight *= wallLength * (sector.ceiling - sector.floor);

    for (int x = beginX; x <= endX; ++x)
    {
        auto distance = (distanceRight - distanceLeft) * (x - leftX) / (rightX - leftX) + distanceLeft;

        auto wallTop    = (x - leftX) * (rightYTop - leftYTop) / (rightX - leftX) + leftYTop;
        auto wallBottom = (x - leftX) * (rightYBottom - leftYBottom) / (rightX - leftX) + leftYBottom;
        auto visibleWallTop    = std::clamp(wallTop, limitTop[x], limitBottom[x]);
        auto visibleWallBottom = std::clamp(wallBottom, limitTop[x], limitBottom[x]);

        renderCeilingAndFloor(sector, player, x,
                              visibleWallTop, visibleWallBottom,
                              distance,
                              ceilingY, floorY,
                              angleSin, angleCos,
                              lightMap);

        int textureX = (textureBoundaryLeft * ((rightX - x) * transformedRightZ) + textureBoundaryRight * ((x - leftX) * transformedLeftZ)) / ((rightX - x) * transformedRightZ + (x - leftX) * transformedLeftZ);
        while (textureX < 0) textureX += t.width; // TODO proper fix for texture offset in negative cases

        double xProgress = (double)(x - leftX) / (double)(rightX - leftX + 1) * (lightPointsCount - 1);

        int lightPoint = (int)std::floor(xProgress);

        double factor2 = std::clamp(xProgress - lightPoint, 0.0, 1.0);
        double factor1 = 1.0 - factor2;

        LightPoint top1 = lightPoints[lightPoint].first;
        LightPoint top2 = lightPoints[lightPoint + 1].first;
        LightPoint bottom1 = lightPoints[lightPoint].second;
        LightPoint bottom2 = lightPoints[lightPoint + 1].second;

        LightPoint top = factor1 * top1 + factor2 * top2;
        LightPoint bottom = factor1 * bottom1 + factor2 * bottom2;

        if (wall.portal.has_value())
        {
            int neighbourTop    = std::clamp((x - leftX) * (neighbourRightYTop - neighbourLeftYTop) / (rightX - leftX) + neighbourLeftYTop,
                                             limitTop[x], limitBottom[x]);
            int neighbourBottom = std::clamp((x - leftX) * (neighbourRightYBottom - neighbourLeftYBottom) / (rightX - leftX) + neighbourLeftYBottom,
                                             limitTop[x], limitBottom[x]);

            lightedLine(x, wallTop, wallBottom, visibleWallTop, neighbourTop - 1, t, textureX, distance, top, bottom);
            lightedLine(x, wallTop, wallBottom, neighbourBottom + 1, visibleWallBottom, t, textureX, distance, top, bottom);

            limitTop[x]    = std::clamp(std::max(visibleWallTop, neighbourTop), limitTop[x], c::renderHeight - 1);
            limitBottom[x] = std::clamp(std::min(visibleWallBottom, neighbourBottom), 0, limitBottom[x]);
        }
        else
        {
            textureX %= t.width;
            if (visibleWallTop >= visibleWallBottom)
            {
                continue;
            }

            lightedLine(x, wallTop, wallBottom, visibleWallTop, visibleWallBottom, t, textureX, distance, top, bottom);
        }
    }

    auto currentRenderDepth = renderParameters.depth;
    if (wall.portal.has_value() and currentRenderDepth > 0)
    {
        auto newOffsetX = renderParameters.offsetX;
        auto newOffsetY = renderParameters.offsetY;
        auto newOffsetZ = renderParameters.offsetZ;
        auto newOffsetAngle = renderParameters.offsetAngle;

        if (wall.portal->transform.has_value())
        {
            const auto& transform = *wall.portal->transform;
            newOffsetX += transform.x;
            newOffsetY += transform.y;
            newOffsetZ += transform.z;
            newOffsetAngle += transform.angle;
        }

        renderQueue.push(SectorRenderParams{wall.portal->sector,
                                            beginX, endX,
                                            currentRenderDepth - 1,
                                            newOffsetX, newOffsetY, newOffsetZ,
                                            newOffsetAngle});
    }
}

void Engine::lightedLine(int x,
                         int wallTop, int wallBottom,
                         int visibleWallTop, int visibleWallBottom,
                         sdl::Surface& texture, int textureX,
                         double distance,
                         const LightPoint& lightTop,
                         const LightPoint& lightBottom)
{
    double yStep = 1 / (double) (visibleWallBottom - visibleWallTop + 1);
    double factorT = 0;

    for (int y = visibleWallTop; y <= visibleWallBottom; ++y)
    {
        if (distance > zBuffer[x + y * c::renderWidth]) continue;
        int textureY = ((texture.height - 1) * (y - wallTop) / (wallBottom - wallTop) + texture.height) % texture.height;

        double factorB = 1 - factorT;
        buffer[x + y * c::renderWidth] = shadeRgb(texture.pixels()[textureX + textureY * texture.width],
                                                  lightTop * factorT + lightBottom * factorB);
        factorT += yStep;

        zBuffer[x + y * c::renderWidth] = distance;
    }
}

LightPoint Engine::calculateSurfaceLighting(double mapX, double mapY, bool isCeiling, const HorizontalLightMap& lightMap)
{
    double nearestX = std::floor(mapX * invMapRes) * mapRes;
    double nearestY = std::floor(mapY * invMapRes) * mapRes;

    auto lightMapX = std::clamp((int)((nearestX - lightMap.x) * invMapRes), 0, lightMap.width);
    auto lightMapY = std::clamp((int)((nearestY - lightMap.y) * invMapRes), 0, lightMap.height);

    auto stepX = std::clamp((mapX - nearestX) * invMapRes, 0.0, 1.0);
    auto stepY = std::clamp((mapY - nearestY) * invMapRes, 0.0, 1.0);
    auto invStepX = 1.0 - stepX;
    auto invStepY = 1.0 - stepY;

    auto& lmXcYc = isCeiling ? lightMap.map[lightMapX + lightMapY * lightMap.width].first
                             : lightMap.map[lightMapX + lightMapY * lightMap.width].second;
    auto& lmXnYc = isCeiling ? lightMap.map[(lightMapX + 1) + lightMapY * lightMap.width].first
                             : lightMap.map[(lightMapX + 1) + lightMapY * lightMap.width].second;
    auto& lmXcYn = isCeiling ? lightMap.map[lightMapX + (lightMapY + 1) * lightMap.width].first
                             : lightMap.map[lightMapX + (lightMapY + 1) * lightMap.width].second;
    auto& lmXnYn = isCeiling ? lightMap.map[(lightMapX + 1) + (lightMapY + 1) * lightMap.width].first
                             : lightMap.map[(lightMapX + 1) + (lightMapY + 1) * lightMap.width].second;

    return lmXcYc * (invStepX * invStepY / 2) +
           lmXnYc * (stepX * invStepY / 2) +
           lmXcYn * (invStepX * stepY / 2) +
           lmXnYn * (stepX * stepY / 2);
}

void Engine::renderCeilingAndFloor(const world::Sector& sector,
                                   const game::Position& player,
                                   int x, int wallTop, int wallBottom,
                                   double distance,
                                   double ceilingY, double floorY,
                                   double angleSin, double angleCos,
                                   const HorizontalLightMap& lightMap)
{
    auto& floorTexture = getTexture(sector.floorTexture);
    auto& ceilingTexture = getTexture(sector.ceilingTexture);

    auto invFovH = (c::renderWidth / 2 - x) / fovH;

    for (int y = limitTop[x]; y <= limitBottom[x]; ++y)
    {
        if (y <= wallBottom and y >= wallTop) continue;
        if (distance > zBuffer[x + y * c::renderWidth]) continue;

        auto isCeiling = y < wallTop;

        auto transformedZ = (isCeiling ? ceilingY : floorY) * fovV / (c::renderHeight / 2 - y);
        auto transformedX = transformedZ * invFovH;

        auto mapX = transformedZ * angleCos + transformedX * angleSin + player.x + renderQueue.front().offsetX;
        auto mapY = transformedZ * angleSin - transformedX * angleCos + player.y + renderQueue.front().offsetY;

        int textureWidth = (isCeiling ? ceilingTexture : floorTexture).width;
        int textureHeight = (isCeiling ? ceilingTexture : floorTexture).height;

        auto tX = (int)std::abs(textureWidth + mapX * textureWidth) % textureWidth;
        auto tY = (int)std::abs(textureHeight + mapY * textureHeight) % textureHeight;

        buffer[x + y * c::renderWidth] = shadeRgb((isCeiling ? ceilingTexture : floorTexture).pixels()[tX + tY * textureWidth],
                                                  calculateSurfaceLighting(mapX, mapY, isCeiling, lightMap));;
    }
}

void Engine::renderSprites(const world::Sector& sector, const game::Position& player, double angleSin, double angleCos)
{
    std::vector<std::tuple<int, double>> spriteQueue{};
    spriteQueue.reserve(sector.sprites.size());
    std::transform(sector.sprites.begin(), sector.sprites.end(),
                   std::back_inserter(spriteQueue),
                   [&player](const auto& sprite)
                   {
                       return std::make_tuple(sprite.id, std::hypot(player.x - sprite.x, player.y - sprite.y));
                   });
    std::sort(spriteQueue.begin(), spriteQueue.end(),
              [](const auto& a, const auto& b) { return std::get<1>(a) > std::get<1>(b); });

    const auto& renderParameters = renderQueue.front();

    for (const auto& [id, distance] : spriteQueue)
    {
        if (distance > 10)
        {
            continue;
        }

        const auto& sprite = sector.sprites[id];
        const auto& texture = getTexture(sprite.texture);

        auto spriteCenterX = sprite.x - player.x - renderParameters.offsetX;
        auto spriteCenterY = sprite.y - player.y - renderParameters.offsetY;

        auto transformedX  = spriteCenterX * angleSin - spriteCenterY * angleCos;
        auto transformedZ  = spriteCenterX * angleCos + spriteCenterY * angleSin;

        if (transformedZ <= 0)
        {
            continue;
        }

        double scaleX = fovH / transformedZ;
        double scaleY = fovV / transformedZ;

        int leftX  = c::renderWidth / 2 - (int)((transformedX + sprite.w / 2) * scaleX);
        int rightX = c::renderWidth / 2 - (int)((transformedX - sprite.w / 2) * scaleX);

        if (leftX >= rightX or rightX < renderParameters.leftXBoundary or leftX > renderParameters.rightXBoundary)
        {
            continue;
        }

        int topY     = c::renderHeight / 2 - (int)((sprite.z - player.z - renderParameters.offsetZ + sprite.h / 2) * scaleY);
        int bottomY  = c::renderHeight / 2 - (int)((sprite.z - player.z - renderParameters.offsetZ - sprite.h / 2) * scaleY);

        if (topY >= bottomY or bottomY < 0 or topY >= c::renderHeight - 1)
        {
            continue;
        }

        const auto& texturePixels = texture.pixels();

        auto startX = std::clamp(leftX,  renderParameters.leftXBoundary, renderParameters.rightXBoundary);
        auto endX   = std::clamp(rightX, renderParameters.leftXBoundary, renderParameters.rightXBoundary);
        auto startY = std::clamp(topY,    0, c::renderHeight - 1);
        auto endY   = std::clamp(bottomY, 0, c::renderHeight - 1);

        LightPoint lighting{};

        auto addLight = [&lighting, &sprite](const auto& light)
        {
            double deltaX = sprite.x - light.x;
            double deltaY = sprite.y - light.y;
            double deltaZ = sprite.z - light.z;

            double distanceFactor = 1 / (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

            lighting += distanceFactor * LightPoint{light.r, light.g, light.b};
        };

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

        for (auto y = startY; y <= endY; ++y)
        {
            int texY = (texture.height - 1) * (y - topY) / (bottomY - topY);
            for (auto x = startX; x <= endX; ++x)
            {
                if (distance > zBuffer[x + y * c::renderWidth]) continue;
                int texX = (texture.width - 1) * (x - leftX) / (rightX - leftX);
                const auto& pixel = texturePixels[texX + texY * texture.width];
                if ((pixel & 0xff000000) >> 24 == 0xff)
                {
                    buffer[x + y * c::renderWidth] = shadeRgb(pixel, lighting);
                    zBuffer[x + y * c::renderWidth] = distance;
                }
            }
        }
    }
}

void Engine::draw()
{
    view.update(buffer.data());
    renderer.copy(view);
}
}
