#pragma once

#include "fonts.hpp"
#include "object.hpp"
#include "sdlwrapper/event.hpp"

#include <memory>
#include <vector>

namespace sdl
{
class Font;
class Renderer;
} // namespace sdl

namespace ui
{
/**
 * @class UI
 * @brief Main UI management class.
 *
 * This class is the owner of all currently active UI widgets and is responsible for
 * calling the functions to update and render them.
 */
class UI
{
public:

    explicit UI(sdl::Renderer& renderer);
    ~UI();

    void add(Widget widget);
    void clear();

    /**
     * @brief Constructs a widget.
     *
     * This function constructs a widget, by forwarding its parameters to the widget
     * constructor, and registers it in the active widgets collection.
     *
     * @tparam T Widget type
     * @param args Widget constructor parameters
     * @return Widget
     */
    template<typename T, typename... Args>
    requires std::derived_from<T, Object>
    std::shared_ptr<T> add(Args&&... args)
    {
        auto widget = std::make_shared<T>(std::forward<Args>(args)...);
        add(widget);
        return widget;
    }

    void event(const sdl::event::Event& event);
    void render();

    Fonts fonts{};

private:

    sdl::Renderer& renderer;
    Widget base;
};
} // namespace ui
