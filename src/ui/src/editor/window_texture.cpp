#include "editor/window_texture.hpp"

#include "util/constants.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>

namespace
{
constexpr auto windowWidth{800};
constexpr auto windowHeight{600};
constexpr auto windowX{c::windowWidth / 2 - windowWidth / 2};
constexpr auto windowY{c::windowHeight / 2 - windowHeight / 2};

constexpr auto previewSize{128};
constexpr auto previewMargin{8};
constexpr auto previewSpacingX{5};
constexpr auto previewSpacingY{16};
} // namespace

namespace ui::editor
{
WindowTexture::WindowTexture(sdl::Font& font,
                             const std::string& category,
                             TextureChoiceCallback onChoice,
                             std::optional<WindowTexture>& container)
    : textureWindow(windowX, windowY, windowWidth, windowHeight)
    , container(container)
    , onChoice(std::move(onChoice))
    , font(font)
    , category(category)
{
    auto path = std::format("res/gfx/{}", category);

    for (const auto& file : std::filesystem::directory_iterator(path))
    {
        if (file.is_regular_file() and file.path().has_extension() and file.path().extension() == ".png")
        {
            auto filename = file.path().filename().string();
            textures.push_back(filename.substr(0, filename.size() - 4));
        }
    }
    std::sort(textures.begin(), textures.end());
    fillWindow();
}

WindowTexture::~WindowTexture() = default;

void WindowTexture::fillWindow()
{
    int currentColumn = 0;
    int currentRow    = 0;
    int skipped       = scrollPosition * 6;

    for (auto texture : textures)
    {
        if (skipped-- > 0)
        {
            continue;
        }
        textureWindow.add<Button>(3 + currentColumn * (previewSpacingX + previewSize),
                                  3 + currentRow * (previewSpacingY + previewSize),
                                  previewSize,
                                  previewSize + (previewSpacingY - previewSpacingX),
                                  font,
                                  texture,
                                  [&, texture] { onChoice(texture); });
        textureWindow.add<Image>(3 + currentColumn * (previewSpacingX + previewSize) + previewMargin,
                                 3 + currentRow * (previewSpacingY + previewSize) + previewMargin + previewSpacingY -
                                     previewSpacingX,
                                 previewSize - previewMargin * 2,
                                 previewSize - previewMargin * 2,
                                 std::format("res/gfx/{}/{}.png", category, texture));
        currentColumn++;
        if (currentColumn >= 6)
        {
            currentColumn = 0;
            currentRow++;
        }
        if (currentRow >= 4)
        {
            break;
        }
    }

    textureWindow.add<Text>(3,
                            windowHeight - 24,
                            font,
                            std::format("Showing {} textures {} to {} of {}",
                                        category,
                                        scrollPosition * 6 + 1,
                                        std::min(scrollPosition * 6 + 24, (int)textures.size()),
                                        textures.size()));
}

bool WindowTexture::consumeClick(bool clicked, int mouseX, int mouseY)
{
    if (not clicked)
    {
        return false;
    }

    if (mouseX < windowX or mouseX > windowX + windowWidth or mouseY < windowY or mouseY > windowY + windowHeight)
    {
        container = std::nullopt;
        return false;
    }

    textureWindow.consumeClick(mouseX, mouseY);
    return false;
}

void WindowTexture::render(sdl::Renderer& renderer)
{
    textureWindow.render(renderer);
}

void WindowTexture::scroll(int rows)
{
    scrollPosition = std::clamp(scrollPosition - rows, 0, (int)textures.size() / 6 - 3);
    textureWindow.clear();
    fillWindow();
}
} // namespace ui::editor
