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

void Editor::render(sdl::Renderer& renderer)
{
    std::vector<sdl::Vertex> vertices{};

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
    }
}
}
