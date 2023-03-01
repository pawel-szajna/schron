#pragma once

#include "util/position.hpp"

#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

namespace world
{
class Wall
{
public:
    double xStart, yStart, xEnd, yEnd;
    std::optional<int> neighbour{std::nullopt};
};

class Sector
{
public:
    int id{};
    std::vector<Wall> walls{};
    double ceiling{1.0};
    double floor{0.0};
};

struct PolygonalSectorBuilder
{
    PolygonalSectorBuilder(double x1, double y1) : lastX(x1), lastY(y1) {}
    auto withId(int id) -> decltype(*this)& { sid = id; return *this; }
    auto withCeiling(double ceiling) -> decltype(*this)& { sceiling = ceiling; return *this; }
    auto withFloor(double floor) -> decltype(*this)& { sfloor = floor; return *this; }
    auto withWall(double x, double y, std::optional<int> neighbour = std::nullopt) -> decltype(*this)&
    {
        walls.push_back(Wall{lastX, lastY, x, y, neighbour});
        lastX = x;
        lastY = y;
        return *this;
    }

    auto build() -> Sector
    {
        return Sector{sid, std::move(walls), sceiling, sfloor};
    }

private:

    int sid{};
    std::vector<Wall> walls{};
    double sceiling{}, sfloor{};
    double lastX, lastY;
};

struct RectangularSectorBuilder
{
    struct NeighbourParams { double x1, y1, x2, y2; int id; };

    auto withId(int id) -> decltype(*this)& { sid = id; return *this; }
    auto withDimensions(double x1, double y1, double x2, double y2) -> decltype(*this)& { sx1 = x1; sy1 = y1; sx2 = x2; sy2 = y2; return *this; }
    auto withCeiling(double ceiling) -> decltype(*this)& { sceiling = ceiling; return *this; }
    auto withFloor(double floor) -> decltype(*this)& { sfloor = floor; return *this; }
    auto withNorthNeighbour(int id, double x1, double y1, double x2, double y2) -> decltype(*this)& { north = {x1, y1, x2, y2, id}; return *this; }
    auto withEastNeighbour(int id, double x1, double y1, double x2, double y2) -> decltype(*this)& { east = {x1, y1, x2, y2, id}; return *this; }
    auto withSouthNeighbour(int id, double x1, double y1, double x2, double y2) -> decltype(*this)& { south = {x1, y1, x2, y2, id}; return *this; }
    auto withWestNeighbour(int id, double x1, double y1, double x2, double y2) -> decltype(*this)& { west = {x1, y1, x2, y2, id}; return *this; }

    auto build() -> Sector
    {
        decltype(Sector::walls) walls{};
        buildWall(sx1, sy1, sx2, sy1, north, walls);
        buildWall(sx2, sy1, sx2, sy2, east, walls);
        buildWall(sx2, sy2, sx1, sy2, south, walls);
        buildWall(sx1, sy2, sx1, sy1, west, walls);
        spdlog::debug("== Built sector {} ==", sid);
        for (const auto& wall : walls)
        {
            spdlog::debug("({}, {}) -> ({}, {}) [{}]",
                          wall.xStart, wall.yStart,
                          wall.xEnd, wall.yEnd,
                          wall.neighbour.has_value() ? *wall.neighbour : -1);
        }
        return Sector{sid, std::move(walls), sceiling, sfloor};
    }

private:

    void buildWall(double wx1, double wy1,
                   double wx2, double wy2,
                   const std::optional<NeighbourParams>& neighbour,
                   decltype(Sector::walls)& walls)
    {
        if (not neighbour.has_value())
        {
            walls.push_back(Wall{wx1, wy1, wx2, wy2, std::nullopt});
            return;
        }

        if (neighbour->x1 == wx1 and neighbour->y1 == wy2 and
            neighbour->x2 == wx2 and neighbour->y2 == wy2)
        {
            walls.push_back(Wall{wx1, wy1, wx2, wy2, neighbour->id});
            return;
        }

        if (neighbour->x1 != wx1 or neighbour->y1 != wy1)
        {
            walls.push_back(Wall{wx1, wy1, neighbour->x1, neighbour->y1, std::nullopt});
        }

        walls.push_back(Wall{neighbour->x1, neighbour->y1, neighbour->x2, neighbour->y2, neighbour->id});

        if (neighbour->x2 != wx2 or neighbour->y2 != wy2)
        {
            walls.push_back(Wall{neighbour->x2, neighbour->y2, wx2, wy2, std::nullopt});
        }
    }

    int sid{};
    double sx1{}, sy1{}, sx2{}, sy2{}, sceiling{}, sfloor{};
    std::optional<NeighbourParams> north{};
    std::optional<NeighbourParams> east{};
    std::optional<NeighbourParams> south{};
    std::optional<NeighbourParams> west{};
};
}
