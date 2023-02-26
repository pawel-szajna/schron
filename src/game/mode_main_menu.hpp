#pragma once

#include "mode_executor.hpp"

namespace game
{
class ModeMainMenu : public ModeExecutor
{
public:

    virtual ~ModeMainMenu() = default;

    void entry() override {}
    std::optional<GameMode> frame(double frameTime) override { return std::nullopt; }
    void exit() override {}
};
}
