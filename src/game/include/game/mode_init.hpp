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

    void event(const sdl::event::Event& event) override {}

    void exit() override {}
};
} // namespace game
