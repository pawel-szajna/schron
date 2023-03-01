#include "engine.hpp"

#include "game/player.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <algorithm>
#include <cmath>
#include <execution>
#include <ranges>
#include <spdlog/spdlog.h>

namespace engine
{
namespace
{
constexpr auto color(uint32_t red, uint32_t green, uint32_t blue) { return 65536 * red + 256 * green + blue; }
constexpr auto gray(uint32_t brightness) { return color(brightness, brightness, brightness); }
constexpr auto cross(double x1, double y1, double x2, double y2) { return x1 * y2 - x2 * y1; };
constexpr auto intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    auto c1 = cross(x1, y1, x2, y2);
    auto c2 = cross(x3, y3, x4, y4);
    auto m = cross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    return std::make_pair(cross(c1, x1 - x2, c2, x3 - x4) / m, cross(c1, y1 - y2, c2, y3 - y4) / m);
};
}

Engine::Engine(world::Level& level) :
    view(sdl::make_surface(c::renderWidth, c::renderHeight)),
    level(level)
{
    spdlog::info("Initializing engine");
}

void Engine::frame(const game::Position& player)
{
    constexpr static auto renderStart = 0;
    constexpr static auto renderEnd = c::renderWidth - 1;
    constexpr static auto initialDepth = 4;

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

    flushBuffer();
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

    double neighbourCeilingY = renderParameters.offsetZ;
    double neighbourFloorY = renderParameters.offsetZ;

    if (wall.portal.has_value())
    {
        const auto& neighbourSector = level.sector(wall.portal->sector);
        neighbourCeilingY = neighbourSector.ceiling - player.z;
        neighbourFloorY = neighbourSector.floor - player.z;
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

    for (int x = beginX; x <= endX; ++x)
    {
        int wallTop    = std::clamp((x - leftX) * (rightYTop - leftYTop) / (rightX - leftX) + leftYTop, limitTop[x], limitBottom[x]);
        int wallBottom = std::clamp((x - leftX) * (rightYBottom - leftYBottom) / (rightX - leftX) + leftYBottom, limitTop[x], limitBottom[x]);

        constexpr static auto colorWall    = gray(0x99);
        constexpr static auto colorCeiling = color(0xaa, 0x88, 0x88);
        constexpr static auto colorFloor   = color(0x88, 0xaa, 0x88);

        line(x, limitTop[x], wallTop, colorCeiling);
        line(x, wallBottom, limitBottom[x], colorFloor);

        if (wall.portal.has_value())
        {
            int neighbourTop    = std::clamp((x - leftX) * (neighbourRightYTop - neighbourLeftYTop) / (rightX - leftX) + neighbourLeftYTop,
                                             limitTop[x], limitBottom[x]);
            int neighbourBottom = std::clamp((x - leftX) * (neighbourRightYBottom - neighbourLeftYBottom) / (rightX - leftX) + neighbourLeftYBottom,
                                             limitTop[x], limitBottom[x]);

            line(x, wallTop,             neighbourTop - 1, x == leftX || x == rightX ? 0 : colorWall);
            line(x, neighbourBottom + 1, wallBottom, x == leftX || x == rightX ? 0 : colorWall);

            limitTop[x]    = std::clamp(std::max(wallTop, neighbourTop), limitTop[x], c::renderHeight - 1);
            limitBottom[x] = std::clamp(std::min(wallBottom, neighbourBottom), 0, limitBottom[x]);
        }
        else
        {
            line(x, wallTop, wallBottom, x > beginX and x < endX ? colorWall : 0);
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
    buffer[x][yStart] = 0;
    for (int y = yStart + 1; y < yEnd; ++y)
    {
        buffer[x][y] = color;
    }
    buffer[x][yEnd] = 0;
}

void Engine::flushBuffer()
{
    auto pixels = (uint32_t*)view->pixels;

    for (auto y = 0; y < c::renderHeight; ++y)
    {
        for (auto x = 0; x < c::renderWidth; ++x)
        {
            *pixels = buffer[x][y];
            ++pixels;
        }
        pixels += view->pitch / 4;
        pixels -= c::renderWidth;
    }

    for (auto x = 0; x < c::renderWidth; ++x)
    {
        buffer[x].fill(0);
    }
}

void Engine::draw(sdl::Surface& target)
{
    view.draw(target);
}
}
