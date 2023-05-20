#include "mode_in_game.hpp"

#include "dialogue.hpp"
#include "engine/engine.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/editor/editor.hpp"
#include "ui/mini_map.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"
#include "world/world.hpp"

#include <fstream>
#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>

namespace game
{
ModeInGame::ModeInGame(
    ui::UI& ui, world::World& world, sdl::Renderer& renderer, scripting::Scripting& scripting, int& noiseLevel)
    : ui(ui)
    , world(world)
    , renderer(renderer)
    , scripting(scripting)
    , player(scripting, noiseLevel)
{
}

ModeInGame::~ModeInGame() = default;

void ModeInGame::entry()
{
    world.loadLevel(1, scripting);
    auto& level = world.level(1);

    engine = std::make_unique<engine::Engine>(renderer, level);
    engine->preload();
    ui.add(std::make_unique<ui::MiniMap>(renderer, level, player));

    scripting.bindYielding("dialogue_start", &ModeInGame::startDialogue, this);
    scripting.bindYielding("dialogue_end", &ModeInGame::endDialogue, this);

    SPDLOG_INFO("Ready");
}

void ModeInGame::exit()
{
    engine.reset();
    ui.clear();
    subMode.reset();

    scripting.unbind("dialogue_start", "dialogue_end");
}

void ModeInGame::startDialogue()
{
    if (tooltipWidget)
    {
        tooltipWidget->first->detach();
        tooltipWidget = std::nullopt;
    }
    subMode = std::make_unique<Dialogue>(player, renderer, scripting, ui);
}

void ModeInGame::endDialogue()
{
    subMode.reset();
}

void ModeInGame::save(const std::string& filename)
{
    if (subMode != nullptr)
    {
        notify(5000, "Saving disabled!");
        return;
    }

    std::ofstream file{filename};
    save(file);
    notify(3000, "Game saved");
}

void ModeInGame::save(std::ostream& os) const
{
    player.save(os);
    scripting.save(os);
}

void ModeInGame::event(const sdl::event::Event& event)
{
    if (subMode != nullptr)
    {
        subMode->event(event);
        return;
    }

    if (std::holds_alternative<sdl::event::Key>(event))
    {
        const auto& key = get<sdl::event::Key>(event);

        if (key.direction != sdl::event::Key::Direction::Down)
        {
            return;
        }

        switch (key.scancode)
        {
        case SDL_SCANCODE_LEFTBRACKET:
            player.modSanity(-1);
            SPDLOG_INFO("sanity = {}", player.getSanity());
            break;
        case SDL_SCANCODE_RIGHTBRACKET:
            player.modSanity(1);
            SPDLOG_INFO("sanity = {}", player.getSanity());
            break;
        case SDL_SCANCODE_F5:
            shouldSave = true;
            break;
        case SDL_SCANCODE_F9:
            scripting.run("save.lua");
            player.setSanity(player.getSanity());
            break;
        case SDL_SCANCODE_RETURN:
            if (tooltipWidget)
            {
                scripting.runAsCoroutine(std::format("scripts/levels/1/{}", tooltipWidget->second));
                tooltipWidget->first->detach();
                tooltipWidget = std::nullopt;
            }
            break;
        }
    }
}

void ModeInGame::notify(uint64_t timeout, const std::string& message)
{
    auto text = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth, c::windowHeight);
    text->move(32, 32);
    text->write(message, "KellySlab", 20);
    enqueue(timeout, [text = std::move(text)]() { text->detach(); });
}

void ModeInGame::enqueue(uint64_t timeout, TimedFunction fn)
{
    timers.push({sdl::currentTime() + timeout, std::move(fn)});
}

std::optional<GameMode> ModeInGame::frame(double frameTime)
{
    const uint8_t* keys = sdl::keyboard();

    if (shouldSave and player.standing())
    {
        save("save.lua");
        shouldSave = false;
    }

    if (keys[SDL_SCANCODE_Q] or keys[SDL_SCANCODE_ESCAPE])
    {
        return GameMode::Quit;
    }

    if (keys[SDL_SCANCODE_F1])
    {
        auto path = std::format("scripts/levels/{}/map.lua", 1);
        std::ofstream file{path};
        file << world.level(1).toLua();
        SPDLOG_INFO("File {} updated", path);
    }

    if (keys[SDL_SCANCODE_F2])
    {
        SPDLOG_INFO("Activating map editor");
        ui.clear();
        ui.add(std::make_unique<ui::editor::Editor>(world.level(1), ui.fonts.get("TitilliumWeb", 14)));
    }

    player.frame(world.level(1), frameTime);

    if (not timers.empty() and timers.top().time <= sdl::currentTime())
    {
        timers.top().function();
        timers.pop();
    }

    if (subMode != nullptr)
    {
        subMode->frame();
    }
    else
    {
        if (keys[SDL_SCANCODE_DOWN])
        {
            player.move(world.level(1), Player::Direction::Backward);
        }
        if (keys[SDL_SCANCODE_UP])
        {
            player.move(world.level(1), Player::Direction::Forward);
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            player.rotate(Player::Rotation::Left);
        }
        if (keys[SDL_SCANCODE_RIGHT])
        {
            player.rotate(Player::Rotation::Right);
        }

        if (player.getPosition().x != lastFrameX or player.getPosition().y != lastFrameY or
            player.getPosition().z != lastFrameZ or player.getPosition().angle != lastFrameAngle)
        {
            auto lookX = player.getPosition().x + std::cos(player.getPosition().angle);
            auto lookY = player.getPosition().y + std::sin(player.getPosition().angle);

            if (auto script = world.level(1).checkScript(player.getPosition().sector, lookX, lookY))
            {
                if (tooltipWidget and tooltipWidget->second != *script)
                {
                    tooltipWidget->first->detach();
                }
                if (not tooltipWidget or tooltipWidget->second != *script)
                {
                    tooltipWidget = {ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth, c::windowHeight), *script};
                    auto& text    = tooltipWidget->first;
                    const static std::string interactionPrompt = "Press [Enter] to interact";
                    text->move(c::windowWidth / 2 - ui.fonts.get("KellySlab", 32).size(interactionPrompt).first / 2,
                              c::windowHeight * 3 / 4);
                    text->write("Press ", "KellySlab", 50, 185);
                    text->write("[Enter]", "KellySlab", 50, 255);
                    text->write(" to interact", "KellySlab", 50, 185);
                }
            }
            else
            {
                if (tooltipWidget)
                {
                    tooltipWidget->first->detach();
                    tooltipWidget = std::nullopt;
                }
            }

            lastFrameX     = player.getPosition().x;
            lastFrameY     = player.getPosition().y;
            lastFrameZ     = player.getPosition().z;
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
    }

    engine->frame(player.getPosition());
    engine->draw();

    return noChange;
}
} // namespace game
