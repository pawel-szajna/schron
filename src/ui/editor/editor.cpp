#include "editor.hpp"

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/font.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "sdlwrapper/surface.hpp"
#include "util/constants.hpp"
#include "world/level.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, sdl::Font& font) :
    level(level),
    font(font)
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
            dragX = btnX = mouseX;
            dragY = btnY = mouseY;
            dragging = true;
        }
        if (mouse.button.has_value() and *mouse.button == -1)
        {
            dragging = false;
            if (dragX == btnX and dragY == btnY)
            {
                clicked = true;
            }
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
        mapX = mapX - (mouseX - dragX);
        mapY = mapY - (mouseY - dragY);
        dragX = mouseX;
        dragY = mouseY;
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
            if (clicked)
            {
                selectedSector = id;
                enqueue(1200, 0, 1199,
                        [&, id = id, originalFloor = sector.floorTexture, originalCeiling = sector.ceilingTexture](double time)
                        {
                            auto& s = level.map.at(id);
                            if ((int)(time) % 400 < 200)
                            {
                                s.ceilingTexture = "highlight";
                                s.floorTexture = "highlight";
                            }
                            else
                            {
                                s.ceilingTexture = originalCeiling;
                                s.floorTexture = originalFloor;
                            }
                        });
                clicked = false;
            }
            break;
        }
    }

    if (clicked)
    {
        selectedSector = std::nullopt;
        clicked = false;
    }
}

void Editor::processMapUpdates()
{
    auto now = sdl::currentTime();

    while (not diffs.empty() and diffs.front().targetTime <= now)
    {
        diffs.front().applier(diffs.front().targetValue);
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
                           uint8_t color = 100;
                           if (sectorUnderMouse == id) color = 150;
                           if (selectedSector == id) color = 170;
                           return sdl::Vertex{vertexX, vertexY,
                                              color, color, color,
                                              selectedSector == id ? (uint8_t)255 : (uint8_t)220};
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
            if (selectedSector == id)
            {
                a = 255;
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

void Editor::drawSelectedSector(sdl::Renderer& renderer)
{
    if (not selectedSector.has_value())
    {
        return;
    }

    auto& sector = level.map.at(*selectedSector);
    auto rightX = *std::max_element(sector.walls.begin(), sector.walls.end(),
                                    [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
    auto topY = *std::min_element(sector.walls.begin(), sector.walls.end(),
                                  [](const auto& a, const auto& b) { return a.yStart < b.yStart; });

    int windowX = rightX.xEnd * mapScale - mapX + 16;
    int windowY = topY.yStart * mapScale - mapY;
    int windowW = 260;
    int windowH = 200;
    renderer.renderRectangle(windowX, windowY, windowW, windowH, 200, 200, 200, 220);
    auto window = sdl::Surface(windowW, windowH);

    auto text = [&](const std::string& what, int x, int y)
    {
        font.render(what, sdl::Color{0, 0, 0, 255}).render(window, sdl::Rectangle{x, y, 0, 0});
    };

    auto button = [&](const std::string& caption, int x, int y, int w, int h, std::function<void(void)> onClick)
    {
        renderer.renderRectangle(windowX + x, windowY + y, w, h, 220, 220, 220, 240);
        renderer.renderLine(windowX + x, windowY + y, windowX + x + w, windowY + y, 100, 100, 100, 200);
        renderer.renderLine(windowX + x, windowY + y + h, windowX + x + w, windowY + y + h, 80, 80, 80, 200);
        renderer.renderLine(windowX + x, windowY + y, windowX + x, windowY + y + h, 100, 100, 100, 200);
        renderer.renderLine(windowX + x + w, windowY + y, windowX + x + w, windowY + y + h, 80, 80, 80, 200);
        auto captionW = font.size(caption).first;
        text(caption, x + (w / 2 - captionW / 2), y - 4);

        if (clicked and btnX > windowX + x and btnX < windowX + x + w and btnY > windowY + y and btnY < windowY + y + h)
        {
            onClick();
            clicked = false;
        }
    };

    text(std::format("Selected sector: {}", *selectedSector), 10, 10);
    text(std::format("Ceiling Z: {:.1f}", sector.ceiling), 10, 30);
    text(std::format("Floor Z: {:.1f}", sector.floor), 10, 50);

    auto sectorFieldAnimator = [&](double diff, double& field)
    {
        return [&, diff](){ enqueue(500, field, field + diff, [&](auto v) { field = v; }); };
    };

    button("+1", windowW - 40, 33, 30, 16, sectorFieldAnimator(1, sector.ceiling));
    button("+0.1", windowW - 75, 33, 30, 16, sectorFieldAnimator(0.1, sector.ceiling));
    button("-0.1", windowW - 110, 33, 30, 16, sectorFieldAnimator(-0.1, sector.ceiling));
    button("-1", windowW - 145, 33, 30, 16, sectorFieldAnimator(-1, sector.ceiling));

    button("+1", windowW - 40, 53, 30, 16, sectorFieldAnimator(1, sector.floor));
    button("+0.1", windowW - 75, 53, 30, 16, sectorFieldAnimator(0.1, sector.floor));
    button("-0.1", windowW - 110, 53, 30, 16, sectorFieldAnimator(-0.1, sector.floor));
    button("-1", windowW - 145, 53, 30, 16, sectorFieldAnimator(-1, sector.floor));

    auto texture = sdl::Texture(renderer, sdl::Texture::Access::Streaming, windowW, windowH);
    texture.setBlendMode(sdl::BlendMode::Blend);
    window.render(texture);
    renderer.copy(texture, std::nullopt, sdl::FRectangle{(float)windowX, (float)windowY, (float)windowW, (float)windowH});
}

void Editor::render(sdl::Renderer& renderer)
{
    processMapUpdates();
    drawMap(renderer);
    drawSelectedSector(renderer);
    updateMouse();
}
}
