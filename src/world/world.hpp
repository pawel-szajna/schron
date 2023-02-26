#pragma once

#include "level.hpp"

#include <map>

namespace world
{
class World
{
public:

    using LevelsMap = std::map<int, Level>;

    World();

    Level& level(int id);

private:
    LevelsMap levels{};
};
}
