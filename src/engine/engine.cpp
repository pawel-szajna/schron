#include "engine.hpp"

#include "game/player.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/surface.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <algorithm>
#include <cmath>
#include <spdlog/spdlog.h>

namespace engine
{
namespace
{
constexpr auto color(uint32_t red, uint32_t green, uint32_t blue) { return 65536 * red + 256 * green + blue; }
constexpr auto cross(double x1, double y1, double x2, double y2) { return x1 * y2 - x2 * y1; };
constexpr auto intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    auto c1 = cross(x1, y1, x2, y2);
    auto c2 = cross(x3, y3, x4, y4);
    auto m = cross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    return std::make_pair(cross(c1, x1 - x2, c2, x3 - x4) / m, cross(c1, y1 - y2, c2, y3 - y4) / m);
};

constexpr auto shade(uint32_t pixel, double distance)
{
    auto pR = (pixel & 0x00ff0000) >> 16;
    auto pG = (pixel & 0x0000ff00) >> 8;
    auto pB = (pixel & 0x000000ff) >> 0;
    pR = std::clamp((int)((int)pR * (1 - distance / 5)), 0, (int)pR);
    pG = std::clamp((int)((int)pG * (1 - distance / 5)), 0, (int)pG);
    pB = std::clamp((int)((int)pB * (1 - distance / 5)), 0, (int)pB);
    return (pR << 16) | (pG << 8) | (pB << 0);
}
}

Engine::Engine(sdl::Renderer& renderer, world::Level& level) :
    renderer(renderer),
    view(renderer.createTexture(sdl::Texture::Access::Streaming, c::renderWidth, c::renderHeight)),
    level(level)
{
    spdlog::info("Initialized engine");
}

sdl::Surface& Engine::texture(const std::string& name)
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
    constexpr static auto initialDepth = 4;

    buffer.fill(0);

    limitTop.fill(0);
    limitBottom.fill(c::renderHeight - 1);

    renderQueue.push(SectorRenderParams{player.sector, renderStart, renderEnd, initialDepth});

    while (not renderQueue.empty())
    {
        auto id = renderQueue.front().id;

        const world::Sector& sector = level.sector(id);
        for (const auto& wall : sector.walls)
        {
            renderWall(sector, wall, player);
        }

        renderQueue.pop();
    }
}

void Engine::renderWall(const world::Sector& sector,
                        const world::Wall& wall,
                        const game::Position& player)
{
    const auto& renderParameters = renderQueue.front();

    auto wallStartX = wall.xStart - player.x - renderParameters.offsetX;
    auto wallStartY = wall.yStart - player.y - renderParameters.offsetY;
    auto wallEndX = wall.xEnd - player.x - renderParameters.offsetX;
    auto wallEndY = wall.yEnd - player.y - renderParameters.offsetY;

    auto renderedAngle = player.angle - renderParameters.offsetAngle;

    auto transformedLeftX  = wallStartX * std::sin(renderedAngle) - wallStartY * std::cos(renderedAngle);
    auto transformedRightX =   wallEndX * std::sin(renderedAngle) -   wallEndY * std::cos(renderedAngle);
    auto transformedLeftZ  = wallStartX * std::cos(renderedAngle) + wallStartY * std::sin(renderedAngle);
    auto transformedRightZ =   wallEndX * std::cos(renderedAngle) +   wallEndY * std::sin(renderedAngle);

    auto& t = texture(wall.texture);
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

        if (std::abs(transformedRightX - transformedLeftX) > std::abs(transformedRightZ - transformedLeftZ))
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

    constexpr static double fovH = c::renderHeight * 0.73;
    constexpr static double fovV = c::renderWidth * 0.2;

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

    auto wallLength = std::hypot(wallStartX - wallEndX, wallStartY - wallEndY);

    auto distanceLeft = std::hypot(transformedLeftX - player.x, transformedLeftZ - player.y);
    auto distanceRight = std::hypot(transformedRightX - player.x, transformedRightZ - player.y);

    textureBoundaryRight *= wallLength * (sector.ceiling - sector.floor);

    for (int x = beginX; x <= endX; ++x)
    {
        auto distance = (x - leftX) * (distanceRight - distanceLeft) / (rightX - leftX) + distanceLeft;

        auto wallTop    = (x - leftX) * (rightYTop - leftYTop) / (rightX - leftX) + leftYTop;
        auto wallBottom = (x - leftX) * (rightYBottom - leftYBottom) / (rightX - leftX) + leftYBottom;
        auto visibleWallTop    = std::clamp(wallTop, limitTop[x], limitBottom[x]);
        auto visibleWallBottom = std::clamp(wallBottom, limitTop[x], limitBottom[x]);

        constexpr static auto colorCeiling = color(0xaa, 0x88, 0x88);

        line(x, limitTop[x], visibleWallTop, colorCeiling);

        auto& floorTexture = texture(sector.floorTexture);
        auto& ceilingTexture = texture(sector.ceilingTexture);

        for (int y = limitTop[x]; y <= limitBottom[x]; ++y)
        {
            if (y < visibleWallBottom and y > visibleWallTop) continue;

            auto transformedZ = (y < visibleWallTop ? ceilingY : floorY) * fovV / (c::renderHeight / 2 - y);
            auto transformedX = transformedZ * (c::renderWidth / 2 - x) / fovH;

            auto mapX = transformedZ * std::cos(player.angle) + transformedX * std::sin(player.angle) + player.x + renderParameters.offsetX;
            auto mapY = transformedZ * std::sin(player.angle) - transformedX * std::cos(player.angle) + player.y + renderParameters.offsetY;

            int textureWidth = (y < visibleWallTop ? ceilingTexture : floorTexture).width;
            int textureHeight = (y < visibleWallTop ? ceilingTexture : floorTexture).height;

            auto tX = (int)std::abs(textureWidth + mapX * textureWidth) % textureWidth;
            auto tY = (int)std::abs(textureHeight + mapY * textureHeight) % textureHeight;

            auto pixel = (y < visibleWallTop ? ceilingTexture : floorTexture).pixels()[tX + tY * textureWidth];
            buffer[x + y * c::renderWidth] = shade(pixel, std::hypot(transformedX, transformedZ));
        }

        int textureX = (textureBoundaryLeft * ((rightX - x) * transformedRightZ) + textureBoundaryRight * ((x - leftX) * transformedLeftZ)) / ((rightX - x) * transformedRightZ + (x - leftX) * transformedLeftZ);

        if (wall.portal.has_value())
        {
            int neighbourTop    = std::clamp((x - leftX) * (neighbourRightYTop - neighbourLeftYTop) / (rightX - leftX) + neighbourLeftYTop,
                                             limitTop[x], limitBottom[x]);
            int neighbourBottom = std::clamp((x - leftX) * (neighbourRightYBottom - neighbourLeftYBottom) / (rightX - leftX) + neighbourLeftYBottom,
                                             limitTop[x], limitBottom[x]);

            texturedLine(x, wallTop, wallBottom, visibleWallTop, neighbourTop - 1, t, textureX, distance);
            texturedLine(x, wallTop, wallBottom, neighbourBottom + 1, visibleWallBottom, t, textureX, distance);

            limitTop[x]    = std::clamp(std::max(visibleWallTop, neighbourTop), limitTop[x], c::renderHeight - 1);
            limitBottom[x] = std::clamp(std::min(visibleWallBottom, neighbourBottom), 0, limitBottom[x]);
        }
        else
        {
            texturedLine(x, wallTop, wallBottom, visibleWallTop, visibleWallBottom, t, textureX, distance);
        }
    }

    auto currentRenderDepth = renderParameters.depth;
    if (wall.portal.has_value() and currentRenderDepth > 0)
    {
        if (wall.portal->transform.has_value())
        {
            const auto& transform = *wall.portal->transform;
            renderQueue.push(SectorRenderParams{wall.portal->sector,
                                                beginX, endX,
                                                currentRenderDepth - 1,
                                                renderParameters.offsetX + transform.x,
                                                renderParameters.offsetY + transform.y,
                                                renderParameters.offsetZ + transform.z,
                                                renderParameters.offsetAngle + transform.angle});
        }
        else
        {
            renderQueue.push(SectorRenderParams{wall.portal->sector,
                                                beginX, endX,
                                                currentRenderDepth - 1,
                                                renderParameters.offsetX,
                                                renderParameters.offsetY,
                                                renderParameters.offsetZ,
                                                renderParameters.offsetAngle});
        }
    }
}

void Engine::line(int x, int yStart, int yEnd, int color)
{
    yStart = std::clamp(yStart, 0, c::renderHeight - 1);
    yEnd   = std::clamp(yEnd, 0, c::renderHeight - 1);
    if (yStart >= yEnd)
    {
        return;
    }
    buffer[yStart * c::renderWidth + x] = 0;
    for (int y = yStart + 1; y < yEnd; ++y)
    {
        buffer[y * c::renderWidth + x] = color;
    }
    buffer[yEnd * c::renderWidth + x] = 0;
}

void Engine::texturedLine(int x,
                          int wallStart, int wallEnd,
                          int yStart, int yEnd,
                          sdl::Surface& t,
                          int textureX,
                          double distance)
{
    textureX %= t.width;
    yStart = std::clamp(yStart, 0, c::renderHeight - 1);
    yEnd   = std::clamp(yEnd, 0, c::renderHeight - 1);
    if (yStart >= yEnd)
    {
        return;
    }
    for (int y = yStart; y <= yEnd; ++y)
    {
        int textureY = (t.height - 1) * (y - wallStart) / (wallEnd - wallStart);
        auto pixel = t.pixels()[textureX + textureY * t.width];
        buffer[y * c::renderWidth + x] = shade(pixel, distance);
    }
}

void Engine::draw()
{
    view.update(buffer.data());
    renderer.copy(view);
}
}
