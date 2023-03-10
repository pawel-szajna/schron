#include "level.hpp"

#include <spdlog/spdlog.h>

namespace world
{
Level::Level(int id,
             std::string name) :
     name(std::move(name))
{
    spdlog::info("Loading level #{}", id);
}

void Level::put(Sector&& sector)
{
    if (map.contains(sector.id))
    {
        spdlog::warn("Duplicate sector {}", sector.id);
    }
    map.emplace(sector.id, std::move(sector));
}

std::string Level::toLua() const
{
    std::string output{};
    for (const auto& [_, sector] : map)
    {
        output += sector.toLua();
    }
    return output;
}
}
