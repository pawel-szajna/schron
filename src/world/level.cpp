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

    put(RectangularSectorBuilder()
            .withId(1)
            .withDimensions(0, 0, 1, 3)
            .withSouthNeighbour(2, 1, 3, 0, 3)
            .build());
    auto s2 = RectangularSectorBuilder()
            .withId(2)
            .withDimensions(-1, 3, 2, 6)
            .withNorthNeighbour(1, 0, 3, 1, 3)
            .withSouthNeighbour(3, 1, 6, 0, 6)
            .build();
    s2.sprites.push_back({0, "res/gfx/sprites/lamp.png", 0.5, 4.5, 0.5, 1.0, 1.0});
    put(std::move(s2));
    put(RectangularSectorBuilder()
            .withId(3)
            .withDimensions(0, 6, 1, 10) // y2 = 9 to miejsce, gdzie jest blokada w oryginalnej grze, ale korytarz istnieje dalej
            .withNorthNeighbour(2, 0, 6, 1, 6)
            .withEastNeighbour(4, 1, 8, 1, 9)
            .build());
    put(RectangularSectorBuilder()
            .withId(4)
            .withDimensions(1, 8, 7 /* 15 */, 9)
            .withWestNeighbour(3, 1, 9, 1, 8)
            .withSouthNeighbour(5, 4, 9, 3, 9) // do kibla
            .withNorthNeighbour(12, 4, 8, 5, 8) // do czyjegoś pokoju
            .withEastNeighbour(14, 7, 8, 7, 9)
            .build());
    put(RectangularSectorBuilder()
            .withId(14)
            .withDimensions(7, 8, 11, 9)
            .withWestNeighbour(4, 7, 9, 7, 8)
            .withNorthNeighbour(15, 8, 8, 9, 8) // do pokoju Oli
            .withSouthNeighbour(17, 8, 9, 7, 9) // do polybiusów
            .build());

    // Kibel
    put(RectangularSectorBuilder()
            .withId(5)
            .withDimensions(3, 9, 4, 12/*16*/)
            .withNorthNeighbour(4, 3, 9, 4, 9)
            .withEastNeighbour(7, 4, 10, 4, 11)
            .withWestNeighbour(6, 3, 12, 3, 10)
            .withSouthNeighbour(8, 4, 12, 3, 12)
            .build());
    put(RectangularSectorBuilder()
            .withId(6)
            .withDimensions(2, 10, 3, 12)
            .withEastNeighbour(5, 3, 10, 3, 12)
            .build());
    put(RectangularSectorBuilder()
            .withId(7)
            .withDimensions(4, 10, 5, 11)
            .withWestNeighbour(5, 4, 11, 4, 10)
            .build());
    put(RectangularSectorBuilder()
            .withId(8)
            .withDimensions(3, 12, 5, 13)
            .withNorthNeighbour(5, 3, 12, 4, 12)
            .withSouthNeighbour(9, 4, 13, 3, 13)
            .build());
    put(RectangularSectorBuilder()
            .withId(9)
            .withDimensions(2, 13, 4, 14)
            .withNorthNeighbour(8, 3, 13, 4, 13)
            .withSouthNeighbour(10, 4, 14, 3, 14)
            .build());
    put(RectangularSectorBuilder()
            .withId(10)
            .withDimensions(3, 14, 5, 16)
            .withNorthNeighbour(9, 3, 14, 4, 14)
            .withWestNeighbour(11, 3, 16, 3, 15)
            .build());
    put(RectangularSectorBuilder()
            .withId(11)
            .withDimensions(2, 15, 3, 16)
            .withEastNeighbour(10, 3, 15, 3, 16)
            .build());

    // Pokój jakiś
    put(RectangularSectorBuilder()
            .withId(12)
            .withDimensions(4, 7, 5, 8)
            .withSouthNeighbour(4, 5, 8, 4, 8)
            .withNorthNeighbour(13, 4, 7, 5, 7)
            .build());
    put(RectangularSectorBuilder()
            .withId(13)
            .withDimensions(3, 4, 6, 7)
            .withSouthNeighbour(12, 5, 7, 4, 7)
            .build());

    // Pokój Oli
    put(RectangularSectorBuilder()
                .withId(15)
                .withDimensions(8, 7, 9, 8)
                .withSouthNeighbour(14, 9, 8, 8, 8)
                .withNorthNeighbour(16, 8, 7, 9, 7)
                .build());
    put(RectangularSectorBuilder()
                .withId(16)
                .withDimensions(7, 4, 10, 7)
                .withSouthNeighbour(15, 9, 7, 8, 7)
                .build());

    // Pokój z polybiusem
    put(RectangularSectorBuilder()
                .withId(17)
                .withDimensions(7, 9, 8, 10)
                .withNorthNeighbour(14, 7, 9, 8, 9)
                .withSouthNeighbour(18, 8, 10, 7, 10)
                .build());
    put(RectangularSectorBuilder()
                .withId(18)
                .withDimensions(6, 10, 9, 13)
                .withNorthNeighbour(17, 7, 10, 8, 10)
                .build());
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
