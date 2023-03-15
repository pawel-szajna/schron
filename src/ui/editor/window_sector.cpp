#include "window_sector.hpp"

#include "util/format.hpp"
#include "world/sector.hpp"

namespace ui::editor
{
WindowSector::WindowSector(world::Sector& sector,
                           sdl::Font& font,
                           std::function<void(double, double&)> animator,
                           int x, int y) :
    width(280),
    height(170 + sector.walls.size() * 20),
    animator(std::move(animator)),
    font(font),
    sector(sector),
    sectorWindow(x, y, width, height),
    ceiling(sectorWindow.add<Text>(10, 30, font, "Ceiling Z")),
    floor(sectorWindow.add<Text>(10, 50, font, "Floor Z")),
    walls(sectorWindow.add<Group>(0, 0))
{
    sectorWindow.add<Text>(10,  5, font, std::format("Selected sector: {}", sector.id));

    auto sectorFieldAnimator = [&](double diff, double& field)
    {
        return [diff, &field, &animator = this->animator]() { animator(diff, field); };
    };

    sectorWindow.add<Button>(width - 40, 33, 30, 16, font, "+1", sectorFieldAnimator(1, sector.ceiling));
    sectorWindow.add<Button>(width - 75, 33, 30, 16, font, "+0.1", sectorFieldAnimator(0.1, sector.ceiling));
    sectorWindow.add<Button>(width - 110, 33, 30, 16, font, "–0.1", sectorFieldAnimator(-0.1, sector.ceiling));
    sectorWindow.add<Button>(width - 145, 33, 30, 16, font, "–1", sectorFieldAnimator(-1, sector.ceiling));

    sectorWindow.add<Button>(width - 40, 53, 30, 16, font, "+1", sectorFieldAnimator(1, sector.floor));
    sectorWindow.add<Button>(width - 75, 53, 30, 16, font, "+0.1", sectorFieldAnimator(0.1, sector.floor));
    sectorWindow.add<Button>(width - 110, 53, 30, 16, font, "–0.1", sectorFieldAnimator(-0.1, sector.floor));
    sectorWindow.add<Button>(width - 145, 53, 30, 16, font, "–1", sectorFieldAnimator(-1, sector.floor));

    sectorWindow.add<Text>(10, 80, font, std::format("Ceiling texture: {}", sector.ceilingTexture));
    sectorWindow.add<Text>(10, 100, font, std::format("Floor texture: {}", sector.floorTexture));

    sectorWindow.add<Button>(width - 80, 83, 70, 16, font, "Change", []{});
    sectorWindow.add<Button>(width - 80, 103, 70, 16, font, "Change", []{});

    sectorWindow.add<Text>(10, 130, font, "Walls:");

    refreshWalls();
}

WindowSector::~WindowSector() = default;

void WindowSector::refreshWalls()
{
    walls.clear();
    int wallDescY{155}, wallId{0};
    for (const auto& wall : sector.walls)
    {
        walls.add<Text>(10, wallDescY, font,
                            std::format("#{}: {} [{:.1f},{:.1f}]–[{:.1f},{:.1f}]",
                                        wallId, wall.portal ? std::format("Portal ({})", wall.portal->sector) : "Wall",
                                        wall.xStart, wall.yStart, wall.xEnd, wall.yEnd));
        walls.add<Button>(width - 65, wallDescY + 3, 55, 16, font, "Edit", []{});
        wallDescY += 20;
        wallId += 1;
    }
}

bool WindowSector::consumeClick(bool clicked, int mouseX, int mouseY)
{
    return clicked and sectorWindow.consumeClick(mouseX, mouseY);
}

void WindowSector::render(sdl::Renderer& renderer)
{
    ceiling.setCaption(std::format("Ceiling Z: {:.1f}", sector.ceiling));
    floor.setCaption(std::format("Floor Z: {:.1f}", sector.floor));
    refreshWalls();
    sectorWindow.render(renderer);
}

void WindowSector::move(int x, int y)
{
    sectorWindow.move(x, y);
}
}