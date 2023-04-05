#include "world_bindings.hpp"

#include "world/builders.hpp"
#include "world/sector.hpp"
#include "world/world.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace scripting
{
WorldBindings::WorldBindings(sol::state& lua, world::World& world) :
    lua(lua),
    world(world)
{
    lua.set_function("sprite_create",  &WorldBindings::sprite, this);
    lua.set_function("sprite_texture", &WorldBindings::spriteTexture, this);

    lua.set_function("light_create", &WorldBindings::light, this);

    lua.set_function("sector_create",    &WorldBindings::create, this);
    lua.set_function("sector_wall",      &WorldBindings::addWall, this);
    lua.set_function("sector_portal",    &WorldBindings::addPortal, this);
    lua.set_function("sector_transform", &WorldBindings::addTransform, this);

    lua.set_function("change_texture", &WorldBindings::changeTexture, this);
    lua.set_function("load_texture",   &WorldBindings::loadTexture, this);

    lua.set_function("interactive_point", &WorldBindings::interactivePoint, this);
}

void WorldBindings::create(int sectorId,
                           double floor, std::string floorTexture,
                           double ceiling, std::string ceilingTexture)
{
    world::Sector sector{sectorId,
                         {}, {}, {},
                         ceiling, floor,
                         std::move(ceilingTexture), std::move(floorTexture)};
    world.level(1).put(sector);
}

void WorldBindings::addWall(int sectorId, std::string texture, double x1, double y1, double x2, double y2)
{
    auto& sector = world.level(1).map.at(sectorId);
    sector.walls.push_back({x1, y1, x2, y2, std::nullopt, std::move(texture)});
    sector.recalculateBounds();
}

void WorldBindings::addPortal(int sectorId, std::string texture, double x1, double y1, double x2, double y2, int target)
{
    auto& sector = world.level(1).map.at(sectorId);
    sector.walls.push_back({x1, y1, x2, y2, world::Wall::Portal{target, std::nullopt}, std::move(texture)});
    sector.recalculateBounds();
}

void WorldBindings::addTransform(int sectorId, std::string texture,
                                 double x1, double y1, double x2, double y2, int target,
                                 double transformX, double transformY, double transformZ, double transformAngle)
{
    auto& sector = world.level(1).map.at(sectorId);
    sector.walls.push_back({x1, y1,
                            x2, y2,
                            world::Wall::Portal{target,
                                                world::Wall::Portal::Transformation{transformX, transformY, transformZ,
                                                                                    transformAngle}},
                            std::move(texture)});
    sector.recalculateBounds();
}

void WorldBindings::sprite(int sectorId, int id,
                           std::string texture,
                           double x, double y, double z,
                           double offset,
                           bool shadows,
                           double lightCenter,
                           bool blocking)
{
    spdlog::debug("WorldBindings::sprite(sectorId={}, id={}, texture={}, ...)", sectorId, id, texture);
    auto& sector = world.level(1).map.at(sectorId);
    sector.sprites.push_back(world::Sprite{.id = id,
                                           .textures = {{0, std::move(texture)}},
                                           .x = x,
                                           .y = y,
                                           .z = z,
                                           .w = 1.0,
                                           .h = 1.0,
                                           .offset = offset,
                                           .shadows = shadows,
                                           .lightCenter = lightCenter,
                                           .blocking = blocking});
}

void WorldBindings::spriteTexture(int sectorId, int id, double angle, std::string texture)
{
    world.level(1).map.at(sectorId).sprites.at(id).textures.push_back({angle, std::move(texture)});
}

void WorldBindings::changeTexture(int sectorId, int spriteId, std::string texture)
try
{
    auto& sector = world.level(1).map.at(sectorId);
    sector.sprites.at(spriteId).textures[0].texture = std::move(texture);
}
catch (std::exception& e)
{
    spdlog::warn("Failure during texture change: {}", e.what());
}

void WorldBindings::loadTexture(std::string texture)
{
    world.level(1).additionalTextures.emplace(std::move(texture));
}

void WorldBindings::light(int sectorId, double x, double y, double z, double r, double g, double b)
{
    auto& sector = world.level(1).map.at(sectorId);
    sector.lights.push_back(world::Light{x, y, z, r, g, b});
}

void WorldBindings::interactivePoint(int sectorId, double x, double y, const std::string& script)
{
    world.level(1).interaction(sectorId, x, y, script);
}
}
