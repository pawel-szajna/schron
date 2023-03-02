#pragma once

#include "sector.hpp"

#include <vector>

namespace world
{
class PolygonalSectorBuilder
{
public:

    PolygonalSectorBuilder(double x1, double y1);

    PolygonalSectorBuilder& withId(int id);
    PolygonalSectorBuilder& withCeiling(double ceiling);
    PolygonalSectorBuilder& withFloor(double floor);
    PolygonalSectorBuilder& withWall(double x, double y, std::optional<Wall::Portal> neighbour = std::nullopt);

    Sector build();

private:

    int sId{};
    std::vector<Wall> walls{};
    double sCeiling{}, sFloor{};
    double lastX, lastY;
};

class RectangularSectorBuilder
{
public:

    struct NeighbourParams
    {
        double x1, y1, x2, y2;
        Wall::Portal portal;
    };

    RectangularSectorBuilder& withId(int id);
    RectangularSectorBuilder& withDimensions(double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withCeiling(double ceiling);
    RectangularSectorBuilder& withFloor(double floor);
    RectangularSectorBuilder& withNorthNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withEastNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withSouthNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withWestNeighbour(int id, double x1, double y1, double x2, double y2);

    Sector build();

private:

    void buildWall(double wx1, double wy1, double wx2, double wy2,
                   const std::optional<NeighbourParams>& neighbour,
                   decltype(Sector::walls)& walls);

    int sid{};
    double sx1{}, sy1{}, sx2{}, sy2{}, sCeiling{}, sFloor{};
    std::optional<NeighbourParams> north{};
    std::optional<NeighbourParams> east{};
    std::optional<NeighbourParams> south{};
    std::optional<NeighbourParams> west{};
};
}