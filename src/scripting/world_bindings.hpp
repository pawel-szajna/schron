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
    void sprite(int sectorId, int id, std::string texture, double x, double y, double z, double offset, bool shadows, double lightCenter, bool blocking);
    void light(int sectorId, double x, double y, double z, double r, double g, double b);
    void changeTexture(int sectorId, int spriteId, std::string texture);

    void interactivePoint(int sectorId, double x, double y, const std::string& script);

    [[maybe_unused]] sol::state& lua;
    world::World& world;
};
}
