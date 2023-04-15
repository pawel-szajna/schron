#pragma once

#include "game_mode.hpp"
#include "sdlwrapper/event.hpp"

#include <optional>

namespace game
{
class ModeExecutor
{
public:

    constexpr static auto noChange = std::nullopt;

    virtual ~ModeExecutor() = default;

    virtual void entry()                                    = 0;
    virtual void exit()                                     = 0;
    virtual std::optional<GameMode> frame(double frameTime) = 0;
    virtual void event(const sdl::event::Event& event)      = 0;
};
} // namespace game
