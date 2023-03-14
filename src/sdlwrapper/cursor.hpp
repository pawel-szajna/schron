#pragma once

#include "wrapped.hpp"

struct SDL_Cursor;

namespace sdl
{
class Cursor : public Wrapped<SDL_Cursor>
{
public:

    explicit Cursor(int id);
    ~Cursor();

    void activate() const;
};
}
