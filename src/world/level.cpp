#include "level.hpp"

#include "builders.hpp"

#include <numbers>
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

    // constexpr static auto deg90 = 90.0 * std::numbers::pi / 180.0;

    put(PolygonalSectorBuilder(-2, -2)
            .withId(1)
            .withCeiling(1.5)
            .withFloor(0.0)
            .withWall(3, -2, "wall")
            .withWall(3, -1, "wall")
            .withWall(3, 2, "wall", Wall::Portal{2})
            .withWall(-2, 2, "wall")
            .withWall(-3, 1, "wall")
            .withWall(-3, -1, "wall", Wall::Portal{3, Wall::Portal::Transformation{8, 3, -0.5, 0}})
            .withWall(-2, -2, "wall")
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
    map.at(3).walls.at(1).portal = Wall::Portal{1, Wall::Portal::Transformation{-8, -3, 0.5, 0}};
}

void Level::put(Sector&& sector)
{
    if (map.contains(sector.id))
    {
        spdlog::warn("Duplicate sector {}", sector.id);
    }
    map.emplace(sector.id, std::move(sector));
}
}
