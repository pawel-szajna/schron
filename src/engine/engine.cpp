#include "engine.hpp"

#include "game/player.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "utilities.hpp"
#include "world/level.hpp"

#include <algorithm>
#include <cmath>

#if not defined(DISABLE_PARALLELISM)
#   include <boost/iterator/counting_iterator.hpp>
#   ifdef __APPLE__
#       include <oneapi/dpl/execution>
#       include <oneapi/dpl/algorithm>
#   else
#       include <execution>
#   endif
#endif

namespace engine
{
namespace
{
constexpr auto loadingMargin = 32;
constexpr auto loadingBarSize = 4;

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

void loadingScreen(sdl::Renderer& renderer, int progress, int max)
{
    renderer.setColor(0, 0, 0, 255);
    renderer.clear();
    renderer.renderRectangle(loadingMargin, 4 * c::windowHeight / 5,
                             progress * (c::windowHeight - 2 * loadingMargin) / max, loadingBarSize,
                             0, 100, 200, 255);
    renderer.present();
}
}

uint64_t lightingTime = 0;
uint64_t geometryTime = 0;
uint64_t spritesTime = 0;

Engine::Engine(sdl::Renderer& renderer, world::Level& level) :
    renderer(renderer),
    view(renderer.createTexture(sdl::Texture::Access::Streaming, c::renderWidth, c::renderHeight)),
    level(level),
    lighting(level, [&](const std::string& texture) -> sdl::Surface& { return getTexture(texture); })
{
    spdlog::info("Initialized engine");
}

void Engine::preload()
{
    loadingScreen(renderer, 0, 1);
    std::unordered_set<std::string> filenames;
    for (const auto& [_, sector] : level.sectors())
    {
        filenames.emplace(sector.ceilingTexture);
        filenames.emplace(sector.floorTexture);
        std::transform(sector.walls.begin(), sector.walls.end(), std::inserter(filenames, filenames.begin()),
                       [](const auto& wall) { return wall.texture; });
        std::transform(sector.sprites.begin(), sector.sprites.end(), std::inserter(filenames, filenames.begin()),
                       [](const auto& sprite) { return sprite.texture; });
    }
    int i = 0;
    for (const auto& filename : filenames)
    {
        loadingScreen(renderer, ++i, filenames.size());
        getTexture(filename);
    }
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

    lightingTime = 0;
    geometryTime = 0;
    spritesTime = 0;

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

        uint64_t sectorStart = sdl::currentTimeNs();

        auto [ceilingLightMap, floorLightMap] = lighting.prepareSurfaceMap(sector, player);
        uint64_t lightingDone = sdl::currentTimeNs();

        for (const auto& wall : sector.walls)
        {
            renderWall(sector, wall, player, angleSin, angleCos, ceilingLightMap, floorLightMap);
        }
        uint64_t wallsDone = sdl::currentTimeNs();

        renderSprites(sector, player, angleSin, angleCos);
        uint64_t spritesDone = sdl::currentTimeNs();

        renderQueue.pop();

        lightingTime += lightingDone - sectorStart;
        geometryTime += wallsDone - lightingDone;
        spritesTime += spritesDone - wallsDone;
    }
}

void Engine::renderWall(const world::Sector& sector,
                        const world::Wall& wall,
                        const game::Position& player,
                        double angleSin, double angleCos,
                        const OffsetLightMap& ceilingLightMap,
                        const OffsetLightMap& floorLightMap)
{
    const auto& renderParameters = renderQueue.front();

    auto wallStartX = wall.xStart - player.x - renderParameters.offsetX;
    auto wallStartY = wall.yStart - player.y - renderParameters.offsetY;
    auto wallEndX = wall.xEnd - player.x - renderParameters.offsetX;
    auto wallEndY = wall.yEnd - player.y - renderParameters.offsetY;

    auto transformedLeftX  = wallStartX * angleSin - wallStartY * angleCos;
    auto transformedRightX =   wallEndX * angleSin -   wallEndY * angleCos;
    auto transformedLeftZ  = wallStartX * angleCos + wallStartY * angleSin;
    auto transformedRightZ =   wallEndX * angleCos +   wallEndY * angleSin;

    auto boundaryLeft = 0.0;
    auto boundaryRight = 1.0;

    if (transformedLeftZ <= 0 and transformedRightZ <= 0)
    {
        return;
    }

    if (transformedLeftZ <= 0 or transformedRightZ <= 0)
    {
        auto nearZ = 1e-4, farZ = 5.0, nearSide = 1e-5, farSide = 20.0;

        auto [iLeftX, iLeftZ] = intersect(transformedLeftX, transformedLeftZ, transformedRightX, transformedRightZ,
                                          -nearSide, nearZ, -farSide, farZ);
        auto [iRightX, iRightZ] = intersect(transformedLeftX, transformedLeftZ, transformedRightX, transformedRightZ,
                                            nearSide, nearZ, farSide, farZ);

        auto oldLeftX = transformedLeftX;
        auto oldLeftZ = transformedLeftZ;
        auto oldRightX = transformedRightX;
        auto oldRightZ = transformedRightZ;

        if (transformedLeftZ < nearZ)
        {
            transformedLeftX = iLeftZ > 0 ? iLeftX : iRightX;
            transformedLeftZ = iLeftZ > 0 ? iLeftZ : iRightZ;
        }

        if (transformedRightZ < nearZ)
        {
            transformedRightX = iLeftZ > 0 ? iLeftX : iRightX;
            transformedRightZ = iLeftZ > 0 ? iLeftZ : iRightZ;
        }

        if (std::abs(transformedRightX - transformedLeftX) > std::abs(transformedRightZ - transformedLeftZ))
        {
            auto denominator = 1 / (oldRightX - oldLeftX);
            boundaryLeft = (transformedLeftX - oldLeftX) * denominator;
            boundaryRight = (transformedRightX - oldLeftX) * denominator;
        }
        else
        {
            auto denominator = 1 / (oldRightZ - oldLeftZ);
            boundaryLeft = (transformedLeftZ - oldLeftZ) * denominator;
            boundaryRight = (transformedRightZ - oldLeftZ) * denominator;
        }
    }

    double scaleX1 = player.fovH / transformedLeftZ;
    double scaleY1 = player.fovV / transformedLeftZ;
    double scaleX2 = player.fovH / transformedRightZ;
    double scaleY2 = player.fovV / transformedRightZ;

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

    auto& t = getTexture(wall.texture);
    int textureBoundaryLeft = (int)((t.width - 1) * boundaryLeft);
    int textureBoundaryRight = (int)((t.width - 1) * boundaryRight);

    textureBoundaryRight *= (int)(wallLength * (sector.ceiling - sector.floor));

    auto lightPoints = lighting.prepareWallMap(sector, wall, player);
    int lightsBoundaryLeft = (int)((lightPoints.width - 2) * boundaryLeft);
    int lightsBoundaryRight = (int)((lightPoints.width - 2) * boundaryRight);

#if defined(DISABLE_PARALLELISM)
    for (int x = beginX; x <= endX; ++x)
#else
    std::for_each(std::execution::par,
                  boost::counting_iterator(beginX),
                  boost::counting_iterator(endX + 1),
                  [&](int x)
#endif
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
                              ceilingLightMap, floorLightMap);

        double denominator = 1.0 / ((rightX - x) * transformedRightZ + (x - leftX) * transformedLeftZ);
        auto xProgress = (((double)lightsBoundaryLeft * (rightX - x) * transformedRightZ) + (double)lightsBoundaryRight * (x - leftX) * transformedLeftZ) * denominator;
        int textureX = (int)((textureBoundaryLeft * ((rightX - x) * transformedRightZ) + textureBoundaryRight * ((x - leftX) * transformedLeftZ)) * denominator);
        while (textureX < 0) textureX += t.width;

        if (wall.portal.has_value())
        {
            int neighbourTop    = std::clamp((x - leftX) * (neighbourRightYTop - neighbourLeftYTop) / (rightX - leftX) + neighbourLeftYTop,
                                             limitTop[x], limitBottom[x]);
            int neighbourBottom = std::clamp((x - leftX) * (neighbourRightYBottom - neighbourLeftYBottom) / (rightX - leftX) + neighbourLeftYBottom,
                                             limitTop[x], limitBottom[x]);

            lightedLine(x, xProgress, wallTop, wallBottom, visibleWallTop, neighbourTop - 1, t, textureX, distance, lightPoints);
            lightedLine(x, xProgress, wallTop, wallBottom, neighbourBottom + 1, visibleWallBottom, t, textureX, distance, lightPoints);

            limitTop[x]    = std::clamp(std::max(visibleWallTop, neighbourTop), limitTop[x], c::renderHeight - 1);
            limitBottom[x] = std::clamp(std::min(visibleWallBottom, neighbourBottom), 0, limitBottom[x]);
        }
        else
        {
            textureX %= t.width;
            if (visibleWallTop >= visibleWallBottom)
            {
#if defined(DISABLE_PARALLELISM)
                continue;
#else
                return;
#endif
            }

            lightedLine(x, xProgress, wallTop, wallBottom, visibleWallTop, visibleWallBottom, t, textureX, distance, lightPoints);
        }
#if defined(DISABLE_PARALLELISM)
    }
#else
    });
#endif

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

void Engine::lightedLine(int x, double xProgress,
                         int wallTop, int wallBottom,
                         int visibleWallTop, int visibleWallBottom,
                         sdl::Surface& texture, int textureX,
                         double distance,
                         const LightMap& lightMap)
{
    double yStep = 1 / (double)(wallBottom - wallTop + 1);
    double yProgress = yStep * (visibleWallTop - wallTop);

    for (int y = visibleWallTop; y <= visibleWallBottom; ++y, yProgress += yStep)
    {
        if (distance > zBuffer[x + y * c::renderWidth]) continue;
        int textureY = ((texture.height - 1) * (y - wallTop) / (wallBottom - wallTop) + texture.height) % texture.height;

        buffer[x + y * c::renderWidth] = shadeRgb(texture.pixels()[textureX + textureY * texture.width],
                                                  lighting.calculateWallLighting(xProgress, yProgress * (lightMap.height - 2), lightMap));
        zBuffer[x + y * c::renderWidth] = distance;
    }
}

void Engine::renderCeilingAndFloor(const world::Sector& sector,
                                   const game::Position& player,
                                   int x, int wallTop, int wallBottom,
                                   double distance,
                                   double ceilingY, double floorY,
                                   double angleSin, double angleCos,
                                   const OffsetLightMap& ceilingLightMap,
                                   const OffsetLightMap& floorLightMap)
{
    auto& floorTexture = getTexture(sector.floorTexture);
    auto& ceilingTexture = getTexture(sector.ceilingTexture);

    auto invFovH = (c::renderWidth / 2 - x) / player.fovH;

    for (int y = limitTop[x]; y <= limitBottom[x]; ++y)
    {
        if (y <= wallBottom and y >= wallTop) continue;
        if (distance > zBuffer[x + y * c::renderWidth]) continue;

        auto isCeiling = y < wallTop;

        auto transformedZ = (isCeiling ? ceilingY : floorY) * player.fovV / (c::renderHeight / 2 - y);
        auto transformedX = transformedZ * invFovH;

        auto mapX = transformedZ * angleCos + transformedX * angleSin + player.x + renderQueue.front().offsetX;
        auto mapY = transformedZ * angleSin - transformedX * angleCos + player.y + renderQueue.front().offsetY;

        int textureWidth = (isCeiling ? ceilingTexture : floorTexture).width;
        int textureHeight = (isCeiling ? ceilingTexture : floorTexture).height;

        auto tX = (int)std::abs(textureWidth + mapX * textureWidth) % textureWidth;
        auto tY = (int)std::abs(textureHeight + mapY * textureHeight) % textureHeight;

        buffer[x + y * c::renderWidth] = shadeRgb((isCeiling ? ceilingTexture : floorTexture).pixels()[tX + tY * textureWidth],
                                                  lighting.calculateSurfaceLighting(mapX, mapY, isCeiling ? ceilingLightMap : floorLightMap));;
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

        auto invZ = 1 / transformedZ;

        double scaleX = player.fovH * invZ;
        double scaleY = player.fovV * invZ;

        int leftX  = c::renderWidth / 2 - (int)((transformedX + sprite.w / 2) * scaleX);
        int rightX = c::renderWidth / 2 - (int)((transformedX - sprite.w / 2) * scaleX);

        if (leftX >= rightX or rightX < renderParameters.leftXBoundary or leftX > renderParameters.rightXBoundary)
        {
            continue;
        }

        int topY     = c::renderHeight / 2 - (int)((sprite.z + sprite.offset - player.z - renderParameters.offsetZ + sprite.h / 2) * scaleY);
        int bottomY  = c::renderHeight / 2 - (int)((sprite.z + sprite.offset - player.z - renderParameters.offsetZ - sprite.h / 2) * scaleY);

        if (topY >= bottomY or bottomY < 0 or topY >= c::renderHeight - 1)
        {
            continue;
        }

        const auto& texturePixels = texture.pixels();

        auto startX = std::clamp(leftX,  renderParameters.leftXBoundary, renderParameters.rightXBoundary);
        auto endX   = std::clamp(rightX, renderParameters.leftXBoundary, renderParameters.rightXBoundary);
        auto startY = std::clamp(topY,    0, c::renderHeight - 1);
        auto endY   = std::clamp(bottomY, 0, c::renderHeight - 1);

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
                    buffer[x + y * c::renderWidth] = shadeRgb(pixel, lighting.calculateSpriteLighting(sector, sprite, player));
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
