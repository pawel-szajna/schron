#pragma once

#include "sector.hpp"

#include <vector>

namespace world
{
class SectorBuilder
{
public:

    virtual ~SectorBuilder() = default;
    virtual Sector build() = 0;
};

class PolygonalSectorBuilder : public SectorBuilder
{
public:

    PolygonalSectorBuilder(double x1, double y1);

    PolygonalSectorBuilder& withId(int id);
    PolygonalSectorBuilder& withCeiling(double ceiling);
    PolygonalSectorBuilder& withFloor(double floor);
    PolygonalSectorBuilder& withWall(double x, double y, std::string texture = "wall");
    PolygonalSectorBuilder& withPortal(double x, double y, std::string texture, int neighbour);

    Sector build() override;

private:

    int sId{};
    std::vector<Wall> walls{};
    double sCeiling{}, sFloor{};
    double lastX, lastY;
};

class RectangularSectorBuilder : public SectorBuilder
{
public:

    struct NeighbourParams
    {
        double x1, y1, x2, y2;
        Wall::Portal portal;
    };

    explicit RectangularSectorBuilder(int id) { withId(id); }

    RectangularSectorBuilder& withId(int id);
    RectangularSectorBuilder& withDimensions(double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withCeiling(double ceiling);
    RectangularSectorBuilder& withFloor(double floor);
    RectangularSectorBuilder& withNorthNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withEastNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withSouthNeighbour(int id, double x1, double y1, double x2, double y2);
    RectangularSectorBuilder& withWestNeighbour(int id, double x1, double y1, double x2, double y2);

    Sector build() override;

private:

    int sid{};
    double sx1{}, sy1{}, sx2{}, sy2{}, sCeiling{1.0}, sFloor{0.0};
    std::optional<NeighbourParams> north{};
    std::optional<NeighbourParams> east{};
    std::optional<NeighbourParams> south{};
    std::optional<NeighbourParams> west{};
};
}
