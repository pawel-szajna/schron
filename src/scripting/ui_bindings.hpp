#pragma once

#include <string>

namespace ui
{
class UI;
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

    UiBindings(sol::state& lua, ui::UI& ui);

private:

    [[maybe_unused]] void keyHandlerMap(int keycode, std::string callback);
    [[maybe_unused]] void keyHandlerUnmap(int keycode);

    sol::state& lua;
    ui::UI& ui;

};
}