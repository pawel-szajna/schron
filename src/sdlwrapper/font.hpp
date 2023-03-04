#pragma once

#include "wrapped.hpp"

#include <string>

struct _TTF_Font;

namespace sdl
{
struct Color;
class Surface;

class Font : public Wrapped<_TTF_Font>
{
public:

    Font(const std::string& file, int size);
    ~Font();

    Surface render(const std::string& text, Color color);
    Surface renderOutlined(const std::string& text, Color color, Color outline, int size = 1);
    void render(Surface& target, const std::string& text);

    std::pair<int, int> size(const std::string& text);
};
}
