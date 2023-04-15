#include "world.hpp"

#include "level.hpp"
#include "scripting/scripting.hpp"
#include "util/format.hpp"

#include <stdexcept>

namespace world
{
World::World() = default;

Level& World::level(int id)
{
    if (not levels.contains(id))
    {
        throw std::invalid_argument{std::format("Requested non-existing level {}", id)};
    }

    return levels.at(id);
}

void World::loadLevel(int id, scripting::Scripting& scripting)
{
    levels.try_emplace(id, id, "Untitled level");
    scripting.run(std::format("scripts/levels/{}/map.lua", id));
    scripting.run(std::format("scripts/levels/{}/script.lua", id));
}
} // namespace world
