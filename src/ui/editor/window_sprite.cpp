#include "window_sprite.hpp"

#include "world/sector.hpp"

namespace ui::editor
{
WindowSprite::WindowSprite(world::Sector& sector,
                           int spriteId,
                           sdl::Font& font,
                           std::optional<WindowTexture>& textureSelector,
                           int x, int y,
                           SpriteAction duplicate,
                           SpriteAction destroy,
                           SpriteAction block) :
        width(280),
        height(170),
        textureSelector(textureSelector),
        font(font),
        sprite(sector.sprites[spriteId]),
        spriteWindow(x, y, width, height),
        texture(spriteWindow.add<Text>(10, 30, font, "Texture")),
        x(spriteWindow.add<Text>(10, 50, font, "X")),
        y(spriteWindow.add<Text>(10, 70, font, "Y")),
        z(spriteWindow.add<Text>(10, 90, font, "Z")),
        blocking(spriteWindow.add<Button>(30, 150, width - 60, 16, font, "Switch", block)),
        duplicate(std::move(duplicate)),
        destroy(std::move(destroy)),
        block(std::move(block))
{
    auto& s = this->sprite;

    spriteWindow.add<Text>(10, 5, font, std::format("Selected sprite: {}", sprite.id));

    spriteWindow.add<Button>(width - 80, 33, 70, 16, font, "Change",
                             [&]{ openTextureSelector("sprites", [&s](std::string t) { s.texture = std::format("sprites/{}", std::move(t)); }); });

    spriteWindow.add<Button>(width - 40, 53, 30, 16, font, "+1", [&s]() { s.x += 1; });
    spriteWindow.add<Button>(width - 75, 53, 30, 16, font, "+0.1", [&s]() { s.x += 0.1; });
    spriteWindow.add<Button>(width - 110, 53, 30, 16, font, "–0.1", [&s]() { s.x -= 0.1; });
    spriteWindow.add<Button>(width - 145, 53, 30, 16, font, "–1", [&s]() { s.x -= 1; });

    spriteWindow.add<Button>(width - 40, 73, 30, 16, font, "+1", [&s]() { s.y += 1; });
    spriteWindow.add<Button>(width - 75, 73, 30, 16, font, "+0.1", [&s]() { s.y += 0.1; });
    spriteWindow.add<Button>(width - 110, 73, 30, 16, font, "–0.1", [&s]() { s.y -= 0.1; });
    spriteWindow.add<Button>(width - 145, 73, 30, 16, font, "–1", [&s]() { s.y -= 1; });

    spriteWindow.add<Button>(width - 40, 93, 30, 16, font, "+1", [&s]() { s.z += 1; });
    spriteWindow.add<Button>(width - 75, 93, 30, 16, font, "+0.1", [&s]() { s.z += 0.1; });
    spriteWindow.add<Button>(width - 110, 93, 30, 16, font, "–0.1", [&s]() { s.z -= 0.1; });
    spriteWindow.add<Button>(width - 145, 93, 30, 16, font, "–1", [&s]() { s.z -= 1; });

    spriteWindow.add<Button>(30, 110, width - 60, 16, font, "Duplicate", duplicate);
    spriteWindow.add<Button>(30, 130, width - 60, 16, font, "Destroy", destroy);
}

WindowSprite::~WindowSprite() = default;

bool WindowSprite::consumeClick(bool clicked, int mouseX, int mouseY)
{
    return clicked and spriteWindow.consumeClick(mouseX, mouseY);
}

void WindowSprite::move(int x, int y)
{
    spriteWindow.move(x, y);
}

void WindowSprite::render(sdl::Renderer& renderer)
{
    texture.setCaption(std::format("Texture: {}", sprite.texture));
    x.setCaption(std::format("x = {:.1f}", sprite.x));
    y.setCaption(std::format("y = {:.1f}", sprite.y));
    z.setCaption(std::format("z = {:.1f}", sprite.z));
    blocking.setCaption(std::format("Make {}", sprite.blocking ? "walkable" : "blocking"));

    spriteWindow.render(renderer);
}

void WindowSprite::openTextureSelector(const std::string& category, WindowTexture::TextureChoiceCallback onChoice)
{
    textureSelector.emplace(font, category, std::move(onChoice), textureSelector);
}
}