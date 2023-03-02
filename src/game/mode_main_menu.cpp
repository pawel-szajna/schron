#include "mode_main_menu.hpp"

#include "scripting/scripting.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/ui.hpp"

#include <SDL_keycode.h>

namespace game
{
ModeMainMenu::ModeMainMenu(scripting::Scripting& scripting, ui::UI& ui) :
    scripting(scripting),
    ui(ui)
{}

void ModeMainMenu::entry()
{
    scripting.run("scripts/menu/menu.lua");
}

std::optional<GameMode> ModeMainMenu::frame(double frameTime)
{
    if (sdl::keyPressed(SDLK_ESCAPE))
    {
        return GameMode::Quit;
    }

    return noChange;
}

void ModeMainMenu::exit()
{
    ui.keyHandler.reset();
}

}