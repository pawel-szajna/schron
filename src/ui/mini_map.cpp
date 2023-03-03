#include "mini_map.hpp"

#include "game/player.hpp"
#include "world/level.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <queue>
#include <vector>

namespace ui
{
namespace
{
constexpr auto mapWidth = 300;
constexpr auto mapHeight = 240;
constexpr auto mapMargin = 32;
constexpr auto mapTop = mapMargin;
constexpr auto mapLeft = 1024 - mapWidth - mapMargin;
constexpr auto mapScale = 40;

constexpr sdl::Vertex grayVertex(float x, float y, uint8_t intensity, uint8_t alpha)
{
    return sdl::Vertex{x, y, intensity, intensity, intensity, alpha, 0, 0};
}

struct MapSector
{
    int id;
    int recursion;
    int caller;
    double transformX;
    double transformY;
};
}

MiniMap::MiniMap(sdl::Renderer& renderer, const world::Level& level, const game::Player& player) :
    level(level),
    player(player),
    surface(mapWidth, mapHeight),
    surfaceRenderer(surface),
    texture(renderer, sdl::Texture::Access::Streaming, mapWidth, mapHeight)
{
    texture.setBlendMode(sdl::BlendMode::Add);
}

MiniMap::~MiniMap() = default;

void MiniMap::render(sdl::Renderer& renderer)
{
    const auto& position = player.getPosition();

    std::vector<sdl::Vertex> vertices{};
    std::queue<MapSector> sectorQueue{};

    constexpr static auto boxColor = 204;
    constexpr static auto boxAlpha = 80;
    const static std::vector<sdl::Vertex> box{grayVertex(       0,         0, boxColor, boxAlpha),
                                              grayVertex(mapWidth,         0, boxColor, boxAlpha),
                                              grayVertex(mapWidth, mapHeight, boxColor, boxAlpha),
                                              grayVertex(       0, mapHeight, boxColor, boxAlpha)};
    surfaceRenderer.clear();
    surfaceRenderer.renderGeometry(box);

    sectorQueue.push({.id = position.sector,
                      .recursion = 3,
                      .caller = -1,
                      .transformX = 0,
                      .transformY = 0});

    while (not sectorQueue.empty())
    {
        auto current = sectorQueue.front();
        sectorQueue.pop();
        const auto& sector = level.sector(current.id);
        vertices.clear();
        std::transform(sector.walls.begin(),
                       sector.walls.end(),
                       std::back_inserter(vertices),
                       [&](const auto& wall)
                       {
                           auto angle = position.angle - std::numbers::pi;

                           double transformedX = wall.xStart - position.x - current.transformX;
                           double transformedY = wall.yStart - position.y - current.transformY;

                           auto rotatedX = transformedX * std::sin(angle) - transformedY * std::cos(angle);
                           auto rotatedY = transformedX * std::cos(angle) + transformedY * std::sin(angle);

                           auto vertexX = (float)(rotatedX * mapScale + mapWidth / 2);
                           auto vertexY = (float)(rotatedY * mapScale + mapHeight / 2);

                           return sdl::Vertex{vertexX, vertexY, 255, 255, 255, 52};
                       });
        surfaceRenderer.renderGeometry(vertices);

        if (current.recursion > 0)
        {
            for (const auto& wall : sector.walls)
            {
                if (wall.portal.has_value() and wall.portal->sector != current.caller)
                {
                    double newTransformX = current.transformX;
                    double newTransformY = current.transformY;
                    if (wall.portal->transform.has_value())
                    {
                        newTransformX -= wall.portal->transform->x;
                        newTransformY -= wall.portal->transform->y;
                    }
                    sectorQueue.push({.id = wall.portal->sector,
                                      .recursion = current.recursion - 1,
                                      .caller = current.id,
                                      .transformX = newTransformX,
                                      .transformY = newTransformY});
                }
            }
        }
    }

    surfaceRenderer.present();
    surface.render(texture);
    renderer.copy(texture, std::nullopt, sdl::Rectangle{mapLeft, mapTop, mapWidth, mapHeight});
}
}