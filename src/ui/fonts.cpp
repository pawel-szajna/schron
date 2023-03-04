#include "fonts.hpp"

#include "util/format.hpp"

#include <spdlog/spdlog.h>

namespace ui
{
Fonts::Fonts()
{}

Fonts::~Fonts() = default;

sdl::Font& Fonts::get(const std::string& name, int size)
{
    auto fontId = std::format("{}_{}pt", name, size);
    if (not fonts.contains(fontId))
    {
        spdlog::debug("Font {} at {}pt not loaded, trying to load");
        fonts.try_emplace(fontId, name, size);
    }
    return fonts.at(fontId);
}
}
