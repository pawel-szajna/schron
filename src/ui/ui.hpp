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

class UI
{
public:

    explicit UI(sdl::Renderer& renderer);
    ~UI();

    int add(std::unique_ptr<Object>&& object);
    Object& get(int id);
    void remove(int id);
    void clear();

    template<typename T>
    requires std::derived_from<T, Object>
    T& get_as(int id)
    {
        return dynamic_cast<T&>(get(id));
    }

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
