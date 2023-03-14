#include "editor.hpp"

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"
#include "world/level.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, const double& playerX, const double& playerY) :
    playerX(playerX),
    playerY(playerY),
    level(level)
{
    sdl::showCursor();
}

Editor::~Editor()
{
    sdl::hideCursor();
}

void Editor::event(const sdl::event::Event& event)
{
    if (std::holds_alternative<sdl::event::Mouse>(event))
    {
        auto& mouse = std::get<sdl::event::Mouse>(event);
        mouseX = mouse.x;
        mouseY = mouse.y;
        if (not dragging and mouse.button.has_value() and *mouse.button == 1)
        {
            btnX = mouseX;
            btnY = mouseY;
            dragging = true;
        }
        if (mouse.button.has_value() and *mouse.button == -1)
        {
            dragging = false;
        }
        if (mouse.scroll.has_value())
        {
            mapScale -= *mouse.scroll;
        }
    } else if (std::holds_alternative<sdl::event::Key>(event))
    {
        auto& key = std::get<sdl::event::Key>(event);
        if (key.scancode == 60 /*SDL_SCANCODE_F3*/) // TODO: unwrapped SDL
        {
            auto& sectorCeiling = level.map.at(2).ceiling;
            enqueue(500, sectorCeiling, sectorCeiling + 0.5, [&sectorCeiling](const auto ceiling) { sectorCeiling = ceiling; });
        }
    }
}

void Editor::enqueue(int length, double startValue, double targetValue, std::function<void(double)> applier)
{
    auto now = sdl::currentTime();
    diffs.emplace_back(Diff{now, now + length, startValue, targetValue, std::move(applier)});
}

template<typename T>
bool Editor::mouseWithin(const T& vertices, VertexGetter<T> vg) const
{
    int sideSign{};

    while (auto line = vg(vertices))
    {
        auto x1 = std::get<0>(*line);
        auto y1 = std::get<1>(*line);
        auto x2 = std::get<2>(*line);
        auto y2 = std::get<3>(*line);

        auto side = (mouseX - x1) * (y2 - y1) - (mouseY - y1) * (x2 - x1);

        if (sideSign == 0)
        {
            sideSign = side < 0 ? -1 : 1;
        }
        else
        {
            if ((side < 0 and sideSign > 0) or (side > 0 and sideSign < 0))
            {
                return false;
            }
        }
    }

    return true;
}

void Editor::updateMouse()
{
    if (dragging)
    {
        mapX = mapX - (mouseX - btnX);
        mapY = mapY - (mouseY - btnY);
        btnX = mouseX;
        btnY = mouseY;
    }

    for (const auto& [id, sector] : level.map)
    {
        using Walls = decltype(sector.walls);
        if (mouseWithin<Walls>(sector.walls,
                               [&, n = (size_t)0](const Walls& walls) mutable -> VertexGetter<Walls>::result_type
                               {
                                   if (n >= walls.size())
                                   {
                                       return std::nullopt;
                                   }
                                   const auto& wall = walls.at(n++);
                                   return std::make_tuple(wall.xStart * mapScale - mapX,
                                                          wall.yStart * mapScale - mapY,
                                                          wall.xEnd * mapScale - mapX,
                                                          wall.yEnd * mapScale - mapY);
                               }))
        {
            sectorUnderMouse = id;
            break;
        }
    }
}

void Editor::processMapUpdates()
{
    auto now = sdl::currentTime();

    while (not diffs.empty() and diffs.front().targetTime <= now)
    {
        diffs.pop_front();
    }

    for (const auto& diff : diffs)
    {
        diff.applier(diff.startValue + (now - diff.startTime) * (diff.targetValue - diff.startValue) / (diff.targetTime - diff.startTime));
    }
}

void Editor::drawMap(sdl::Renderer& renderer)
{
    std::vector<sdl::Vertex> vertices{};
    for (auto& [id, sector] : level.map)
    {
        vertices.clear();
        std::transform(sector.walls.begin(),
                       sector.walls.end(),
                       std::back_inserter(vertices),
                       [&, id = id](const auto& wall)
                       {
                           float vertexX = wall.xStart * mapScale - mapX;
                           float vertexY = wall.yStart * mapScale - mapY;
                           uint8_t color = sectorUnderMouse == id ? 150 : 100;
                           return sdl::Vertex{vertexX, vertexY, color, color, color, 220};
                       });
        renderer.renderGeometry(vertices);

        for (const auto& wall : sector.walls)
        {
            uint8_t r = 50, g = 50, b = 50, a = 240;
            if (wall.portal.has_value())
            {
                a = 220;
                if (wall.portal->transform.has_value())
                {
                    r = 200;
                }
                else
                {
                    r = 90;
                    g = 100;
                    b = 135;
                }
            }
            renderer.renderLine(mapScale * wall.xStart - mapX, mapScale * wall.yStart - mapY,
                                mapScale * wall.xEnd - mapX, mapScale * wall.yEnd - mapY,
                                r, g, b, a);
        }
        for (const auto& sprite : sector.sprites)
        {
            uint8_t r = 140, g = 170, b = 190, a = 200;
            vertices = {{(float)(mapScale * sprite.x - mapX - (mapScale / 2)), (float)(mapScale * sprite.y - mapY), r, g, b, a},
                        {(float)(mapScale * sprite.x - mapX), (float)(mapScale * sprite.y - mapY - (mapScale / 2)), r, g, b, a},
                        {(float)(mapScale * sprite.x - mapX + (mapScale / 2)), (float)(mapScale * sprite.y - mapY), r, g, b, a},
                        {(float)(mapScale * sprite.x - mapX), (float)(mapScale * sprite.y - mapY + (mapScale / 2)), r, g, b, a}};
            renderer.renderGeometry(vertices);
        }
    }
}

void Editor::render(sdl::Renderer& renderer)
{
    processMapUpdates();
    updateMouse();
    drawMap(renderer);
}
}
