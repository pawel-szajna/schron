#include "ui.hpp"

#include "object.hpp"
#include "sdlwrapper/renderer.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
UI::UI(sdl::Renderer& renderer) :
    renderer(renderer)
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

void UI::render()
{
    keyHandler.check();

    for (auto& object : objects)
    {
        if (object != nullptr)
        {
            object->render(renderer);
        }
    }
}
}
