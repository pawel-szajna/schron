#include "font.hpp"

#include "common_types.hpp"
#include "surface.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>

namespace sdl
{
Font::Font(const std::string& file, int size)
{
    assign(TTF_OpenFont(file.c_str(), size), std::format("SDL TTF font ({})", TTF_GetError()));
}

Font::~Font()
{
    if (wrapped != nullptr)
    {
        TTF_CloseFont(wrapped);
    }
}

Surface Font::render(const std::string& text, Color color)
{
    return Surface(TTF_RenderUTF8_Blended(wrapped, text.c_str(), SDL_Color{color.r, color.g, color.b, color.a}));
}

Surface Font::renderOutlined(const std::string& text, Color color, Color outline, int size)
{
    auto textFg = render(text, color);
    auto textBg = render(text, outline);

    auto target = Surface(textFg->w + 2, textFg->h + 2);

    textBg.render(target, Rectangle{0, 0, 0, 0});
    textBg.render(target, Rectangle{2, 0, 0, 0});
    textBg.render(target, Rectangle{0, 2, 0, 0});
    textBg.render(target, Rectangle{2, 2, 0, 0});
    textFg.render(target, Rectangle{1, 1, 0, 0});

    return target;
}

void Font::render(Surface& target, const std::string& text)
{
    auto surface = render(text, Color{255, 255, 255, 255});
    surface.render(target);
}

std::pair<int, int> Font::size(const std::string& text)
{
    int w, h;
    TTF_SizeUTF8(wrapped, text.c_str(), &w, &h);
    return {w, h};
}
} // namespace sdl
