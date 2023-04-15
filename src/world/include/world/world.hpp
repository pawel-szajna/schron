#pragma once

#include "level.hpp"

#include <map>

namespace scripting
{
class Scripting;
}

namespace world
{
class World
{
public:

    using LevelsMap = std::map<int, Level>;

    World();

    Level& level(int id);
    void loadLevel(int id, scripting::Scripting& scripting);

private:

    LevelsMap levels{};
};
} // namespace world
