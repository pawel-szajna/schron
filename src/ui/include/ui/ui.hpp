#pragma once

#include "fonts.hpp"
#include "sdlwrapper/event.hpp"

#include <memory>
#include <vector>

namespace sdl
{
class Font;
class Renderer;
}

namespace ui
{
class Object;

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

    /**
     * @brief Emplaces a widget in the UI.
     *
     * This function takes ownership of the widget (as it is passed by move inside
     * a unique pointer) and emplaces it in the collection of widgets managed (i.e.
     * updated and rendered) by the main UI class.
     *
     * @param object The widget to emplace
     * @return Widget ID
     */
    int add(std::unique_ptr<Object>&& object);

    /**
     * @brief Retrieves given widget.
     * @param id Widget ID to retrieve
     * @return Widget object
     */
    Object& get(int id);

    /**
     * @brief Removes given widget.
     * @param id Widget ID to remove
     */
    void remove(int id);

    /**
     * @brief Removes all widgets.
     */
    void clear();

    /**
     * @brief Retrieves given widget.
     *
     * Retrieves a widget with given ID as a requested type. This function does not
     * perform any type checks and can result in a bad cast, if an invalid type is
     * requested for given ID.
     *
     * @tparam T Widget type
     * @param id Widget ID
     * @return Widget object (cast to type T)
     */
    template<typename T>
    requires std::derived_from<T, Object>
    T& get_as(int id)
    {
        return dynamic_cast<T&>(get(id));
    }

    /**
     * @brief Constructs a widget.
     *
     * This function constructs a widget, by forwarding its parameters to the widget
     * constructor, and registers it in the active widgets collection.
     *
     * @tparam T Widget type
     * @param args Widget constructor parameters
     * @return Widget ID
     */
    template<typename T, typename... Args>
    requires std::derived_from<T, Object>
    int add(Args&& ...args)
    {
        return add(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void event(const sdl::event::Event& event);
    void render();

    Fonts fonts{};

private:

    sdl::Renderer& renderer;
    std::vector<std::unique_ptr<Object>> objects;
    int counter{0};
};
}
