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

void UI::add(std::unique_ptr<Object>&& object)
{
    objects.push_back(std::move(object));
}

void UI::clear()
{
    objects.clear();
}

void UI::render()
{
    keyHandler.check();

    std::erase_if(objects, [](const auto& obj) { return obj == nullptr; });

    for (auto& object : objects)
    {
        object->render(renderer);
    }
}
}
