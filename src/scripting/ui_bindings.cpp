#include "ui_bindings.hpp"

#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace scripting
{
UiBindings::UiBindings(sol::state& lua, ui::UI& ui, sdl::Renderer& renderer) :
    lua(lua),
    ui(ui),
    renderer(renderer)
{
    lua.set_function("ui_createText", &UiBindings::createText, this);
    lua.set_function("ui_textClear", &UiBindings::textClear, this);
    lua.set_function("ui_textWrite", &UiBindings::textWrite, this);
}

int UiBindings::createText()
{
    spdlog::debug("UiBindings::createText()");
    auto text = std::make_unique<ui::Text>(renderer, ui.fonts, c::windowWidth - 64, 192, 32, c::windowHeight - 192 - 32);
    return ui.add(std::move(text));
}

void UiBindings::textClear(int id)
{
    spdlog::debug("UiBindings::textClear(id={})", id);
    auto& text = dynamic_cast<ui::Text&>(ui.get(id));
    text.clear();
}

void UiBindings::textWrite(int id, std::string message, std::string font, int speed)
{
    spdlog::debug("UiBindings::textWrite(id={}, message={}, font={}, speed={})", id, message, font, speed);
    auto& text = dynamic_cast<ui::Text&>(ui.get(id));
    text.write(std::move(message), std::move(font), speed, true);
}
}
