#include "editor/widgets.hpp"

#include "sdlwrapper/texture.hpp"

namespace ui::editor
{
Group::Group() = default;

Group::~Group() = default;

void Group::render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY)
{
    for (auto& [widgetX, widgetY, widget] : children)
    {
        if (widget != nullptr)
        {
            widget->render(renderer, target, absX + widgetX, absY + widgetY, widgetX, widgetY);
        }
    }
}

bool Group::consumeClick(int mouseX, int mouseY)
{
    for (auto& [widgetX, widgetY, widget] : children)
    {
        if (not widget->consumeClick(mouseX - widgetX, mouseY - widgetY))
        {
            return false;
        }
    }

    return true;
}

void Group::clear()
{
    children.clear();
}

Text::Text(sdl::Font& font,
           const std::string& text) :
    font(font),
    buffer(0, 0)
{
    setCaption(text);
}

Text::~Text() = default;

void Text::setCaption(const std::string& text)
{
    if (text != caption)
    {
        buffer = font.render(text, sdl::Color{0, 0, 0, 255});
        caption = text;
    }
}

std::pair<int, int> Text::getSize() const
{
    return std::make_pair(buffer.width, buffer.height);
}

void Text::render(sdl::Renderer& renderer, sdl::Surface& target, int, int, int offsetX, int offsetY)
{
    buffer.render(target, sdl::Rectangle{offsetX, offsetY, 0, 0});
}

bool Text::consumeClick(int mouseX, int mouseY)
{
    return true;
}

Button::Button(int width, int height,
               sdl::Font& font,
               const std::string& caption,
               std::function<void(void)> onClick) :
    width(width), height(height),
    caption(font, caption),
    onClick(std::move(onClick))
{}

Button::~Button() = default;

void Button::render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY)
{
    renderer.renderRectangle(absX, absY, width, height, 220, 220, 220, 240);
    renderer.renderBox(absX, absY, width, height, 100, 100, 100, 200);
    caption.render(renderer, target, absX, absY, offsetX + width / 2 - caption.getSize().first / 2, offsetY - 4);
}

bool Button::consumeClick(int mouseX, int mouseY)
{
    if (mouseX < 0 or mouseY < 0 or mouseX > width or mouseY > height)
    {
        return true;
    }

    onClick();
    return false;
}

void Button::setCaption(const std::string& text)
{
    caption.setCaption(text);
}

Window::Window(int x, int y, int width, int height) :
    x(x), y(y),
    width(width), height(height),
    buffer(width, height)
{}

Window::~Window() = default;

bool Window::consumeClick(int mouseX, int mouseY)
{
    mouseX -= x;
    mouseY -= y;

    if (mouseX < 0 or mouseY < 0 or mouseX > width or mouseY > height)
    {
        return true;
    }

    Group::consumeClick(mouseX, mouseY);
    return false;
}

void Window::render(sdl::Renderer& renderer)
{
    auto surface = sdl::Surface(width, height);
    renderer.renderRectangle(x, y, width, height, 200, 200, 200, 220);
    renderer.renderBox(x, y, width, height, 100, 100, 100, 220);
    render(renderer, surface, x, y, 0, 0);
    auto texture = sdl::Texture(renderer, sdl::Texture::Access::Streaming, width, height);
    texture.setBlendMode(sdl::BlendMode::Blend);
    surface.render(texture);
    renderer.copy(texture, std::nullopt, sdl::FRectangle{(float)x, (float)y, (float)width, (float)height});
}

void Window::render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY)
{
    Group::render(renderer, target, absX, absY, offsetX, offsetY);
}

void Window::move(int x, int y)
{
    this->x = x;
    this->y = y;
}

Image::Image(int width, int height, const std::string& filename) :
    width(width), height(height),
    buffer(filename)
{}

Image::~Image() = default;

bool Image::consumeClick(int mouseX, int mouseY)
{
    return true;
}

void Image::render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY)
{
    buffer.renderScaled(target, sdl::Rectangle{offsetX, offsetY, width, height});
}
}
