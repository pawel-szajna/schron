#pragma once

#include "widgets.hpp"

namespace world
{
class Sector;
}

namespace ui::editor
{
class WindowSector
{
public:

    WindowSector(world::Sector& sector,
                 sdl::Font& font,
                 std::function<void(double, double&)> animator,
                 int x, int y);
    ~WindowSector();

    bool consumeClick(bool clicked, int mouseX, int mouseY);
    void render(sdl::Renderer& renderer);
    void move(int x, int y);

private:

    void refreshWalls();

    int width, height;
    std::function<void(double, double&)> animator;
    sdl::Font& font;
    world::Sector& sector;
    Window sectorWindow;
    Text& ceiling;
    Text& floor;
    Group& walls;
};
}