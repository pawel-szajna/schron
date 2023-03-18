#include "world_bindings.hpp"

#include "world/builders.hpp"
#include "world/sector.hpp"
#include "world/world.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace scripting
{
WorldBindings::WorldBindings(sol::state&lua, world::World& world) :
    lua(lua),
    world(world)
{
    lua.set_function("world_put", &WorldBindings::put, this);
    lua.set_function("world_sprite", &WorldBindings::sprite, this);
    lua.set_function("world_light", &WorldBindings::light, this);

    sol::usertype<world::PolygonalSectorBuilder> polygonalSectorBuilderType =
        lua.new_usertype<world::PolygonalSectorBuilder>("PolygonalSectorBuilder",
                                                        sol::constructors<world::PolygonalSectorBuilder(double, double)>());
    polygonalSectorBuilderType["withId"] = &world::PolygonalSectorBuilder::withId;
    polygonalSectorBuilderType["withCeiling"] = &world::PolygonalSectorBuilder::withCeiling;
    polygonalSectorBuilderType["withFloor"] = &world::PolygonalSectorBuilder::withFloor;
    polygonalSectorBuilderType["withWall"] = &world::PolygonalSectorBuilder::withWall;
    polygonalSectorBuilderType["withPortal"] = &world::PolygonalSectorBuilder::withPortal;
    polygonalSectorBuilderType["build"] = &world::PolygonalSectorBuilder::build;

    sol::usertype<world::RectangularSectorBuilder> rectangularSectorBuilderType =
        lua.new_usertype<world::RectangularSectorBuilder>("RectangularSectorBuilder",
                                                          sol::constructors<world::RectangularSectorBuilder(int)>());
    rectangularSectorBuilderType["withDimensions"] = &world::RectangularSectorBuilder::withDimensions;
    rectangularSectorBuilderType["withCeiling"] = &world::RectangularSectorBuilder::withCeiling;
    rectangularSectorBuilderType["withFloor"] = &world::RectangularSectorBuilder::withFloor;
    rectangularSectorBuilderType["withNorthNeighbour"] = &world::RectangularSectorBuilder::withNorthNeighbour;
    rectangularSectorBuilderType["withEastNeighbour"] = &world::RectangularSectorBuilder::withEastNeighbour;
    rectangularSectorBuilderType["withSouthNeighbour"] = &world::RectangularSectorBuilder::withSouthNeighbour;
    rectangularSectorBuilderType["withWestNeighbour"] = &world::RectangularSectorBuilder::withWestNeighbour;
    rectangularSectorBuilderType["build"] = &world::RectangularSectorBuilder::build;
}

void WorldBindings::put(world::SectorBuilder& builder)
{
    auto sector = builder.build();
    spdlog::debug("WorldBindings::put(sector={{id:{}}})", sector.id);
    world.level(1).put(std::move(sector));
}

void WorldBindings::sprite(int sectorId, int id,
                           std::string texture,
                           double x, double y, double z,
                           double offset,
                           bool shadows,
                           double lightCenter)
{
    spdlog::debug("WorldBindings::sprite(sectorId={}, id={}, texture={}, ...)", sectorId, id, texture);
    auto& sector = const_cast<world::Sector&>(world.level(1).sector(sectorId));
    sector.sprites.push_back(world::Sprite{.id = id,
                                           .texture = std::move(texture),
                                           .x = x,
                                           .y = y,
                                           .z = z,
                                           .w = 1.0,
                                           .h = 1.0,
                                           .offset = offset,
                                           .shadows = shadows,
                                           .lightCenter = lightCenter});
}

void WorldBindings::light(int sectorId, double x, double y, double z, double r, double g, double b)
{
    auto& sector = const_cast<world::Sector&>(world.level(1).sector(sectorId));
    sector.lights.push_back(world::Light{x, y, z, r, g, b});
}
}
