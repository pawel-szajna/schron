#include "game.hpp"

#include "game_mode.hpp"
#include "mode_init.hpp"
#include "mode_main_menu.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/ui.hpp"
#include "util/constants.hpp"
#include "world/world.hpp"

#include <spdlog/spdlog.h>

namespace game
{
Game::Game() :
    mainWindow(sdl::make_main_window(800, 600, false)),
    screen(sdl::make_surface(c::renderWidth, c::renderHeight)),
    world(std::make_unique<world::World>()),
    ui(std::make_unique<ui::UI>(mainWindow)),
    scripting(std::make_unique<scripting::Scripting>(*ui, *world))
{
    modes.emplace(GameMode::Init, std::make_unique<DummyMode>());
    modes.emplace(GameMode::MainMenu, std::make_unique<ModeMainMenu>());
    modes.emplace(GameMode::Quit, std::make_unique<DummyMode>());

    spdlog::debug("Game initialization complete");
}

Game::~Game() = default;

void Game::start()
{
    switchMode(GameMode::MainMenu);
    mainLoop();
}

void Game::switchMode(GameMode target)
{
    if (not modes.contains(target))
    {
        spdlog::warn("Mode change requested, but no mode handler found");
        return;
    }

    modes.at(mode)->exit();
    mode = target;
    modes.at(mode)->entry();
}

void Game::mainLoop()
{
    double newTime{}, oldTime{};

    while (mode != GameMode::Quit)
    {
        sdl::pollEvents();

        if (sdl::quitEvent())
        {
            switchMode(GameMode::Quit);
        }

        oldTime = newTime;
        newTime = sdl::currentTime();
        auto frameTime = (newTime - oldTime) / 1000.0;
        if (frameTime < c::frameLimit)
        {
            sdl::delay(c::frameLimit - frameTime);
        }

        screen.clear();

        auto stateChange = modes.at(mode)->frame(frameTime);
        if (stateChange.has_value())
        {
            switchMode(*stateChange);
        }

        noise.render(screen);
        screen.draw(mainWindow);
        ui->render();
        mainWindow.update();

        sdl::setTitle(std::format("Schron ({} fps)", (int)(1 / frameTime)));
    }
}
}
