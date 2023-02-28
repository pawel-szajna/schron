#include "mode_in_game.hpp"

#include "engine/engine.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/editor/editor.hpp"
#include "ui/ui.hpp"
#include "world/level.hpp"
#include "world/world.hpp"

#include <cmath>
#include <spdlog/spdlog.h>

namespace game
{
ModeInGame::ModeInGame(ui::UI& ui, world::World& world, sdl::Surface& target) :
    ui(ui),
    world(world),
    target(target)
{}

ModeInGame::~ModeInGame() = default;

void ModeInGame::entry()
{
    auto& level = world.level(1);

    engine = std::make_unique<engine::Engine>(level);
    editor = std::make_unique<ui::editor::Editor>(level, playerX, playerY);
    ui.objects.push_back(std::move(editor));
}

void ModeInGame::exit()
{
    engine.reset();
    ui.objects.clear();
}

std::optional<GameMode> ModeInGame::frame(double frameTime)
{
    uint8_t* keys = SDL_GetKeyState(nullptr);

    static bool raising = true;
    auto& dynamicSector = const_cast<world::Sector&>(world.level(1).sector(2));
    if (dynamicSector.floor >= 0.0) raising = false;
    if (dynamicSector.floor <= -2.0) raising = true;
    dynamicSector.floor += (raising ? 1 : -1) * frameTime;

    double walkFactor = 0.0, rotationFactor = 0.0, strafeFactor = 0.0;

    if (keys[SDLK_DOWN]) walkFactor -= 1;
    if (keys[SDLK_UP])   walkFactor += 1;
    if (keys[SDLK_RIGHT]) rotationFactor += 1;
    if (keys[SDLK_LEFT])  rotationFactor -= 1;
    if (keys[SDLK_q] || keys[SDLK_ESCAPE]) return GameMode::Quit;

    if (walkFactor != 0)
    {
        auto movementSpeed = 1.5 * frameTime;
        playerX += movementSpeed * (walkFactor * std::cos(playerAngle) + strafeFactor * std::sin(playerAngle));
        playerY += movementSpeed * (walkFactor * std::sin(playerAngle) - strafeFactor * std::cos(playerAngle));
    }

    if (rotationFactor != 0)
    {
        playerAngle += rotationFactor * frameTime * 2.0;
    }

    engine->frame(playerX, playerY, playerZ, playerAngle);
    engine->draw(target);

    return noChange;
}
}