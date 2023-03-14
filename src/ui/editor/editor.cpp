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
    }
    else if (std::holds_alternative<sdl::event::Key>(event))
    {
        auto& key = std::get<sdl::event::Key>(event);
        if (key.scancode == 225 /* SDL_SCANCODE_LSHIFT */)
        {
            modShift = key.direction == sdl::event::Key::Direction::Down;
        }
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
    if (dragged)
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

void Editor::resizeSector(int id, double left, double right, double top, double bottom)
{
    resizeSingleSector(id, left, right, top, bottom, true);
}

void Editor::resizeSingleSector(int id, double left, double right, double top, double bottom, bool recurse)
{
    spdlog::debug("resizeSingleSector(id={}, left={}, right={}, top={}, bottom={} recurse={})", id, left, right, top, bottom, recurse);
    auto& sector = level.map.at(id);
    using Intersection = std::tuple<int, double, double>;
    std::vector<Intersection> xsTop, xsRight, xsBottom, xsLeft;
    std::vector<std::tuple<int, double, double, double, double>> toResize{};
    for (auto& [otherId, other] : level.map)
    {
        if (otherId == id or
            other.ceiling < sector.floor or
            other.floor > sector.ceiling)
        {
            continue;
        }
        auto wallLeftX =   *std::min_element(other.walls.begin(), other.walls.end(), [](const auto& a, const auto& b) { return a.xStart < b.xStart; });
        auto wallRightX =  *std::max_element(other.walls.begin(), other.walls.end(), [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
        auto wallTopY =    *std::min_element(other.walls.begin(), other.walls.end(), [](const auto& a, const auto& b) { return a.yStart < b.yStart; });
        auto wallBottomY = *std::max_element(other.walls.begin(), other.walls.end(), [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; });
        auto otherLeft = wallLeftX.xStart;
        auto otherRight = wallRightX.xEnd;
        auto otherTop = wallTopY.yStart;
        auto otherBottom = wallBottomY.yEnd;

        auto enqueue = [&, otherId = otherId]()
        {
            if (not resized.contains(otherId) and recurse)
            {
                toResize.emplace_back(otherId, otherLeft, otherRight, otherTop, otherBottom);
            }
        };

        if (otherBottom == top and otherLeft < right and otherRight > left)
        {
            xsTop.emplace_back(otherId, otherLeft, otherRight);
            enqueue();
        }
        else if (otherLeft == right and otherTop < bottom and otherBottom > top)
        {
            xsRight.emplace_back(otherId, otherTop, otherBottom);
            enqueue();
        }
        else if (otherTop == bottom and otherLeft < right and otherRight > left)
        {
            xsBottom.emplace_back(otherId, otherRight, otherLeft);
            enqueue();
        }
        else if (otherRight == left and otherTop < bottom and otherBottom > top)
        {
            xsLeft.emplace_back(otherId, otherBottom, otherTop);
            enqueue();
        }
        else if (std::any_of(other.walls.begin(), other.walls.end(),
                             [id](const auto& wall) { return wall.portal and wall.portal->sector == id; }))
        {
            enqueue();
        }
    }
    std::sort(xsTop.begin(), xsTop.end(), [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) < std::get<1>(xs2); });
    std::sort(xsRight.begin(), xsRight.end(), [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) < std::get<1>(xs2); });
    std::sort(xsBottom.begin(), xsBottom.end(), [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) > std::get<1>(xs2); });
    std::sort(xsLeft.begin(), xsLeft.end(), [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) > std::get<1>(xs2); });

    sector.walls.clear();
    auto addHWall = [&](double x1, double x2, double y, std::optional<world::Wall::Portal> portal)
    {
        sector.walls.emplace_back(world::Wall{x1, y, x2, y, portal, "wall"});
    };
    auto addVWall = [&](double y1, double y2, double x, std::optional<world::Wall::Portal> portal)
    {
        sector.walls.emplace_back(world::Wall{x, y1, x, y2, portal, "wall"});
    };
    auto segmentize = [](const std::vector<Intersection>& intersections,
                         double init, double limit, double constant,
                         std::function<void(double, double, double, std::optional<world::Wall::Portal>)> builder,
                         auto comparator = std::less<>())
    {
        double x = init;
        for (const auto& xs : intersections)
        {
            int neighbourId = std::get<0>(xs);
            double neighbourStart = std::max(std::get<1>(xs), x, comparator);
            double neighbourEnd = std::min(std::get<2>(xs), limit, comparator);
            if (comparator(x, neighbourStart))
            {
                builder(x, neighbourStart, constant, std::nullopt);
            }
            if (comparator(neighbourStart, neighbourEnd))
            {
                builder(neighbourStart, neighbourEnd, constant, world::Wall::Portal{neighbourId});
            }
            x = neighbourEnd;
        }
        if (comparator(x, limit))
        {
            builder(x, limit, constant, std::nullopt);
        }
    };

    segmentize(xsTop, left, right, top, addHWall, std::less<>());
    segmentize(xsRight, top, bottom, right, addVWall, std::less<>());
    segmentize(xsBottom, right, left, bottom, addHWall, std::greater<>());
    segmentize(xsLeft, bottom, top, left, addVWall, std::greater<>());

    for (auto args : toResize)
    {
        std::apply(&Editor::resizeSingleSector,
                   std::tuple_cat(std::forward_as_tuple(this), args, std::forward_as_tuple(false)));
    }
}

void Editor::drawSelectedSector(sdl::Renderer& renderer)
{
    if (not selectedSector.has_value())
    {
        return;
    }

    auto& sector = level.map.at(*selectedSector);
    auto wallLeftX =   *std::min_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.xStart < b.xStart; });
    auto wallRightX =  *std::max_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
    auto wallTopY =    *std::min_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.yStart < b.yStart; });
    auto wallBottomY = *std::max_element(sector.walls.begin(), sector.walls.end(), [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; });

    double leftX = wallLeftX.xStart * mapScale - mapX;
    double rightX = wallRightX.xEnd * mapScale - mapX;
    double topY = wallTopY.yStart * mapScale - mapY;
    double bottomY = wallBottomY.yEnd * mapScale - mapY;

    auto mouseInBox = [&](double x1, double y1, double x2, double y2)
    {
        return mouseX >= x1 and mouseX <= x2 and mouseY >= y1 and mouseY <= y2;
    };

    if (not draggedWall)
    {
        bool draggingStarted = dragged and mouseX == dragX and mouseY == dragY;
        if (mouseInBox(leftX - 5, topY - 5, rightX + 5, topY + 5))
        {
            vertical.activate();
            if (draggingStarted)
            {
                draggedWall = Direction::Top;
                dragged = false;
            }
        }
        else if (mouseInBox(leftX - 5, bottomY - 5, rightX + 5, bottomY + 5))
        {
            vertical.activate();
            if (draggingStarted)
            {
                draggedWall = Direction::Bottom;
                dragged = false;
            }
        }
        else if (mouseInBox(leftX - 5, topY - 5, leftX + 5, bottomY + 5))
        {
            horizontal.activate();
            if (draggingStarted)
            {
                draggedWall = Direction::Left;
                dragged = false;
            }
        }
        else if (mouseInBox(rightX - 5, topY - 5, rightX + 5, bottomY + 5))
        {
            horizontal.activate();
            if (draggingStarted)
            {
                draggedWall = Direction::Right;
                dragged = false;
            }
        }
        else
        {
            sdl::changeCursor(0);
        }
    }

    if (draggedWall)
    {
        if (not dragging)
        {
            draggedWall = std::nullopt;
            sdl::changeCursor(0);
        }
        else
        {
            auto left = wallLeftX.xStart;
            auto right = wallRightX.xEnd;
            auto top = wallTopY.yStart;
            auto bottom = wallBottomY.yEnd;
            auto step = 1.0;
            if (modShift) step /= 10;
            if (draggedWall == Direction::Top)
            {
                if (mouseY < topY - mapScale * step / 2) resizeSector(*selectedSector, left, right, top - step, bottom);
                else if (mouseY > topY + mapScale * step / 2) resizeSector(*selectedSector, left, right, top + step, bottom);
            }
            else if (draggedWall == Direction::Bottom)
            {
                if (mouseY < bottomY - mapScale * step / 2) resizeSector(*selectedSector, left, right, top, bottom - step);
                else if (mouseY > bottomY + mapScale * step / 2) resizeSector(*selectedSector, left, right, top, bottom + step);
            }
            else if (draggedWall == Direction::Left)
            {
                if (mouseX < leftX - mapScale * step / 2) resizeSector(*selectedSector, left - step, right, top, bottom);
                else if (mouseX > leftX + mapScale * step / 2) resizeSector(*selectedSector, left + step, right, top, bottom);
            }
            else if (draggedWall == Direction::Right)
            {
                if (mouseX < rightX - mapScale * step / 2) resizeSector(*selectedSector, left, right - step, top, bottom);
                else if (mouseX > rightX + mapScale * step / 2) resizeSector(*selectedSector, left, right + step, top, bottom);
            }
            dragged = false;
        }
    }

    int windowX = rightX + 16;
    int windowY = topY;
    int windowW = 280;
    int windowH = 170 + sector.walls.size() * 20;
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

    text(std::format("Selected sector: {}", *selectedSector), 10, 5);
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

    text(std::format("Ceiling texture: {}", sector.ceilingTexture), 10, 80);
    text(std::format("Floor texture: {}", sector.floorTexture), 10, 100);

    button("Change", windowW - 80, 83, 70, 16, []{});
    button("Change", windowW - 80, 103, 70, 16, []{});

    text("Walls:", 10, 130);
    int wallDescY = 155;
    int wallId = 0;
    for (const auto& wall : sector.walls)
    {
        text(std::format("#{}: {} [{:.1f},{:.1f}]–[{:.1f},{:.1f}]",
                         wallId, wall.portal ? std::format("Portal ({})", wall.portal->sector) : "Wall",
                         wall.xStart, wall.yStart, wall.xEnd, wall.yEnd), 10, wallDescY);
        button("Edit", windowW - 65, wallDescY + 3, 55, 16, []{});
        wallDescY += 20;
        wallId++;
    }

    auto texture = sdl::Texture(renderer, sdl::Texture::Access::Streaming, windowW, windowH);
    texture.setBlendMode(sdl::BlendMode::Blend);
    window.render(texture);
    renderer.copy(texture, std::nullopt, sdl::FRectangle{(float)windowX, (float)windowY, (float)windowW, (float)windowH});
}

void Editor::drawGrid(sdl::Renderer& renderer) const
{
    for (int x = (int)mapScale - (int)mapX % (int)mapScale; x < c::windowWidth; x += mapScale)
    {
        renderer.renderLine(x, 0, x, c::windowHeight - 1, 255, 255, 255, 64);
    }
    for (int y = (int)mapScale - (int)mapY % (int)mapScale; y < c::windowHeight; y += mapScale)
    {
        renderer.renderLine(0, y, c::windowWidth - 1, y, 255, 255, 255, 64);
    }
}

void Editor::render(sdl::Renderer& renderer)
{
    dragged = dragging;

    processMapUpdates();
    drawGrid(renderer);
    drawMap(renderer);
    drawSelectedSector(renderer);
    updateMouse();
}
}
