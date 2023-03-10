#include "ui_bindings.hpp"

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
    lua.set_function("ui_keyHandler_map", &UiBindings::keyHandlerMap, this);
    lua.set_function("ui_keyHandler_unmap", &UiBindings::keyHandlerUnmap, this);
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

void UiBindings::keyHandlerMap(int keycode, std::string callback)
{
    spdlog::debug("UiBindings::keyHandlerMap({}, {})", keycode, callback);
    ui.keyHandler.map(keycode, [callback = std::move(callback), &lua = this->lua]()
                               {
                                   spdlog::debug("Callback function {}", callback);
                                   try
                                   {
                                       lua[callback]();
                                   }
                                   catch (std::runtime_error& error)
                                   {
                                       spdlog::error("LUA error: {}", error.what());
                                   }
                               });
}

void UiBindings::keyHandlerUnmap(int keycode)
{
    ui.keyHandler.unmap(keycode);
}
}
