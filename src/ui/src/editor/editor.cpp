#include "editor/editor.hpp"

#include "editor/widgets.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"
#include "world/builders.hpp"
#include "world/level.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, sdl::Font& font)
    : level(level)
    , font(font)
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
        mouseX      = mouse.x;
        mouseY      = mouse.y;
        if (not dragging and mouse.button.has_value() and *mouse.button == 1)
        {
            dragX = btnX = mouseX;
            dragY = btnY = mouseY;
            dragging     = true;
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
            if (textureWindow.has_value())
            {
                textureWindow->scroll(*mouse.scroll);
            }
            else
            {
                mapScale -= *mouse.scroll;
            }
        }
    }
    else if (std::holds_alternative<sdl::event::Key>(event))
    {
        auto& key = std::get<sdl::event::Key>(event);
        if (key.scancode == 224 /* LCTRL */)
        {
            modCtrl = key.direction == sdl::event::Key::Direction::Down;
        }
        else if (key.scancode == 225 /* SDL_SCANCODE_LSHIFT */)
        {
            modShift = key.direction == sdl::event::Key::Direction::Down;
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
        mapX  = mapX - (mouseX - dragX);
        mapY  = mapY - (mouseY - dragY);
        dragX = mouseX;
        dragY = mouseY;
    }

    sectorUnderMouse = std::nullopt;

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
                if (selectedSector and *selectedSector == id)
                {
                    clicked = false;
                    for (const auto& sprite : sector.sprites)
                    {
                        std::vector<sdl::Vertex> spriteVertices = {
                            {(float)(mapScale * sprite.x - mapX - (mapScale / 2)), (float)(mapScale * sprite.y - mapY)},
                            {(float)(mapScale * sprite.x - mapX), (float)(mapScale * sprite.y - mapY - (mapScale / 2))},
                            {(float)(mapScale * sprite.x - mapX + (mapScale / 2)), (float)(mapScale * sprite.y - mapY)},
                            {(float)(mapScale * sprite.x - mapX),
                             (float)(mapScale * sprite.y - mapY + (mapScale / 2))}};
                        if (mouseWithin<std::vector<sdl::Vertex>>(
                                spriteVertices,
                                [n = (size_t)0](const std::vector<sdl::Vertex>& vertices) mutable
                                -> VertexGetter<sdl::Vertex>::result_type
                                {
                                    if (n >= vertices.size())
                                    {
                                        return std::nullopt;
                                    }
                                    n++;
                                    return std::make_tuple(vertices[n].x,
                                                           vertices[n].y,
                                                           vertices[(n + 1) % vertices.size()].x,
                                                           vertices[(n + 1) % vertices.size()].y);
                                }))
                        {
                            selectedSprite       = sprite.id;
                            selectedSpriteWindow = std::nullopt;
                            break;
                        }
                    }
                }
                if (not selectedSector or *selectedSector != id)
                {
                    selectedSector       = id;
                    selectedSectorWindow = std::nullopt;
                    selectedSprite       = std::nullopt;
                    selectedSpriteWindow = std::nullopt;
                    enqueue(1200,
                            0,
                            1199,
                            [&, id = id, originalFloor = sector.floorTexture, originalCeiling = sector.ceilingTexture](
                                double time)
                            {
                                auto& s = level.map.at(id);
                                if ((int)(time) % 400 < 200)
                                {
                                    s.ceilingTexture = "highlight";
                                    s.floorTexture   = "highlight";
                                }
                                else
                                {
                                    s.ceilingTexture = originalCeiling;
                                    s.floorTexture   = originalFloor;
                                }
                            });
                    clicked = false;
                }
            }
            break;
        }
    }

    if (clicked)
    {
        selectedSector = std::nullopt;
        if (modCtrl)
        {
            int newId   = 1;
            double newX = std::floor((mouseX + mapX) / mapScale);
            double newY = std::floor((mouseY + mapY) / mapScale);
            while (level.map.contains(newId))
            {
                ++newId;
            }
            level.put(world::RectangularSectorBuilder(newId).withCeiling(1.0).withFloor(0.0).build());
            resizeSector(newId, newX, newX + 1, newY, newY + 1);
            selectedSector       = newId;
            selectedSectorWindow = std::nullopt;
        }
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
        diff.applier(diff.startValue + (now - diff.startTime) * (diff.targetValue - diff.startValue) /
                                           (diff.targetTime - diff.startTime));
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
                           if (sectorUnderMouse == id)
                           {
                               color = 150;
                           }
                           if (selectedSector == id)
                           {
                               color = 170;
                           }
                           return sdl::Vertex{vertexX,
                                              vertexY,
                                              color,
                                              color,
                                              color,
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
            renderer.renderLine(mapScale * wall.xStart - mapX,
                                mapScale * wall.yStart - mapY,
                                mapScale * wall.xEnd - mapX,
                                mapScale * wall.yEnd - mapY,
                                r,
                                g,
                                b,
                                a);
        }
        for (const auto& sprite : sector.sprites)
        {
            uint8_t r = 140, g = 170, b = 190, a = 200;
            if (selectedSector and *selectedSector == id and selectedSprite and *selectedSprite == sprite.id)
            {
                r = 120, g = 180, b = 230, a = 230;
            }
            vertices = {
                {(float)(mapScale * sprite.x - mapX - (mapScale / 2)), (float)(mapScale * sprite.y - mapY), r, g, b, a},
                {(float)(mapScale * sprite.x - mapX), (float)(mapScale * sprite.y - mapY - (mapScale / 2)), r, g, b, a},
                {(float)(mapScale * sprite.x - mapX + (mapScale / 2)), (float)(mapScale * sprite.y - mapY), r, g, b, a},
                {(float)(mapScale * sprite.x - mapX),
                 (float)(mapScale * sprite.y - mapY + (mapScale / 2)),
                 r,
                 g,
                 b,
                 a}};
            renderer.renderGeometry(vertices);
        }
    }
}

void Editor::moveSectorSprites(int id, double diffX, double diffY)
{
    auto& sprites = level.map.at(id).sprites;
    for (auto& sprite : sprites)
    {
        sprite.x += diffX;
        sprite.y += diffY;
    }
}

void Editor::resizeSector(int id, double left, double right, double top, double bottom)
{
    if (left >= right or top >= bottom)
    {
        return;
    }

    resizeSingleSector(id,
                       std::round(left * 10) / 10,
                       std::round(right * 10) / 10,
                       std::round(top * 10) / 10,
                       std::round(bottom * 10) / 10,
                       true);
}

void Editor::resizeSingleSector(int id, double left, double right, double top, double bottom, bool recurse)
{
    spdlog::debug("resizeSingleSector(id={}, left={}, right={}, top={}, bottom={} recurse={})",
                  id,
                  left,
                  right,
                  top,
                  bottom,
                  recurse);
    auto& sector       = level.map.at(id);
    using Intersection = std::tuple<int, double, double>;
    std::vector<Intersection> xsTop, xsRight, xsBottom, xsLeft;
    std::vector<std::tuple<int, double, double, double, double>> toResize{};
    for (auto& [otherId, other] : level.map)
    {
        if (otherId == id or other.ceiling < sector.floor or other.floor > sector.ceiling)
        {
            continue;
        }
        auto wallLeftX   = *std::min_element(other.walls.begin(),
                                           other.walls.end(),
                                           [](const auto& a, const auto& b) { return a.xStart < b.xStart; });
        auto wallRightX  = *std::max_element(other.walls.begin(),
                                            other.walls.end(),
                                            [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
        auto wallTopY    = *std::min_element(other.walls.begin(),
                                          other.walls.end(),
                                          [](const auto& a, const auto& b) { return a.yStart < b.yStart; });
        auto wallBottomY = *std::max_element(other.walls.begin(),
                                             other.walls.end(),
                                             [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; });
        auto otherLeft   = wallLeftX.xStart;
        auto otherRight  = wallRightX.xEnd;
        auto otherTop    = wallTopY.yStart;
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
        else if (std::any_of(other.walls.begin(),
                             other.walls.end(),
                             [id](const auto& wall) { return wall.portal and wall.portal->sector == id; }))
        {
            enqueue();
        }
    }
    std::sort(xsTop.begin(),
              xsTop.end(),
              [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) < std::get<1>(xs2); });
    std::sort(xsRight.begin(),
              xsRight.end(),
              [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) < std::get<1>(xs2); });
    std::sort(xsBottom.begin(),
              xsBottom.end(),
              [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) > std::get<1>(xs2); });
    std::sort(xsLeft.begin(),
              xsLeft.end(),
              [](const auto& xs1, const auto& xs2) { return std::get<1>(xs1) > std::get<1>(xs2); });

    sector.walls.clear();
    auto addHWall = [&](double x1, double x2, double y, std::optional<world::Wall::Portal> portal) {
        sector.walls.emplace_back(world::Wall{x1, y, x2, y, portal, "wall"});
    };
    auto addVWall = [&](double y1, double y2, double x, std::optional<world::Wall::Portal> portal) {
        sector.walls.emplace_back(world::Wall{x, y1, x, y2, portal, "wall"});
    };
    auto segmentize = [](const std::vector<Intersection>& intersections,
                         double init,
                         double limit,
                         double constant,
                         std::function<void(double, double, double, std::optional<world::Wall::Portal>)> builder,
                         auto comparator = std::less<>())
    {
        double x = init;
        for (const auto& xs : intersections)
        {
            int neighbourId       = std::get<0>(xs);
            double neighbourStart = std::max(std::get<1>(xs), x, comparator);
            double neighbourEnd   = std::min(std::get<2>(xs), limit, comparator);
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
        selectedSectorWindow = std::nullopt;
        return;
    }

    auto& sector     = level.map.at(*selectedSector);
    auto wallLeftX   = *std::min_element(sector.walls.begin(),
                                       sector.walls.end(),
                                       [](const auto& a, const auto& b) { return a.xStart < b.xStart; });
    auto wallRightX  = *std::max_element(sector.walls.begin(),
                                        sector.walls.end(),
                                        [](const auto& a, const auto& b) { return a.xEnd < b.xEnd; });
    auto wallTopY    = *std::min_element(sector.walls.begin(),
                                      sector.walls.end(),
                                      [](const auto& a, const auto& b) { return a.yStart < b.yStart; });
    auto wallBottomY = *std::max_element(sector.walls.begin(),
                                         sector.walls.end(),
                                         [](const auto& a, const auto& b) { return a.yEnd < b.yEnd; });

    double leftX   = wallLeftX.xStart * mapScale - mapX;
    double rightX  = wallRightX.xEnd * mapScale - mapX;
    double topY    = wallTopY.yStart * mapScale - mapY;
    double bottomY = wallBottomY.yEnd * mapScale - mapY;

    auto mouseInBox = [&](double x1, double y1, double x2, double y2)
    { return mouseX >= x1 and mouseX <= x2 and mouseY >= y1 and mouseY <= y2; };

    if (not drag)
    {
        bool draggingStarted = dragged and mouseX == dragX and mouseY == dragY;
        if (mouseInBox(leftX - 5, topY - 5, rightX + 5, topY + 5))
        {
            cursorVertical.activate();
            if (draggingStarted)
            {
                drag    = DragOperation::WallTop;
                dragged = false;
            }
        }
        else if (mouseInBox(leftX - 5, bottomY - 5, rightX + 5, bottomY + 5))
        {
            cursorVertical.activate();
            if (draggingStarted)
            {
                drag    = DragOperation::WallBottom;
                dragged = false;
            }
        }
        else if (mouseInBox(leftX - 5, topY - 5, leftX + 5, bottomY + 5))
        {
            cursorHorizontal.activate();
            if (draggingStarted)
            {
                drag    = DragOperation::WallLeft;
                dragged = false;
            }
        }
        else if (mouseInBox(rightX - 5, topY - 5, rightX + 5, bottomY + 5))
        {
            cursorHorizontal.activate();
            if (draggingStarted)
            {
                drag    = DragOperation::WallRight;
                dragged = false;
            }
        }
        else if (mouseInBox(leftX, topY, rightX, bottomY))
        {
            cursorMove.activate();
            if (draggingStarted)
            {
                drag    = DragOperation::Sector;
                dragged = false;
            }
        }
        else
        {
            sdl::resetCursor();
        }
    }

    if (drag)
    {
        if (not dragging)
        {
            drag = std::nullopt;
            sdl::resetCursor();
        }
        else
        {
            auto left   = wallLeftX.xStart;
            auto right  = wallRightX.xEnd;
            auto top    = wallTopY.yStart;
            auto bottom = wallBottomY.yEnd;
            auto step   = 1.0;
            if (modShift)
            {
                step /= 10;
            }
            if (drag == DragOperation::Sector)
            {
                if (mouseX < dragX - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left - step, right - step, top, bottom);
                    moveSectorSprites(*selectedSector, -step, 0);
                    dragX = mouseX;
                }
                else if (mouseX > dragX + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left + step, right + step, top, bottom);
                    moveSectorSprites(*selectedSector, step, 0);
                    dragX = mouseX;
                }
                if (mouseY < dragY - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top - step, bottom - step);
                    moveSectorSprites(*selectedSector, 0, -step);
                    dragY = mouseY;
                }
                else if (mouseY > dragY + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top + step, bottom + step);
                    moveSectorSprites(*selectedSector, 0, step);
                    dragY = mouseY;
                }
            }
            if (drag == DragOperation::WallTop)
            {
                if (mouseY < topY - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top - step, bottom);
                }
                else if (mouseY > topY + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top + step, bottom);
                }
            }
            else if (drag == DragOperation::WallBottom)
            {
                if (mouseY < bottomY - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top, bottom - step);
                }
                else if (mouseY > bottomY + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right, top, bottom + step);
                }
            }
            else if (drag == DragOperation::WallLeft)
            {
                if (mouseX < leftX - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left - step, right, top, bottom);
                }
                else if (mouseX > leftX + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left + step, right, top, bottom);
                }
            }
            else if (drag == DragOperation::WallRight)
            {
                if (mouseX < rightX - mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right - step, top, bottom);
                }
                else if (mouseX > rightX + mapScale * step / 2)
                {
                    resizeSector(*selectedSector, left, right + step, top, bottom);
                }
            }
            dragged = false;
        }
    }

    if (not selectedSectorWindow)
    {
        selectedSectorWindow.emplace(
            sector,
            font,
            [&](double diff, double& field) { enqueue(500, field, field + diff, [&](auto v) { field = v; }); },
            textureWindow,
            rightX + 16,
            topY);
    }

    clicked = selectedSectorWindow->consumeClick(clicked, mouseX, mouseY);
    selectedSectorWindow->move(rightX + 16, topY);
    selectedSectorWindow->render(renderer);

    if (selectedSprite)
    {
        if (not selectedSpriteWindow)
        {
            selectedSpriteWindow.emplace(
                sector,
                *selectedSprite,
                font,
                textureWindow,
                rightX + 312,
                topY,
                [&, this]()
                {
                    const auto& src = sector.sprites[*selectedSprite];
                    sector.sprites.emplace_back(world::Sprite{sector.sprites.back().id + 1,
                                                              {{0, src.texture(0)}},
                                                              src.x + 0.5,
                                                              src.y + 0.5,
                                                              src.z,
                                                              src.w,
                                                              src.h,
                                                              src.offset,
                                                              src.shadows,
                                                              src.lightCenter,
                                                              src.blocking});
                },
                [&, this]()
                {
                    sector.sprites.erase(std::find_if(sector.sprites.begin(),
                                                      sector.sprites.end(),
                                                      [this](const world::Sprite& sprite)
                                                      { return sprite.id == *selectedSprite; }));
                    selectedSprite.reset();
                    int counter = 0;
                    for (auto& sprite : sector.sprites)
                    {
                        sprite.id = counter++;
                    }
                },
                [&, this]()
                {
                    auto& sprite    = sector.sprites[*selectedSprite];
                    sprite.blocking = not sprite.blocking;
                });
        }

        clicked = selectedSpriteWindow->consumeClick(clicked, mouseX, mouseY);
        selectedSpriteWindow->move(rightX + 312, topY);
        selectedSpriteWindow->render(renderer);
    }
}

void Editor::drawGrid(sdl::Renderer& renderer) const
{
    for (int x = (int)mapScale - (int)mapX % (int)mapScale; x < c::windowWidth; x += mapScale)
    {
        renderer.renderLine(x, 0, x, c::windowHeight - 1, 255, 255, 255, 64);
        if (modShift)
        {
            auto step = mapScale / 10;
            for (int i = 1; i < 10; ++i)
            {
                renderer
                    .renderLine(x + (int)(step * i), 0, x + (int)(step * i), c::windowHeight - 1, 255, 255, 255, 32);
            }
        }
    }
    for (int y = (int)mapScale - (int)mapY % (int)mapScale; y < c::windowHeight; y += mapScale)
    {
        renderer.renderLine(0, y, c::windowWidth - 1, y, 255, 255, 255, 64);
        if (modShift)
        {
            auto step = mapScale / 10;
            for (int i = 1; i < 10; ++i)
            {
                renderer.renderLine(0, y + (int)(step * i), c::windowWidth - 1, y + (int)(step * i), 255, 255, 255, 32);
            }
        }
    }
}

void Editor::render(sdl::Renderer& renderer)
{
    dragged = dragging;

    processMapUpdates();
    drawGrid(renderer);
    drawMap(renderer);
    if (textureWindow)
    {
        textureWindow->consumeClick(clicked, mouseX, mouseY);
        clicked = false;
        dragged = false;
    }
    drawSelectedSector(renderer);
    if (textureWindow)
    {
        textureWindow->render(renderer);
    }
    updateMouse();
}
} // namespace ui::editor
