#include "mode_in_game.hpp"

#include "engine/engine.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/editor/editor.hpp"
#include "ui/mini_map.hpp"
#include "ui/ui.hpp"
#include "ui/text.hpp"
#include "world/world.hpp"

#include <SDL3/SDL_scancode.h>
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
    world.loadLevel(1, scripting);
    auto& level = world.level(1);

    engine = std::make_unique<engine::Engine>(renderer, level);
    engine->preload();
    ui.add(std::make_unique<ui::MiniMap>(renderer, level, player));

    scripting.bind("choice", &ModeInGame::choice, this);
    scripting.bind("text", &ModeInGame::text, this);
    scripting.bind("speech", &ModeInGame::speech, this);

    spdlog::info("Ready");
}

void ModeInGame::exit()
{
    engine.reset();
    ui.clear();

    scripting.unbind("choice");
    scripting.unbind("text");
    scripting.unbind("speech");
}

void ModeInGame::choice(std::string caption, std::vector<std::string> choices)
{
    state = State::Choice;
    choiceWidget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 32, c::windowHeight - 64, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(choiceWidget));
    text.move(32, c::windowHeight - 100);
    text.write(std::move(caption), "KellySlab", 80);
    int counter{};
    for (auto& c : choices)
    {
        ++counter;
        text.write(std::format("\n{}. ", counter), "RubikDirt", 20);
        text.write(c, "KellySlab", 90);
    }
}

void ModeInGame::text(std::string caption)
{
    state = State::Speech;
    choiceWidget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 32, c::windowHeight - 64, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(choiceWidget));
    text.move(32, c::windowHeight - 64);
    text.write(std::move(caption), "KellySlab", 64);
}

void ModeInGame::speech(std::string person, std::string caption)
{
    state = State::Speech;
    choiceWidget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 32, c::windowHeight - 64, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(choiceWidget));
    text.move(32, c::windowHeight - 64);
    text.write(std::format("{}: ", std::move(person)), "RubikDirt", 48);
    text.write(std::move(caption), "KellySlab", 64);
}

void ModeInGame::event(const sdl::event::Event& event)
{
    if (std::holds_alternative<sdl::event::Key>(event))
    {
        const auto& key = get<sdl::event::Key>(event);

        if (key.direction != sdl::event::Key::Direction::Down)
        {
            return;
        }

        if (state == State::Choice)
        {
            scripting.set("result", key.scancode - SDL_SCANCODE_1 + 1);
            ui.remove(choiceWidget);
            state = State::Default;
            scripting.resume();
            return;
        }

        switch (key.scancode)
        {
        case SDL_SCANCODE_RETURN:
            if (state == State::Speech)
            {
                ui.remove(choiceWidget);
                state = State::Default;
                scripting.resume();
                return;
            }

            if (tooltipWidget)
            {
                scripting.runAsCoroutine(std::format("scripts/levels/1/{}", tooltipWidget->second));
                ui.remove(tooltipWidget->first);
                tooltipWidget = std::nullopt;
            }
            break;
        }
    }
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

    if (keys[SDL_SCANCODE_F2])
    {
        spdlog::info("Activating map editor");
        ui.clear();
        ui.add(std::make_unique<ui::editor::Editor>(world.level(1), ui.fonts.get("TitilliumWeb", 14)));
    }

    if (state == State::Default)
    {
        if (keys[SDL_SCANCODE_DOWN]) player.move(world.level(1), Player::Direction::Backward);
        if (keys[SDL_SCANCODE_UP]) player.move(world.level(1), Player::Direction::Forward);
        if (keys[SDL_SCANCODE_LEFT]) player.rotate(Player::Rotation::Left);
        if (keys[SDL_SCANCODE_RIGHT]) player.rotate(Player::Rotation::Right);
    }

    player.frame(world.level(1), frameTime);

    if (player.getPosition().x != lastFrameX or
        player.getPosition().y != lastFrameY or
        player.getPosition().z != lastFrameZ or
        player.getPosition().angle != lastFrameAngle)
    {
        auto lookX = player.getPosition().x + std::cos(player.getPosition().angle);
        auto lookY = player.getPosition().y + std::sin(player.getPosition().angle);

        if (auto script = world.level(1).checkScript(player.getPosition().sector, lookX, lookY))
        {
            if (tooltipWidget and tooltipWidget->second != *script)
            {
                ui.remove(tooltipWidget->first);
            }
            if (not tooltipWidget or tooltipWidget->second != *script)
            {
                tooltipWidget = {ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth, c::windowHeight), *script};
                auto& text = dynamic_cast<ui::Text&>(ui.get(tooltipWidget->first));
                const static std::string interactionPrompt = "Press [Enter] to interact";
                text.move(c::windowWidth / 2 - ui.fonts.get("KellySlab", 32).size(interactionPrompt).first / 2, c::windowHeight * 3 / 4);
                text.write(interactionPrompt, "KellySlab", 50);
            }
        }
        else
        {
            if (tooltipWidget)
            {
                ui.remove(tooltipWidget->first);
                tooltipWidget = std::nullopt;
            }
        }

        lastFrameX = player.getPosition().x;
        lastFrameY = player.getPosition().y;
        lastFrameZ = player.getPosition().z;
        lastFrameAngle = player.getPosition().angle;
    }

    int playerX = static_cast<int>(player.getPosition().x);
    int playerY = static_cast<int>(player.getPosition().y);
    int playerZ = static_cast<int>(player.getPosition().z);

    if (playerX != lastX or playerY != lastY or playerZ != lastZ)
    {
        lastX = playerX;
        lastY = playerY;
        lastZ = playerZ;
        scripting.sectorEntry(playerX, playerY, playerZ);
    }

    engine->frame(player.getPosition());
    engine->draw();

    return noChange;
}
}