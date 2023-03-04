#pragma once

#include "sdlwrapper/font.hpp"

#include <map>
#include <string>

namespace ui
{
class Fonts
{
public:

    Fonts();
    ~Fonts();

    sdl::Font& get(const std::string& name, int size);

private:

    std::map<std::string, sdl::Font> fonts;

};
}