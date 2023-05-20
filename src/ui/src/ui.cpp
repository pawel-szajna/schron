#include "ui.hpp"

#include "object.hpp"
#include "sdlwrapper/renderer.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
UI::UI(sdl::Renderer& renderer)
    : renderer{renderer}
    , base{std::make_shared<Object>()}
{
    SPDLOG_DEBUG("UI initialized");
}

UI::~UI() = default;

void UI::add(Widget widget)
{
    base->attach(std::move(widget));
}

void UI::clear()
{
    base->detachAll();
}

void UI::render()
{
    base->render(renderer);
}

void UI::event(const sdl::event::Event& event)
{
    base->event(event);
}
} // namespace ui
