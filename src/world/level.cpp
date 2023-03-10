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
