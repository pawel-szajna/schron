#include "game.hpp"

#include "world/world.hpp"
#include "sdlwrapper/sdlwrapper.hpp"

namespace game
{
Game::Game() :
    world(std::make_unique<world::World>())
{
    sdl::initialize();
}

Game::~Game()
{
    sdl::teardown();
}

void Game::start()
{
}
}
