#include "fonts.hpp"

#include "util/format.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
Fonts::Fonts() = default;

Fonts::~Fonts() = default;

sdl::Font& Fonts::get(const std::string& name, int size)
{
    auto fontId = std::format("{}_{}pt", name, size);
    if (not fonts.contains(fontId))
    {
        spdlog::debug("Font {} at {}pt not loaded, trying to load", name, size);
        fonts.try_emplace(fontId, std::format("res/font/{}.ttf", name), size);
    }
    return fonts.at(fontId);
}
} // namespace ui
