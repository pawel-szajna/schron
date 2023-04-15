#include "game.hpp"

#include "engine/engine.hpp"
#include "game_mode.hpp"
#include "mode_in_game.hpp"
#include "mode_init.hpp"
#include "mode_main_menu.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"
#include "util/constants.hpp"
#include "util/format.hpp"
#include "world/world.hpp"

#include <spdlog/spdlog.h>

namespace game
{
Game::Game()
    : world(std::make_unique<world::World>())
    , ui(std::make_unique<ui::UI>(renderer))
    , scripting(std::make_unique<scripting::Scripting>(*ui, *world, renderer))
{
    modes.emplace(GameMode::Init, std::make_unique<DummyMode>());
    modes.emplace(GameMode::MainMenu, std::make_unique<ModeMainMenu>(*scripting, *ui));
    modes.emplace(GameMode::InGame, std::make_unique<ModeInGame>(*ui, *world, renderer, *scripting, noiseLevel));
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
    uint64_t newTime{}, oldTime{}, windowUpdateTime{};
    uint64_t frameStartTime{0}, frameStartTimeOld{0}, renderEndTime{0}, noiseEndTime{0}, uiEndTime{0};
    int framesCounter{};
    sdl::Surface statsSurface(c::windowWidth, c::windowHeight);
    sdl::Texture statsTexture(renderer, sdl::Texture::Access::Streaming, c::windowWidth, c::windowHeight);
    statsTexture.setBlendMode(sdl::BlendMode::Add);

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

        oldTime        = newTime;
        newTime        = sdl::currentTime();
        auto frameTime = (double)(newTime - oldTime) / 1000.0;

        renderer.clear();

        frameStartTimeOld   = frameStartTime;
        frameStartTime      = sdl::currentTimeNs();
        auto frameTotalTime = frameStartTime - frameStartTimeOld;
        auto miscTime       = frameStartTime - uiEndTime;

        auto stateChange = modes.at(mode)->frame(frameTime);
        if (stateChange.has_value())
        {
            switchMode(*stateChange);
        }

        renderEndTime   = sdl::currentTimeNs();
        auto renderTime = renderEndTime - frameStartTime;

        noise.render();
        noiseEndTime   = sdl::currentTimeNs();
        auto noiseTime = noiseEndTime - renderEndTime;

        ui->render();
        uiEndTime   = sdl::currentTimeNs();
        auto uiTime = uiEndTime - noiseEndTime;

        if (c::renderStats)
        {
            auto& font = ui->fonts.get("TitilliumWeb", 16);
            statsSurface.empty();
            font.render(std::format("render: {:.4f}ms", (double)renderTime / 1'000'000.0),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 8, 0, 0});
            font.render(std::format("lighting: {:.4f}ms", (double)engine::lightingTime / 1'000'000.0),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{48, 24, 0, 0});
            font.render(std::format("geometry: {:.4f}ms", (double)engine::geometryTime / 1'000'000.0),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{48, 40, 0, 0});
            font.render(std::format("sprites: {:.4f}ms", (double)engine::spritesTime / 1'000'000.0),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{48, 56, 0, 0});
            font.render(std::format("noise: {:.4f}ms", (double)noiseTime / 1'000'000.0), sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 72, 0, 0});
            font.render(std::format("ui: {:.4f}ms", (double)uiTime / 1'000'000.0), sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 88, 0, 0});
            font.render(std::format("other: {:.4f}ms", (double)miscTime / 1'000'000.0), sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 104, 0, 0});
            font.render(std::format("total: {:.4f}ms", (double)frameTotalTime / 1'000'000.0),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 120, 0, 0});
            font.render(std::format("fps: {:.1f}", 1'000'000'000.0 / (double)frameTotalTime),
                        sdl::Color{255, 255, 255, 216})
                .render(statsSurface, sdl::Rectangle{16, 136, 0, 0});
            statsSurface.render(statsTexture);
            renderer.copy(statsTexture);
        }

        renderer.present();

        if (newTime - windowUpdateTime >= 1000)
        {
            window.setTitle(std::format("Schron ({} fps)", framesCounter));
            framesCounter    = 0;
            windowUpdateTime = newTime;
        }
    }
}
} // namespace game
