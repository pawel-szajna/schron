#pragma once

#include "widgets.hpp"

#include <functional>
#include <optional>

namespace ui::editor
{
class WindowTexture
{
public:

    using TextureChoiceCallback = std::function<void(const std::string&)>;

    WindowTexture(sdl::Font& font,
                  const std::string& category,
                  TextureChoiceCallback onChoice,
                  std::optional<WindowTexture>& container);
    ~WindowTexture();

    bool consumeClick(bool clicked, int mouseX, int mouseY);
    void scroll(int rows);
    void render(sdl::Renderer& renderer);

private:

    void fillWindow();

    Window textureWindow;
    std::optional<WindowTexture>& container;
    TextureChoiceCallback onChoice;
    sdl::Font& font;
    std::string category;
    std::vector<std::string> textures;
    int scrollPosition;
};
} // namespace ui::editor
