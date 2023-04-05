#pragma once

#include "widgets.hpp"
#include "window_texture.hpp"

#include <optional>

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
                 std::optional<WindowTexture>& textureSelector,
                 int x, int y);
    ~WindowSector();

    bool consumeClick(bool clicked, int mouseX, int mouseY);
    void render(sdl::Renderer& renderer);
    void move(int x, int y);

private:

    void refreshWalls();
    void openTextureSelector(const std::string& category, WindowTexture::TextureChoiceCallback onChoice);

    int width, height;
    std::function<void(double, double&)> animator;
    std::optional<WindowTexture>& textureSelector;
    sdl::Font& font;
    world::Sector& sector;
    Window sectorWindow;
    Text& ceiling;
    Text& floor;
    Text& textureCeiling;
    Text& textureFloor;
    Group& walls;
};
}