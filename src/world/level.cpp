#include "level.hpp"

#include <spdlog/spdlog.h>

namespace world
{
Level::Level(int width,
             int height,
             std::string name) :
     width(width),
     height(height),
     name(std::move(name))
{
    spdlog::debug("Creating level {}", this->name);

    put(PolygonalSectorBuilder(-2, -2)
            .withId(1)
            .withCeiling(1.5)
            .withFloor(0.0)
            .withWall(3, -2)
            .withWall(3, -1)
            .withWall(3, 2, 2)
            .withWall(-2, 2)
            .withWall(-2, -2)
            .build());
    put(RectangularSectorBuilder()
            .withId(2)
            .withDimensions(3, -1, 5, 2)
            .withCeiling(1.0)
            .withFloor(0.0)
            .withWestNeighbour(1, 3, 2, 3, -1)
            .withSouthNeighbour(3, 5, 2, 3, 2)
            .build());
    put(RectangularSectorBuilder()
            .withId(3)
            .withDimensions(3, 2, 5, 4)
            .withCeiling(1.0)
            .withFloor(-0.5)
            .withNorthNeighbour(2, 3, 2, 5, 2)
            .build());
}

void Level::put(Sector&& sector)
{
    spdlog::info("NEW SECTOR id = {}", sector.id);
    for (const auto& wall : sector.walls)
    {
        spdlog::debug("[{},{}] - [{},{}] -> {}", wall.xStart, wall.yStart, wall.xEnd, wall.yEnd, wall.neighbour.has_value() ? *wall.neighbour : -1);
    }
    if (map.contains(sector.id))
    {
        spdlog::warn("Duplicate sector {}", sector.id);
    }
    map.emplace(sector.id, sector);
}
}
