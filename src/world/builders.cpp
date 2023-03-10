#include "builders.hpp"

namespace world
{

namespace
{
void addWall(double wx1, double wy1, double wx2, double wy2,
             const std::optional<RectangularSectorBuilder::NeighbourParams>& neighbour,
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
        walls.push_back(Wall{wx1, wy1, wx2, wy2, neighbour->portal});
        return;
    }

    if (neighbour->x1 != wx1 or neighbour->y1 != wy1)
    {
        walls.push_back(Wall{wx1, wy1, neighbour->x1, neighbour->y1, std::nullopt});
    }

    walls.push_back(Wall{neighbour->x1, neighbour->y1, neighbour->x2, neighbour->y2, neighbour->portal});

    if (neighbour->x2 != wx2 or neighbour->y2 != wy2)
    {
        walls.push_back(Wall{neighbour->x2, neighbour->y2, wx2, wy2, std::nullopt});
    }
}
}

PolygonalSectorBuilder::PolygonalSectorBuilder(double x1, double y1) :
    lastX(x1),
    lastY(y1)
{}

PolygonalSectorBuilder& PolygonalSectorBuilder::withId(int id)
{
    sId = id;
    return *this;
}

PolygonalSectorBuilder& PolygonalSectorBuilder::withCeiling(double ceiling)
{
    sCeiling = ceiling;
    return *this;
}

PolygonalSectorBuilder& PolygonalSectorBuilder::withFloor(double floor)
{
    sFloor = floor;
    return *this;
}

PolygonalSectorBuilder& PolygonalSectorBuilder::withWall(double x, double y, std::string texture)
{
    walls.push_back(Wall{lastX, lastY, x, y, std::nullopt, std::move(texture)});
    lastX = x;
    lastY = y;
    return *this;
}

PolygonalSectorBuilder& PolygonalSectorBuilder::withPortal(double x, double y, std::string texture, int neighbour)
{
    walls.push_back(Wall{lastX, lastY, x, y, Wall::Portal{neighbour, std::nullopt}, std::move(texture)});
    lastX = x;
    lastY = y;
    return *this;
}

Sector PolygonalSectorBuilder::build()
{
    return Sector{sId, std::move(walls), {}, sCeiling, sFloor, "ceiling", "floor"};
}

RectangularSectorBuilder& RectangularSectorBuilder::withId(int id)
{
    sid = id;
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withDimensions(double x1, double y1, double x2, double y2)
{
    sx1 = x1;
    sy1 = y1;
    sx2 = x2;
    sy2 = y2;
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withCeiling(double ceiling)
{
    sCeiling = ceiling;
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withFloor(double floor)
{
    sFloor = floor;
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withNorthNeighbour(int id,
                                                                       double x1, double y1,
                                                                       double x2, double y2)
{
    north = {x1, y1, x2, y2, {id}};
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withEastNeighbour(int id,
                                                                      double x1, double y1,
                                                                      double x2, double y2)
{
    east = {x1, y1, x2, y2, {id}};
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withSouthNeighbour(int id,
                                                                       double x1, double y1,
                                                                       double x2, double y2)
{
    south = {x1, y1, x2, y2, {id}};
    return *this;
}

RectangularSectorBuilder& RectangularSectorBuilder::withWestNeighbour(int id,
                                                                      double x1, double y1,
                                                                      double x2, double y2)
{
    west = {x1, y1, x2, y2, {id}};
    return *this;
}

Sector RectangularSectorBuilder::build()
{
    decltype(Sector::walls) walls{};
    addWall(sx1, sy1, sx2, sy1, north, walls);
    addWall(sx2, sy1, sx2, sy2, east, walls);
    addWall(sx2, sy2, sx1, sy2, south, walls);
    addWall(sx1, sy2, sx1, sy1, west, walls);
    return Sector{sid, std::move(walls), {}, sCeiling, sFloor, "ceiling", "floor"};
}
}
