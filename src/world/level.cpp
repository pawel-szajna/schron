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

    auto x = RectangularSectorBuilder()
            .withId(1)
            .withDimensions(-2, -2, 3, 2)
            .withCeiling(1.5)
            .withFloor(0.0)
            .withEastNeighbour(2, 3, -1, 3, 2)
            .build();
    x.walls[x.walls.size() - 1].xEnd -= 1;
    x.walls[x.walls.size() - 1].yEnd += 1;
    x.walls.push_back(Wall{x.walls[x.walls.size() - 1].xEnd,
                           x.walls[x.walls.size() - 1].yEnd,
                           x.walls[0].xStart,
                           x.walls[0].yStart,
                           std::nullopt});
    put(std::move(x));
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
