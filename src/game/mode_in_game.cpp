#include "mode_in_game.hpp"

#include "engine/engine.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/editor/editor.hpp"
#include "ui/ui.hpp"
#include "world/level.hpp"
#include "world/world.hpp"

#include <cmath>
#include <SDL_scancode.h>
#include <spdlog/spdlog.h>

namespace game
{
ModeInGame::ModeInGame(ui::UI& ui, world::World& world, sdl::Renderer& renderer) :
    ui(ui),
    world(world),
    renderer(renderer)
{}

ModeInGame::~ModeInGame() = default;

void ModeInGame::entry()
{
    auto& level = world.level(1);

    engine = std::make_unique<engine::Engine>(renderer, level);
    editor = std::make_unique<ui::editor::Editor>(level, player.getPosition().x, player.getPosition().y);
    ui.objects.push_back(std::move(editor));
}

void ModeInGame::exit()
{
    engine.reset();
    ui.objects.clear();
}

std::optional<GameMode> ModeInGame::frame(double frameTime)
{
    const uint8_t* keys = sdl::keyboard();

    if (keys[SDL_SCANCODE_Q] or keys[SDL_SCANCODE_ESCAPE])
    {
        return GameMode::Quit;
    }

    static bool raising = true;
    auto& dynamicSector = const_cast<world::Sector&>(world.level(1).sector(2));
    if (dynamicSector.floor >= 0.0) raising = false;
    if (dynamicSector.floor <= -2.0) raising = true;
    dynamicSector.floor += (raising ? 1 : -1) * frameTime;

    double walkDirection = 0.0, rotationDirection = 0.0, strafeDirection = 0.0;

    if (keys[SDL_SCANCODE_DOWN])  walkDirection -= 1;
    if (keys[SDL_SCANCODE_UP])    walkDirection += 1;
    if (keys[SDL_SCANCODE_X])     strafeDirection -= 1;
    if (keys[SDL_SCANCODE_Z])     strafeDirection += 1;
    if (keys[SDL_SCANCODE_RIGHT]) rotationDirection += 1;
    if (keys[SDL_SCANCODE_LEFT])  rotationDirection -= 1;

    player.handleMovement(walkDirection * frameTime, strafeDirection * frameTime, rotationDirection * frameTime, world.level(1));

    engine->frame(player.getPosition());
    engine->draw();

    return noChange;
}
}