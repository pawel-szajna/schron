#include "sector.hpp"

#include "util/format.hpp"

namespace world
{
std::string Sector::toLua() const
{
    std::string output{std::format("sector_create({},{},\"{}\",{},\"{}\")\n",
                                   id, floor, floorTexture, ceiling, ceilingTexture)};
    for (const auto& wall : walls)
    {
        output += wall.toLua(id);
    }
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
    if (not walls.empty())
    {
        boundsLeft = std::min_element(walls.begin(), walls.end(),
                                      [](const auto& a, const auto& b) { return a.xStart < b.xStart; })->xStart;
        boundsRight = std::max_element(walls.begin(), walls.end(),
                                       [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; })->xEnd;
        boundsTop = std::min_element(walls.begin(), walls.end(),
                                     [](const auto& a, const auto& b) { return a.yStart < b.yStart; })->yStart;
        boundsBottom = std::max_element(walls.begin(), walls.end(),
                                        [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; })->yEnd;
    }
}

std::string Sprite::toLua(int sectorId) const
{
    std::string output{std::format("  sprite_create({},{},\"{}\",{},{},{},{},{},{},{})\n",
                                   sectorId, id, texture(0), x, y, z, offset, shadows, lightCenter, blocking)};
    for (const auto& [angle, texture] : textures)
    {
        if (angle <= 0)
        {
            continue;
        }
        output += std::format("    sprite_texture({},{},{},\"{}\")\n",
                              sectorId, id, angle, texture);
    }
    return output;
}

std::string Light::toLua(int sectorId) const
{
    return std::format("  light_create({},{},{},{},{},{},{})\n", sectorId, x, y, z, r, g, b);
}

std::string Wall::toLua(int sectorId) const
{
    if (portal.has_value())
    {
        if (portal->transform.has_value())
        {
            return std::format("  sector_transform({},\"{}\",{},{},{},{},{},{},{},{},{})\n",
                               sectorId, texture, xStart, yStart, xEnd, yEnd, portal->sector,
                               portal->transform->x, portal->transform->y, portal->transform->z,
                               portal->transform->angle);
        }
        else
        {
            return std::format("  sector_portal({},\"{}\",{},{},{},{},{})\n",
                               sectorId, texture, xStart, yStart, xEnd, yEnd, portal->sector);
        }
    }
    else
    {
        return std::format("  sector_wall({},\"{}\",{},{},{},{})\n",
                           sectorId, texture, xStart, yStart, xEnd, yEnd);
    }
    return "";
}

const std::string& Sprite::texture(double angle) const
{
    auto it = std::find_if(textures.begin(), textures.end(),
                           [angle](const auto& t) { return t.angle > angle; });
    return (--it)->texture;
}
}
