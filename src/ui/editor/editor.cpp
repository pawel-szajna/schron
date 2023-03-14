#include "editor.hpp"

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"
#include "world/level.hpp"

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
        auto mouse = std::get<sdl::event::Mouse>(event);
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
    }
}

void Editor::enqueue(int length, double startValue, double targetValue, std::function<void(double)> applier)
{
    auto now = sdl::currentTime();
    diffs.emplace_back(Diff{now, now + length, startValue, targetValue, std::move(applier)});
}

void Editor::render(sdl::Renderer& renderer)
{
    std::vector<sdl::Vertex> vertices{};
    auto now = sdl::currentTime();

    while (not diffs.empty() and diffs.front().targetTime <= now)
    {
        diffs.pop_front();
    }

    for (const auto& diff : diffs)
    {
        diff.applier(diff.startValue + (now - diff.startTime) * (diff.targetValue - diff.startValue) / (diff.targetTime - diff.startTime));
    }

    if (dragging)
    {
        mapX = mapX - (mouseX - btnX);
        mapY = mapY - (mouseY - btnY);
        btnX = mouseX;
        btnY = mouseY;
    }

    for (auto& [_, sector] : level.map)
    {
        vertices.clear();
        bool mouseInSector{true};
        int sideSign{};
        for (const auto& wall : sector.walls)
        {
            double mapMouseX = (double)(mouseX + mapX) / mapScale;
            double mapMouseY = (double)(mouseY + mapY) / mapScale;

            auto side = (mapMouseX - wall.xStart) * (wall.yEnd - wall.yStart) - (mapMouseY - wall.yStart) * (wall.xEnd - wall.xStart);
            if (sideSign == 0)
            {
                sideSign = side < 0 ? -1 : 1;
            }
            else
            {
                if ((side < 0 and sideSign > 0) or (side > 0 and sideSign < 0))
                {
                    mouseInSector = false;
                    break;
                }
            }
        }
        std::transform(sector.walls.begin(),
                       sector.walls.end(),
                       std::back_inserter(vertices),
                       [&](const auto& wall)
                       {
                           float vertexX = wall.xStart * mapScale - mapX;
                           float vertexY = wall.yStart * mapScale - mapY;
                           uint8_t color = mouseInSector ? 150 : 100;
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
}
