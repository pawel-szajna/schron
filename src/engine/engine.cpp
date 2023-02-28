#include "engine.hpp"

#include "util/position.hpp"
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

void Engine::frame(double playerX, double playerY, double playerZ, double playerAngle)
{
    limitTop.fill(0);
    limitBottom.fill(c::renderHeight - 1);
    sectors.push({1, 0, c::renderWidth - 1, 4});
    rendered.clear();

    while (not sectors.empty())
    {
        auto id = sectors.front().id;

        const world::Sector& sector = level.sector(id);
        for (const auto& wall: sector.walls)
        {
            renderWall(sector, wall, playerX, playerY, playerZ, playerAngle);
        }

        sectors.pop();
    }

    flushBuffer();
}

void Engine::renderWall(const world::Sector& sector,
                        const world::Wall& wall,
                        double playerX, double playerY, double playerZ, double playerAngle)
{
    auto wallX1 = wall.xStart - playerX;
    auto wallX2 = wall.xEnd - playerX;
    auto wallY1 = wall.yStart - playerY;
    auto wallY2 = wall.yEnd - playerY;

    double transformedX1 = wallX1 * std::sin(playerAngle) - wallY1 * std::cos(playerAngle);
    double transformedX2 = wallX2 * std::sin(playerAngle) - wallY2 * std::cos(playerAngle);
    double transformedZ1 = wallX1 * std::cos(playerAngle) + wallY1 * std::sin(playerAngle);
    double transformedZ2 = wallX2 * std::cos(playerAngle) + wallY2 * std::sin(playerAngle);

    if (transformedZ1 <= 0 and transformedZ2 <= 0)
    {
        return;
    }

    if (transformedZ1 <= 0 or transformedZ2 <= 0)
    {
        double nearZ = 1e-4, farZ = 5, nearSide = 1e-5, farSide = 20.0;

        auto [i1x, i1y] = intersect(transformedX1, transformedZ1, transformedX2, transformedZ2, -nearSide, nearZ, -farSide, farZ);
        auto [i2x, i2y] = intersect(transformedX1, transformedZ1, transformedX2, transformedZ2, nearSide, nearZ, farSide, farZ);

        if (transformedZ1 < nearZ)
        {
            transformedX1 = i1y > 0 ? i1x : i2x;
            transformedZ1 = i1y > 0 ? i1y : i2y;
        }

        if (transformedZ2 < nearZ)
        {
            transformedX2 = i1y > 0 ? i1x : i2x;
            transformedZ2 = i1y > 0 ? i1y : i2y;
        }
    }

    constexpr static double fovH = c::renderHeight * 0.73;
    constexpr static double fovV = c::renderWidth * 0.2;

    double scaleX1 = fovH / transformedZ1;
    double scaleY1 = fovV / transformedZ1;
    double scaleX2 = fovH / transformedZ2;
    double scaleY2 = fovV / transformedZ2;

    int x1 = c::renderWidth / 2 - (int)(transformedX1 * scaleX1);
    int x2 = c::renderWidth / 2 - (int)(transformedX2 * scaleX2);

    if (x1 >= x2 or x2 < sectors.front().xLeft or x1 > sectors.front().xRight)
    {
        return;
    }

    double ceilingY = sector.ceiling - playerZ;
    double floorY = sector.floor - playerZ;

    double neighbourCeilingY = 0.0;
    double neighbourFloorY = 0.0;

    if (wall.neighbour.has_value())
    {
        const auto& neighbourSector = level.sector(*wall.neighbour);
        neighbourCeilingY = neighbourSector.ceiling - playerZ;
        neighbourFloorY = neighbourSector.floor - playerZ;
    }

    int y1a = c::renderHeight / 2 - (int)(ceilingY * scaleY1);
    int y1b = c::renderHeight / 2 - (int)(floorY * scaleY1);
    int y2a = c::renderHeight / 2 - (int)(ceilingY * scaleY2);
    int y2b = c::renderHeight / 2 - (int)(floorY * scaleY2);

    int ny1a = c::renderHeight / 2 - (int)(neighbourCeilingY * scaleY1);
    int ny1b = c::renderHeight / 2 - (int)(neighbourFloorY * scaleY1);
    int ny2a = c::renderHeight / 2 - (int)(neighbourCeilingY * scaleY2);
    int ny2b = c::renderHeight / 2 - (int)(neighbourFloorY * scaleY2);

    int beginX = std::max(x1, sectors.front().xLeft);
    int endX   = std::min(x2, sectors.front().xRight);

    for (int x = beginX; x <= endX; ++x)
    {
        int wallTop    = std::clamp((x - x1) * (y2a - y1a) / (x2 - x1) + y1a, limitTop[x], limitBottom[x]);
        int wallBottom = std::clamp((x - x1) * (y2b - y1b) / (x2 - x1) + y1b, limitTop[x], limitBottom[x]);

        constexpr static auto colorWall    = gray(0x99);
        constexpr static auto colorCeiling = color(0xaa, 0x88, 0x88);
        constexpr static auto colorFloor   = color(0x88, 0xaa, 0x88);

        line(x, limitTop[x], wallTop, colorCeiling);
        line(x, wallBottom, limitBottom[x], colorFloor);

        if (wall.neighbour.has_value())
        {
            int neighbourTop    = std::clamp((x - x1) * (ny2a - ny1a) / (x2 - x1) + ny1a, limitTop[x], limitBottom[x]);
            int neighbourBottom = std::clamp((x - x1) * (ny2b - ny1b) / (x2 - x1) + ny1b, limitTop[x], limitBottom[x]);

            line(x, wallTop,             neighbourTop - 1, x == x1 || x == x2 ? 0 : colorWall);
            line(x, neighbourBottom + 1, wallBottom,       x == x1 || x == x2 ? 0 : colorWall);

            limitTop[x]    = std::clamp(std::max(wallTop, neighbourTop), limitTop[x], c::renderHeight - 1);
            limitBottom[x] = std::clamp(std::min(wallBottom, neighbourBottom), 0, limitBottom[x]);
        }
        else
        {
            line(x, wallTop, wallBottom, x > beginX and x < endX ? colorWall : 0);
        }
    }

    if (wall.neighbour.has_value())
    {
        if (sectors.front().renderDepth > 0)
        {
            spdlog::debug("Adding neighbour {} to queue", *wall.neighbour);
            sectors.push({*wall.neighbour, beginX, endX, sectors.front().renderDepth - 1});
        }
        else
        {
            spdlog::debug("Neighbour {} skipped, depth exceeded", *wall.neighbour);
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
