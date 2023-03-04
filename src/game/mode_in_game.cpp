#include "mode_in_game.hpp"

#include "engine/engine.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/mini_map.hpp"
#include "ui/text.hpp"
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
    ui.add(std::make_unique<ui::MiniMap>(renderer, level, player));
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

    if (keys[SDL_SCANCODE_1])
    {
        auto text = std::make_unique<ui::Text>(renderer, ui.fonts, 1024 - 64, 192, 32, 768 - 192 - 32);
        auto& textRef = *text;
        ui.add(std::move(text));
        textRef.writeAnimated("Cyprian Kamil Norwid: ", "RubikDirt", 16);
        textRef.writeAnimated("Do kraju tego, gdzie kruszynę chleba podnoszą z ziemi przez uszanowanie "
                              "dla darów nieba… tęskno mi, Panie! Do kraju tego, gdzie winą jest dużą "
                              "popsować gniazdo na gruszy bocianie – bo wszystkim służą… tęskno mi, Panie! "
                              "Do kraju tego, gdzie pierwsze ukłony są jak odwieczne Chrystusa wyznanie: "
                              "b ą d ź p o c h w a l o n y! Tęskno mi, "
                              "Panie! I tak być musi, choć się tak nie stanie.", "KellySlab", 32);
    }

    if (keys[SDL_SCANCODE_DOWN])  player.move(Player::Direction::Backward);
    if (keys[SDL_SCANCODE_UP])    player.move(Player::Direction::Forward);
    if (keys[SDL_SCANCODE_X])     player.move(Player::Direction::Left);
    if (keys[SDL_SCANCODE_Z])     player.move(Player::Direction::Right);
    if (keys[SDL_SCANCODE_LEFT])  player.rotate(Player::Rotation::Left);
    if (keys[SDL_SCANCODE_RIGHT]) player.rotate(Player::Rotation::Right);

    player.frame(world.level(1), frameTime);

    engine->frame(player.getPosition());
    engine->draw();

    return noChange;
}
}