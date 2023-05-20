#include "ui_bindings.hpp"

#include "sdlwrapper/sdlwrapper.hpp"
#include "ui/text.hpp"
#include "ui/ui.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace scripting
{
UiBindings::UiBindings(sol::state& lua, ui::UI& ui, sdl::Renderer& renderer)
    : lua(lua)
    , ui(ui)
    , renderer(renderer)
{
}
} // namespace scripting
