#pragma once

#include <string>

namespace sdl
{
class Renderer;
}

namespace sol
{
class state;
}

namespace world
{
class SectorBuilder;
class Sprite;
class World;
}

namespace scripting
{
class WorldBindings
{
public:

    WorldBindings(sol::state &lua, world::World &world);

private:

    void put(world::SectorBuilder& builder);
    void sprite(int sectorId, int id, std::string texture, double x, double y);

    sol::state& lua;
    world::World& world;
};
}
