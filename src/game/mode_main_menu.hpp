#pragma once

#include "mode_executor.hpp"

namespace scripting
{
class Scripting;
}

namespace ui
{
class UI;
}

namespace game
{
class ModeMainMenu : public ModeExecutor
{
public:

    ModeMainMenu(scripting::Scripting& scripting, ui::UI& ui);
    virtual ~ModeMainMenu() = default;

    void entry() override;
    std::optional<GameMode> frame(double frameTime) override;
    void exit() override;

private:

    scripting::Scripting& scripting;
    ui::UI& ui;
};
}
