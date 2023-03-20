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
class UI;
}

namespace game
{
class Dialogue : public SubMode
{
    enum class State
    {
        Default,
        Choice,
        Speech,
    };

public:

    explicit Dialogue(sdl::Renderer& renderer,
                      scripting::Scripting& scripting,
                      ui::UI& ui);
    virtual ~Dialogue();

    void event(const sdl::event::Event& event) override;
    void frame() override;

private:

    void choice(std::string caption, std::vector<std::string> choices);
    void text(std::string caption);
    void speech(std::string person, std::string caption);

    sdl::Renderer& renderer;
    scripting::Scripting& scripting;
    ui::UI& ui;

    State state{State::Default};

    std::optional<int> widget;

    int choicesCount;
    int currentChoice{1};
};
}