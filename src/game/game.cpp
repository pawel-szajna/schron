#include "game.hpp"

#include "game_mode.hpp"
#include "mode_in_game.hpp"
#include "mode_init.hpp"
#include "mode_main_menu.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/ui.hpp"
#include "util/constants.hpp"
#include "util/format.hpp"
#include "world/world.hpp"

#include <spdlog/spdlog.h>

namespace game
{
Game::Game() :
    world(std::make_unique<world::World>()),
    ui(std::make_unique<ui::UI>(renderer)),
    scripting(std::make_unique<scripting::Scripting>(*ui, *world, renderer))
{
    modes.emplace(GameMode::Init,     std::make_unique<DummyMode>());
    modes.emplace(GameMode::MainMenu, std::make_unique<ModeMainMenu>(*scripting, *ui));
    modes.emplace(GameMode::InGame,   std::make_unique<ModeInGame>(*ui, *world, renderer, *scripting));
    modes.emplace(GameMode::Quit,     std::make_unique<DummyMode>());

    spdlog::debug("Game initialization complete");
}

Game::~Game() = default;

void Game::start()
{
    switchMode(GameMode::InGame);
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
    double newTime{}, oldTime{}, windowUpdateTime{};
    int framesCounter{};

    while (++framesCounter, mode != GameMode::Quit)
    {
        sdl::event::Event event;
        while ((event = sdl::pollEvent()), not std::holds_alternative<sdl::event::None>(event))
        {
            if (std::holds_alternative<sdl::event::Quit>(event))
            {
                switchMode(GameMode::Quit);
                continue;
            }

            modes.at(mode)->event(event);
            ui->event(event);
        }

        oldTime = newTime;
        newTime = sdl::currentTime();
        auto frameTime = (newTime - oldTime) / 1000.0;
        if (frameTime < c::frameLimit)
        {
            sdl::delay(c::frameLimit - frameTime);
        }

        renderer.clear();

        auto stateChange = modes.at(mode)->frame(frameTime);
        if (stateChange.has_value())
        {
            switchMode(*stateChange);
        }

        noise.render();
        ui->render();
        renderer.present();

        if (newTime - windowUpdateTime >= 1000)
        {
            window.setTitle(std::format("Schron ({} fps)", framesCounter));
            framesCounter = 0;
            windowUpdateTime = newTime;
        }
    }
}
}
