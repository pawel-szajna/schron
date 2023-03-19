#include "sector.hpp"

#include "util/format.hpp"

namespace world
{
std::string Sector::toLua() const
{
    std::string output{};
    double x1 = walls[0].xStart, y1 = walls[0].yStart;
    std::string builder = std::format("PolygonalSectorBuilder.new({},{})", x1, y1);
    builder += std::format(":withId({})", id);
    builder += std::format(":withCeiling({})", ceiling);
    builder += std::format(":withFloor({})", floor);
    for (const auto& wall : walls)
    {
        builder += wall.toLua();
    }
    output += std::format("world_put({})\n", builder);
    for (const auto& sprite : sprites)
    {
        output += sprite.toLua(id);
    }
    for (const auto& light : lights)
    {
        output += light.toLua(id);
    }
    return output;
}

Sector::Sector(int id,
       std::vector<Wall> walls,
       std::vector<Sprite> sprites,
       std::vector<Light> lights,
       double ceiling,
       double floor,
       std::string ceilingTexture,
       std::string floorTexture) :
    id(id),
    walls(std::move(walls)),
    sprites(std::move(sprites)),
    lights(std::move(lights)),
    ceiling(ceiling),
    floor(floor),
    ceilingTexture(std::move(ceilingTexture)),
    floorTexture(std::move(floorTexture))
{
    recalculateBounds();
}

Sector::~Sector() = default;

void Sector::recalculateBounds()
{
    boundsLeft   = std::min_element(walls.begin(), walls.end(), [](const auto& a, const auto& b) { return a.xStart < b.xStart; })->xStart;
    boundsRight  = std::max_element(walls.begin(), walls.end(), [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; })->xEnd;
    boundsTop    = std::min_element(walls.begin(), walls.end(), [](const auto& a, const auto& b) { return a.yStart < b.yStart; })->yStart;
    boundsBottom = std::max_element(walls.begin(), walls.end(), [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; })->yEnd;
}

std::string Sprite::toLua(int sectorId) const
{
    return std::format("world_sprite({},{},\"{}\",{},{},{},{},{},{},{})\n", sectorId, id, texture, x, y, z, offset, shadows, lightCenter, blocking);
}

std::string Light::toLua(int sectorId) const
{
    return std::format("world_light({},{},{},{},{},{},{})\n", sectorId, x, y, z, r, g, b);
}

std::string Wall::toLua() const
{
    std::string output{};
    if (portal.has_value())
    {
        if (portal->transform.has_value())
        {
            spdlog::error("Transforming portals cannot be exported yet!");
            return "";
        }
        else
        {
            output += std::format(":withPortal({},{},\"{}\",{})", xEnd, yEnd, texture, portal->sector);
        }
    }
    else
    {
        output += std::format(":withWall({},{},\"{}\")", xEnd, yEnd, texture);
    }
    return output;
}
}
