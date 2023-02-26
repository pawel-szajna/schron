#include "ui_bindings.hpp"

#include "ui/ui.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace scripting
{
UiBindings::UiBindings(sol::state& lua, ui::UI& ui) :
    lua(lua),
    ui(ui)
{
    lua.set_function("ui_keyHandler_map", &UiBindings::keyHandlerMap, this);
    lua.set_function("ui_keyHandler_unmap", &UiBindings::keyHandlerUnmap, this);
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
