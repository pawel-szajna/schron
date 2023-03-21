#include "dialogue.hpp"

#include "player.hpp"
#include "scripting/scripting.hpp"
#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"
#include "util/format.hpp"

#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>
#include <variant>

namespace game
{
Dialogue::Dialogue(Player& player,
                   sdl::Renderer& renderer,
                   scripting::Scripting& scripting,
                   ui::UI& ui) :
    player(player),
    renderer(renderer),
    scripting(scripting),
    ui(ui)
{
    spdlog::debug("Entering dialogue mode, binding LUA functions");

    scripting.bind("choice", &Dialogue::choice, this);
    scripting.bind("text", &Dialogue::text, this);
    scripting.bind("speech", &Dialogue::speech, this);

    animationTarget = sdl::currentTime() + 250;
    player.animateFov(c::renderHeight * 0.65 * 1.9, c::renderWidth * 0.22 * 1.9, animationTarget);
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

    player.animateFov(c::renderHeight * 0.65, c::renderWidth * 0.22, sdl::currentTime() + 250);

    spdlog::debug("Dialogue mode finished");
}

ui::Text& Dialogue::resetTextbox()
{
    if (widget)
    {
        ui.remove(*widget);
    }
    widget = ui.add<ui::Text>(renderer, ui.fonts, c::windowWidth - 68, c::windowHeight - 64, 32, 0);
    auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
    text.move(32, c::windowHeight - 64);
    return text;
}

void Dialogue::redrawChoiceWithHighlight()
{
    auto& text = resetTextbox();
    int counter = 0;
    for (auto& c : currentChoices)
    {
        ++counter;
        text.write(std::format("\n{}. ", counter), "RubikDirt", -1, counter == currentChoice ? 255 : 88);
        text.write(c, "KellySlab", -1, counter == currentChoice ? 255 : 120);
    }
}

void Dialogue::choice(std::vector<std::string> choices)
{
    state = State::Choice;
    auto& text = resetTextbox();
    choicesCount = 0;
    currentChoice = 0;
    for (auto& c : choices)
    {
        ++choicesCount;
        text.write(std::format("\n{}. ", choicesCount), "RubikDirt", 40);
        text.write(c, "KellySlab", 130);
    }
    currentChoices = std::move(choices);
}

void Dialogue::text(std::string caption)
{
    state = State::Speech;
    auto& text = resetTextbox();
    text.write(std::move(caption), "KellySlab", 64);
}

void Dialogue::speech(std::string person, std::string caption)
{
    state = State::Speech;
    auto& text = resetTextbox();
    text.write(std::format("{}: ", std::move(person)), "RubikDirt", 48);
    text.write(std::move(caption), "KellySlab", 64);
}

void Dialogue::eventChoice(const sdl::event::Key& key)
{
    switch (key.scancode)
    {
    case SDL_SCANCODE_RETURN:
        if (currentChoice > 0 and currentChoice <= choicesCount)
        {
            scripting.set("result", currentChoice);
            scripting.resume();
        }
        return;

    case SDL_SCANCODE_UP:
        currentChoice--;
        if (currentChoice < 1)
        {
            currentChoice = choicesCount;
        }
        redrawChoiceWithHighlight();
        return;

    case SDL_SCANCODE_DOWN:
        currentChoice++;
        if (currentChoice > choicesCount)
        {
            currentChoice = 1;
        }
        redrawChoiceWithHighlight();
        return;

    case SDL_SCANCODE_1:
    case SDL_SCANCODE_2:
    case SDL_SCANCODE_3:
    case SDL_SCANCODE_4:
    case SDL_SCANCODE_5:
    case SDL_SCANCODE_6:
    case SDL_SCANCODE_7:
    case SDL_SCANCODE_8:
    case SDL_SCANCODE_9:
        int value = key.scancode - SDL_SCANCODE_1 + 1;
        if (value <= choicesCount)
        {
            scripting.set("result", value);
            scripting.resume();
        }
        return;
    }
}

void Dialogue::eventSpeech(const sdl::event::Key& key)
{
    switch (key.scancode)
    {
    case SDL_SCANCODE_RETURN:
        auto& text = dynamic_cast<ui::Text&>(ui.get(*widget));
        if (not text.done())
        {
            text.finish();
            return;
        }
        scripting.resume();
        return;
    }
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

        switch (state)
        {
        case State::Default:
            return;
        case State::Speech:
            eventSpeech(key);
            return;
        case State::Choice:
            eventChoice(key);
            return;
        }
    }
}

void Dialogue::frame()
{
    if (state == State::Default and sdl::currentTime() >= animationTarget)
    {
        scripting.resume();
    }
}
}