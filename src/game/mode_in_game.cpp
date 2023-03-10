#include "mode_in_game.hpp"

#include "engine/engine.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/mini_map.hpp"
#include "ui/ui.hpp"
#include "world/world.hpp"

#include <SDL_scancode.h>
#include <spdlog/spdlog.h>

namespace game
{
ModeInGame::ModeInGame(ui::UI& ui,
                       world::World& world,
                       sdl::Renderer& renderer,
                       scripting::Scripting& scripting) :
    ui(ui),
    world(world),
    renderer(renderer),
    scripting(scripting)
{}

ModeInGame::~ModeInGame() = default;

void ModeInGame::entry()
{
    auto& level = world.level(1);

    engine = std::make_unique<engine::Engine>(renderer, level);
    ui.add(std::make_unique<ui::MiniMap>(renderer, level, player));
    scripting.run("scripts/levels/entry.lua");
}

void ModeInGame::exit()
{
    engine.reset();
    ui.clear();
}

std::optional<GameMode> ModeInGame::frame(double frameTime)
{
    const uint8_t* keys = sdl::keyboard();

    if (keys[SDL_SCANCODE_Q] or keys[SDL_SCANCODE_ESCAPE])
    {
        return GameMode::Quit;
    }

    if (keys[SDL_SCANCODE_F1])
    {
        spdlog::info("Exporting level\n{}", world.level(1).toLua());
    }

    if (keys[SDL_SCANCODE_DOWN])  player.move(Player::Direction::Backward);
    if (keys[SDL_SCANCODE_UP])    player.move(Player::Direction::Forward);
    if (keys[SDL_SCANCODE_X])     player.move(Player::Direction::Left);
    if (keys[SDL_SCANCODE_Z])     player.move(Player::Direction::Right);
    if (keys[SDL_SCANCODE_LEFT])  player.rotate(Player::Rotation::Left);
    if (keys[SDL_SCANCODE_RIGHT]) player.rotate(Player::Rotation::Right);

    player.frame(world.level(1), frameTime);

    int playerX = static_cast<int>(player.getPosition().x);
    int playerY = static_cast<int>(player.getPosition().y);
    if (playerX != lastX || playerY != lastY)
    {
        lastX = playerX;
        lastY = playerY;
        scripting.sectorEntry(playerX, playerY);
    }

    engine->frame(player.getPosition());
    engine->draw();

    return noChange;
}
}