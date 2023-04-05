#pragma once

#include <string>

namespace ui
{
class UI;
}

namespace sdl
{
class Renderer;
}

namespace sol
{
class state;
}

namespace scripting
{
class UiBindings
{
public:

    UiBindings(sol::state& lua, ui::UI& ui, sdl::Renderer& renderer);

private:

    [[maybe_unused]] int createText();
    [[maybe_unused]] void keyHandlerMap(int keycode, std::string callback);
    [[maybe_unused]] void keyHandlerUnmap(int keycode);
    [[maybe_unused]] void textClear(int id);
    [[maybe_unused]] void textWrite(int id, std::string message, std::string font, int speed);

    sol::state& lua;
    ui::UI& ui;
    sdl::Renderer& renderer;

};
}