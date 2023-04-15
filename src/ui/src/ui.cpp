#include "ui.hpp"

#include "object.hpp"
#include "sdlwrapper/renderer.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
UI::UI(sdl::Renderer& renderer)
    : renderer(renderer)
{
    spdlog::debug("UI initialized");
}

UI::~UI() = default;

int UI::add(std::unique_ptr<Object>&& object)
{
    objects.push_back(std::move(object));
    return counter++;
}

Object& UI::get(int id)
{
    return *objects.at(id);
}

void UI::clear()
{
    objects.clear();
    counter = 0;
}

void UI::remove(int id)
{
    objects[id] = nullptr;
}

void UI::render()
{
    for (auto& object : objects)
    {
        if (object != nullptr)
        {
            object->render(renderer);
        }
    }
}

void UI::event(const sdl::event::Event& event)
{
    for (auto& object : objects)
    {
        if (object != nullptr)
        {
            object->event(event);
        }
    }
}
} // namespace ui
