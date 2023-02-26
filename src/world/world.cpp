#include "world.hpp"

#include "level.hpp"
#include "util/constants.hpp"

#include <spdlog/spdlog.h>

namespace world
{
World::World()
{
}

Level& World::level(int id)
{
    if (not levels.contains(id))
    {
        spdlog::info("Requested level {}, but it has not been created yet. Attempting to load.", id);
        levels.emplace(id, Level(c::levelSize, c::levelSize, "Untitled level"));
    }

    return levels.at(id);
}
}
