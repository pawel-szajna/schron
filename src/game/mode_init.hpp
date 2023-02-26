#pragma once

#include "mode_executor.hpp"

namespace game
{
class DummyMode : public ModeExecutor
{
public:

    virtual ~DummyMode() = default;

    void entry() override {}
    std::optional<GameMode> frame(double frameTime) override { return std::nullopt; }
    void exit() override {}
};
}
