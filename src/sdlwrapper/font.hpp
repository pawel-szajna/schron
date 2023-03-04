#pragma once

#include "wrapped.hpp"

struct _TTF_Font;

namespace sdl
{
class Font : public Wrapped<_TTF_Font>
{
public:

    Font(const std::string& file, int size);
    ~Font();
};
}
