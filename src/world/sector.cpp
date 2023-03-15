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

std::string Sprite::toLua(int sectorId) const
{
    return std::format("world_sprite({},{},\"{}\",{},{})\n", sectorId, id, texture, x, y);
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
