#include "editor/window_sector.hpp"

#include "util/format.hpp"
#include "editor/window_texture.hpp"
#include "world/sector.hpp"

namespace ui::editor
{
WindowSector::WindowSector(world::Sector& sector,
                           sdl::Font& font,
                           std::function<void(double, double&)> animator,
                           std::optional<WindowTexture>& textureSelector,
                           int x, int y) :
    width(280),
    height(170 + sector.walls.size() * 20),
    animator(std::move(animator)),
    textureSelector(textureSelector),
    font(font),
    sector(sector),
    sectorWindow(x, y, width, height),
    ceiling(sectorWindow.add<Text>(10, 30, font, "Ceiling Z")),
    floor(sectorWindow.add<Text>(10, 50, font, "Floor Z")),
    textureCeiling(sectorWindow.add<Text>(10, 80, font, "Ceiling texture")),
    textureFloor(sectorWindow.add<Text>(10, 100, font, "Floor texture")),
    walls(sectorWindow.add<Group>(0, 0))
{
    spdlog::debug(this->textureSelector.has_value());
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

    sectorWindow.add<Button>(width - 80, 83, 70, 16, font, "Change",
                             [&]{ openTextureSelector("", [&sector](std::string t) { sector.ceilingTexture = std::move(t); }); });
    sectorWindow.add<Button>(width - 80, 103, 70, 16, font, "Change",
                             [&]{ openTextureSelector("", [&sector](std::string t) { sector.floorTexture = std::move(t); }); });

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
    textureCeiling.setCaption(std::format("Ceiling texture: {}", sector.ceilingTexture));
    textureFloor.setCaption(std::format("Floor texture: {}", sector.floorTexture));
    refreshWalls();
    sectorWindow.render(renderer);
}

void WindowSector::move(int x, int y)
{
    sectorWindow.move(x, y);
}

void WindowSector::openTextureSelector(const std::string& category, WindowTexture::TextureChoiceCallback onChoice)
{
    textureSelector.emplace(font, category, std::move(onChoice), textureSelector);
}
}
