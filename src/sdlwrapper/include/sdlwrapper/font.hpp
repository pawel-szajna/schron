#pragma once

#include "wrapped.hpp"

#include <string>

struct _TTF_Font; // NOLINT(bugprone-reserved-identifier): no choice due to SDL_ttf design

namespace sdl
{
struct Color;
class Surface;

/**
 * @class Font
 * @brief An object representing a font.
 *
 * This object wraps a SDL_ttf TTF_Font structure, which represents a font face with given size.
 * A different sdl::Font is required for every used font size.
 */
class Font : public Wrapped<_TTF_Font>
{
public:

    /**
     * @brief Constructs a wrapped TTF_Font.
     * @param file TTF font file path.
     * @param size Font size.
     */
    Font(const std::string& file, int size);
    ~Font();

    /**
     * @brief Renders the requested text to a new surface.
     * @param text Text to render.
     * @param color Text face color.
     * @return sdl::Surface with rendered text.
     */
    Surface render(const std::string& text, Color color);

    /**
     * @brief Renders the requested text, with an outline, to a new surface.
     * @param text Text to render.
     * @param color Text face color.
     * @param outline Text outline color.
     * @param size unused // TODO check
     * @return sdl::Surface with rendered text.
     */
    Surface renderOutlined(const std::string& text, Color color, Color outline, int size = 1);

    /**
     * @brief Renders the text to a target surface.
     * @param target Target sdl::Surface.
     * @param text Text to render.
     */
    void render(Surface& target, const std::string& text);

    /**
     * @brief Returns size, in pixels, of the text.
     * @param text Text contents.
     * @return A pair of values [width, height].
     */
    std::pair<int, int> size(const std::string& text);
};
} // namespace sdl
