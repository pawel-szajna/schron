#include "text.hpp"

#include "fonts.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"

#include <algorithm>
#include <spdlog/spdlog.h>

namespace ui
{
Text::Text(sdl::Renderer& renderer, Fonts& fonts, int width, int height, int x, int y) :
    width(width), height(height),
    x(x), y(y), lastX(x), lastY(y),
    buffer(c::windowWidth, c::windowHeight),
    texture(renderer.createTexture(sdl::Texture::Access::Streaming, c::windowWidth, c::windowHeight)),
    fonts(fonts)
{
    texture.setBlendMode(sdl::BlendMode::Blend);
}

Text::~Text() = default;

void Text::render(sdl::Renderer& target)
{
    target.copy(texture);

    if (current != std::nullopt)
    {
        advance();
        return;
    }

    if (not requests.empty())
    {
        current.emplace(std::move(requests.front()), lastX, lastY);
        current->lastLetterAdded = sdl::currentTime();
        buffer.render(current->flashed);
        requests.pop();
    }
}

void Text::event(const sdl::event::Event& event)
{}

void Text::clear()
{
    while (not requests.empty())
    {
        requests.pop();
    }
    current = std::nullopt;
    buffer.empty();
}

void Text::write(std::string text, std::string font, int charsPerSecond)
{
    requests.push({std::move(text), std::move(font), charsPerSecond, 255});
}

bool Text::done() const
{
    return requests.empty() and not current;
}

void Text::finish()
{
    if (not current)
    {
        return;
    }

    current->request.charsPerSecond = -1;
}

void Text::advance()
{
    if (not current.has_value())
    {
        spdlog::warn("Tried to advance a text with no current operation");
        return;
    }

    auto now = sdl::currentTime();
    auto timePassed = now - current->lastLetterAdded;
    auto lettersToAppend = static_cast<std::string::size_type>(timePassed * current->request.charsPerSecond / 1000);
    const auto& text = current->request.text;

    lettersToAppend = std::min(lettersToAppend, text.size() - current->position);

    if (current->request.charsPerSecond < 0)
    {
        lettersToAppend = text.size() - current->position;
    }

    auto& font = fonts.get(current->request.font, 32);

    if (lettersToAppend > 0)
    {
        if (current->position + static_cast<int>(lettersToAppend) > current->verified)
        {
            auto nextSpace = text.find(' ', current->position);
            auto nextBreak = text.find('\n', current->position);
            if (nextBreak < nextSpace) nextSpace = nextBreak;

            std::string nextWord = text.substr(current->position, nextSpace - current->position);
            auto [w, h] = font.size(nextWord);
            if (current->x + w > x + width or text[current->position] == '\n')
            {
                current->x = x;
                current->y += h;
            }

            if (current->y + h > y + height)
            {
                current->y -= h;
                sdl::Surface tempBuffer{c::windowWidth, c::windowHeight};
                current->flashed.render(tempBuffer, sdl::Rectangle{0, -(h + y + 2), 0, 0});
                current->flashed.empty();
                tempBuffer.render(current->flashed, sdl::Rectangle{0, (y + 2), 0, 0});
            }

            current->verified = (nextSpace == std::string::npos ? text.size() : nextSpace) + 1;
        }

        // UTF-8 multibyte character should not be cut in half
        while ((text[current->position + lettersToAppend] & 0xc0) == 0x80)
        {
            ++lettersToAppend;
        }

        while (lettersToAppend > 0 and current->position < text.size() and text[current->position] == '\n')
        {
            current->position++;
            lettersToAppend--;
        }

        if (lettersToAppend > 0)
        {
            auto substring = text.substr(current->position, lettersToAppend);
            auto [w, _] = font.size(substring);
            current->position += lettersToAppend;
            current->lastLetterAdded = now;
            auto rendered = font.renderOutlined(substring,
                                                sdl::Color{current->request.color, current->request.color, current->request.color, 255},
                                                sdl::Color{64, 64, 64, 255});
            rendered.render(current->flashed, sdl::Rectangle{current->x, current->y, 0, 0});
            current->x += w;
        }
    }

    buffer.empty();
    current->flashed.render(buffer);

    if (current->position >= static_cast<int>(current->request.text.size()))
    {
        lastX = current->x;
        lastY = current->y;
        current = std::nullopt;
    }

    buffer.render(texture);
}

void Text::move(int x, int y)
{
    lastX = x;
    lastY = y;
}
}
