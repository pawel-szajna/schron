#pragma once

#include "sub_mode.hpp"

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
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
} // namespace ui

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

    explicit Dialogue(Player& player, sdl::Renderer& renderer, scripting::Scripting& scripting, ui::UI& ui);
    virtual ~Dialogue();

    void event(const sdl::event::Event& event) override;
    void frame() override;

private:

    void choice(std::vector<std::array<std::string, 2>> choices);
    void text(std::string caption);
    void speech(std::string person, std::string caption);

    void choose();
    void eventChoice(const sdl::event::Key& key);
    void eventSpeech(const sdl::event::Key& key);
    void redrawChoiceWithHighlight();
    void resetTextbox();

    Player& player;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;
    ui::UI& ui;

    State state{State::Default};

    std::shared_ptr<ui::Text> widget;
    std::vector<std::array<std::string, 2>> currentChoices;

    uint64_t animationTarget{};

    size_t currentChoice{};
};
} // namespace game
