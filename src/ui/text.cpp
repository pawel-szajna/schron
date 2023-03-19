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

void Text::write(std::string text, std::string font, int charsPerSecond, bool flashingLetters)
{
    requests.push({std::move(text), std::move(font), 200, charsPerSecond});
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
                std::transform(current->flashes.begin(), current->flashes.end(), current->flashes.begin(),
                               [h = h](auto& flash) { flash.y -= h; return flash; });
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

        auto substring = text.substr(current->position, lettersToAppend);
        auto [w, _] = font.size(substring);
        current->flashes.push_back(Flash{ substring, now + current->request.flashSpeed, current->x, current->y });
        current->position += lettersToAppend;
        current->lastLetterAdded = now;
        current->x += w;
    }

    auto renderFlash = [&font, now, flashSpeed = current->request.flashSpeed](const Flash& flash)
    {
        auto intensity = static_cast<uint8_t>(112 * ((now - flash.finishTime + flashSpeed) / flashSpeed));
        uint8_t bright = std::clamp(128 + intensity, 0, 255);
        uint8_t dark = std::clamp(128 - intensity, 0, 255);
        uint8_t alpha = std::clamp(2 * intensity, 1, 255);
        return font.renderOutlined(flash.part,
                                   sdl::Color{bright, bright, bright, alpha},
                                   sdl::Color{dark, dark, dark, alpha});
    };

    while (not current->flashes.empty() and
           current->flashes.front().finishTime < now)
    {
        auto flash = std::move(current->flashes.front());
        auto finishedText = renderFlash(flash);
        finishedText.render(current->flashed, sdl::Rectangle{flash.x, flash.y, 0, 0});
        current->flashes.pop_front();
    }

    buffer.empty();
    current->flashed.render(buffer);

    for (const auto& flash : current->flashes)
    {
        auto flashingText = renderFlash(flash);
        flashingText.render(buffer, sdl::Rectangle{flash.x, flash.y, 0, 0});
    }

    if (current->position >= static_cast<int>(current->request.text.size()) and current->flashes.empty())
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
