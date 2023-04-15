#include "mode_main_menu.hpp"

#include "scripting/scripting.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/ui.hpp"

#include <SDL3/SDL_scancode.h>

namespace game
{
ModeMainMenu::ModeMainMenu(scripting::Scripting& scripting, ui::UI& ui)
    : scripting(scripting)
    , ui(ui)
{
}

void ModeMainMenu::entry()
{
    scripting.run("scripts/menu/menu.lua");
    ui.clear();
}

std::optional<GameMode> ModeMainMenu::frame(double frameTime)
{
    return GameMode::InGame;
}

void ModeMainMenu::event(const sdl::event::Event& event) {}

void ModeMainMenu::exit() {}

} // namespace game
