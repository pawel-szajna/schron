#include "mini_map.hpp"

#include "game/player.hpp"
#include "sdlwrapper/font.hpp"
#include "util/constants.hpp"
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
constexpr auto mapLeft = c::windowWidth - mapWidth - mapMargin;
constexpr auto mapScale = 32;

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

void MiniMap::event(const sdl::event::Event& event)
{}

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

    constexpr static auto gridRange = mapWidth / mapScale;

    auto angleSin = std::sin(position.angle - std::numbers::pi);
    auto angleCos = std::cos(position.angle - std::numbers::pi);

    auto drawGridLine = [&](double x1, double x2, double y1, double y2)
    {
        double transformedX1 = x1 * angleSin - y1 * angleCos;
        double transformedX2 = x2 * angleSin - y2 * angleCos;
        double transformedY1 = x1 * angleCos + y1 * angleSin;
        double transformedY2 = x2 * angleCos + y2 * angleSin;

        transformedX1 = mapScale * transformedX1 + mapWidth / 2;
        transformedX2 = mapScale * transformedX2 + mapWidth / 2;
        transformedY1 = mapScale * transformedY1 + mapHeight / 2;
        transformedY2 = mapScale * transformedY2 + mapHeight / 2;

        surfaceRenderer.renderLine(transformedX1, transformedY1, transformedX2, transformedY2, 255, 255, 255, 64);
    };

    {
        double y1 = -gridRange - position.y + std::floor(position.y);
        double y2 = gridRange - position.y + std::floor(position.y);
        for (int x = -gridRange; x <= gridRange; ++x)
        {
            double x1 = x - position.x + std::floor(position.x);
            drawGridLine(x1, x1, y1, y2);
        }
    }

    {
        double x1 = -gridRange - position.x + std::floor(position.x);
        double x2 = gridRange - position.x + std::floor(position.x);
        for (int y = -gridRange; y <= gridRange; ++y)
        {
            double y1 = y - position.y + std::floor(position.y);
            drawGridLine(x1, x2, y1, y1);
        }
    }

    sectorQueue.push({.id = position.sector,
                      .recursion = 5,
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
                           double transformedX = wall.xStart - position.x - current.transformX;
                           double transformedY = wall.yStart - position.y - current.transformY;

                           auto rotatedX = transformedX * angleSin - transformedY * angleCos;
                           auto rotatedY = transformedX * angleCos + transformedY * angleSin;

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
                        newTransformX += wall.portal->transform->x;
                        newTransformY += wall.portal->transform->y;
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

    const static std::vector<sdl::Vertex> playerArrow
    {
        sdl::Vertex{mapWidth / 2 - mapScale / 5, mapHeight / 2 + mapScale / 5, 255, 192, 192, 90, 0, 0},
        sdl::Vertex{mapWidth / 2, mapHeight / 2 - mapScale / 5, 255, 192, 192, 90, 0, 0},
        sdl::Vertex{mapWidth / 2 + mapScale / 5, mapHeight / 2 + mapScale / 5, 255, 192, 192, 90, 0, 0}
    };
    surfaceRenderer.renderGeometry(playerArrow);


    surfaceRenderer.present();

    auto font = sdl::Font("res/font/KellySlab.ttf", 18);
    auto text = font.render(std::format("x={:.2} y={:.2} z={:.2}", position.x, position.y, position.z),
                            sdl::Color{255, 255, 255, 200});
    text.render(surface, sdl::Rectangle{0, mapHeight - text.height});

    surface.render(texture);
    renderer.copy(texture, std::nullopt, sdl::FRectangle{mapLeft, mapTop, mapWidth, mapHeight});
}
}