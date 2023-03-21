#include "dialogue.hpp"

#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/renderer.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"
#include "util/format.hpp"

#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>
#include <variant>

namespace game
{
Dialogue::Dialogue(sdl::Renderer& renderer,
                   scripting::Scripting& scripting,
                   ui::UI& ui) :
    renderer(renderer),
    scripting(scripting),
    ui(ui)
{
    spdlog::debug("Entering dialogue mode, binding LUA functions");

    scripting.bind("choice", &Dialogue::choice, this);
    scripting.bind("text", &Dialogue::text, this);
    scripting.bind("speech", &Dialogue::speech, this);
}

Dialogue::~Dialogue()
{
    if (widget)
    {
        ui.remove(*widget);
    }

    scripting.unbind("choice");
    scripting.unbind("text");
    scripting.unbind("speech");

    spdlog::debug("Dialogue mode finished");
}

// NOLINTNEXTLINE(performance-unnecessary-value-param) Temporary value coming from LUA
void Dialogue::choice(std::string caption, std::vector<std::string> choices)
{
    state = State::Choice;
    if (widget)
    {
        ui.remove(*widget);
    }
    widget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 32, c::windowHeight - 64, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
    text.move(32, c::windowHeight - 64);
    text.write(std::move(caption), "KellySlab", 80);
    choicesCount = 0;
    for (auto& c : choices)
    {
        ++choicesCount;
        text.write(std::format("\n{}. ", choicesCount), "RubikDirt", 20);
        text.write(c, "KellySlab", 90);
    }
}

void Dialogue::text(std::string caption)
{
    state = State::Speech;
    if (widget)
    {
        ui.remove(*widget);
    }
    widget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 64, c::windowHeight - 32, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
    text.move(32, c::windowHeight - 64);
    text.write(std::move(caption), "KellySlab", 64);
}

void Dialogue::speech(std::string person, std::string caption)
{
    state = State::Speech;
    if (widget)
    {
        ui.remove(*widget);
    }
    widget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 64, c::windowHeight - 32, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
    text.move(32, c::windowHeight - 64);
    text.write(std::format("{}: ", std::move(person)), "RubikDirt", 48);
    text.write(std::move(caption), "KellySlab", 64);
}

void Dialogue::event(const sdl::event::Event& event)
{
    if (std::holds_alternative<sdl::event::Key>(event))
    {
        const auto& key = get<sdl::event::Key>(event);

        if (key.direction != sdl::event::Key::Direction::Down)
        {
            return;
        }

        switch (key.scancode)
        {
        case SDL_SCANCODE_RETURN:
            switch (state)
            {
            case State::Default:
                break;
            case State::Choice:
                scripting.set("result", currentChoice);
                scripting.resume();
                return;
            case State::Speech:
                auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
                if (not text.done())
                {
                    text.finish();
                    return;
                }
                scripting.resume();
                return;
            }

        case SDL_SCANCODE_1:
        case SDL_SCANCODE_2:
        case SDL_SCANCODE_3:
        case SDL_SCANCODE_4:
        case SDL_SCANCODE_5:
        case SDL_SCANCODE_6:
        case SDL_SCANCODE_7:
        case SDL_SCANCODE_8:
        case SDL_SCANCODE_9:
            if (state == State::Choice)
            {
                int value = key.scancode - SDL_SCANCODE_0;
                if (value > 0 and value < choicesCount)
                {
                    scripting.set("result", value);
                    scripting.resume();
                }
            }
            return;
        }
    }
}

void Dialogue::frame()
{
    if (state == State::Default)
    {
        scripting.resume();
    }
}
}