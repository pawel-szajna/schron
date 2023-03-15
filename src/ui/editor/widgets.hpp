#pragma once

#include "sdlwrapper/font.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/surface.hpp"

#include <concepts>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace ui::editor
{
class Widget
{
public:

    Widget() = default;
    Widget(const Widget&) = delete;
    Widget(Widget&&) = default;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = default;

    virtual ~Widget() = default;
    virtual void render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY) = 0;
    virtual bool consumeClick(int mouseX, int mouseY) = 0;
};

class Text : public Widget
{
public:

    Text(sdl::Font& font, const std::string& text);
    virtual ~Text();

    void setCaption(const std::string& text);
    [[nodiscard]] std::pair<int, int> getSize() const;

    void render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY) override;
    bool consumeClick(int mouseX, int mouseY) override;

private:

    sdl::Font& font;
    sdl::Surface buffer;
};

class Button : public Widget
{
public:

    Button(int width, int height, sdl::Font& font, const std::string& caption, std::function<void(void)> onClick);
    virtual ~Button();

    void render(sdl::Renderer &renderer, sdl::Surface &target, int absX, int absY, int offsetX, int offsetY) override;
    bool consumeClick(int mouseX, int mouseY) override;

private:

    int width, height;
    Text caption;
    std::function<void(void)> onClick;
};

class Window : public Widget
{
public:

    Window(int x, int y, int width, int height);
    ~Window();

    bool consumeClick(int mouseX, int mouseY) override;
    void render(sdl::Renderer& renderer);
    void render(sdl::Renderer& renderer, sdl::Surface& target, int absX, int absY, int offsetX, int offsetY) override;

    template<typename T, typename... Args>
    requires std::derived_from<T, Widget>
    T& add(int widgetX, int widgetY, Args&&... args)
    {
        widgets.emplace_back(widgetX, widgetY, std::make_unique<T>(std::forward<Args>(args)...));
        return dynamic_cast<T&>(*std::get<std::unique_ptr<Widget>>(widgets.back()));
    }

private:

    int x, y;
    int width, height;

    sdl::Surface buffer;
    std::vector<std::tuple<int, int, std::unique_ptr<Widget>>> widgets;
};

}