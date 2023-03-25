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

    WorldBindings(sol::state& lua, world::World& world);

private:

    void create(int sectorId, double floor, std::string floorTexture, double ceiling, std::string ceilingTexture);
    void addWall(int sectorId, std::string texture, double x1, double y1, double x2, double y2);
    void addPortal(int sectorId, std::string texture, double x1, double y1, double x2, double y2, int target);
    void addTransform(int sectorId, std::string texture, double x1, double y1, double x2, double y2, int target,
                      double transformX, double transformY, double transformZ, double transformAngle);

    void sprite(int sectorId, int id, std::string texture, double x, double y, double z, double offset, bool shadows, double lightCenter, bool blocking);
    void light(int sectorId, double x, double y, double z, double r, double g, double b);
    void changeTexture(int sectorId, int spriteId, std::string texture);

    void interactivePoint(int sectorId, double x, double y, const std::string& script);

    [[maybe_unused]] sol::state& lua;
    world::World& world;
};
}
