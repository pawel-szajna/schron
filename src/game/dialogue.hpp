#pragma once

#include "sub_mode.hpp"

#include <optional>
#include <string>
#include <vector>

namespace scripting
{
class Scripting;
}

namespace sdl
{
class Renderer;
}

namespace ui
{
class Text;
class UI;
}

namespace game
{
class Player;

class Dialogue : public SubMode
{
    enum class State
    {
        Default,
        Choice,
        Speech,
    };

public:

    explicit Dialogue(Player& player,
                      sdl::Renderer& renderer,
                      scripting::Scripting& scripting,
                      ui::UI& ui);
    virtual ~Dialogue();

    void event(const sdl::event::Event& event) override;
    void frame() override;

private:

    void choice(std::vector<std::string> choices);
    void text(std::string caption);
    void speech(std::string person, std::string caption);

    void eventChoice(const sdl::event::Key& key);
    void eventSpeech(const sdl::event::Key& key);
    void redrawChoiceWithHighlight();
    ui::Text& resetTextbox();

    Player& player;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;
    ui::UI& ui;

    State state{State::Default};

    std::optional<int> widget;
    std::vector<std::string> currentChoices;

    uint64_t animationTarget{};

    int choicesCount{};
    int currentChoice{};
};
}