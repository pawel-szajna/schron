#pragma once

#include "widgets.hpp"
#include "window_texture.hpp"

#include <optional>

namespace world
{
class Sector;
class Sprite;
}

namespace ui::editor
{
class WindowSprite
{
public:

    WindowSprite(world::Sector& sector,
                 int spriteId,
                 sdl::Font& font,
                 std::optional<WindowTexture>& textureSelector,
                 int x, int y);
    ~WindowSprite();

    bool consumeClick(bool clicked, int mouseX, int mouseY);
    void render(sdl::Renderer& renderer);
    void move(int x, int y);

private:

    void openTextureSelector(const std::string& category, WindowTexture::TextureChoiceCallback onChoice);

    int width, height;
    std::optional<WindowTexture>& textureSelector;
    sdl::Font& font;
    world::Sprite& sprite;
    Window spriteWindow;
    Text& texture;
    Text& x;
    Text& y;
    Text& z;
};
}