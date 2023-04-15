#pragma once

#include "wrapped.hpp"

struct SDL_Cursor;

namespace sdl
{
/**
 * @class Cursor
 * @brief An object representing a mouse cursor.
 */
class Cursor : public Wrapped<SDL_Cursor>
{
public:

    /**
     * @brief Constructs the cursor using a system cursor ID.
     * @param id Numeric value of SDL_SystemCursor enum.
     *
     * This function calls the SDL_CreateSystemCursor to obtain the cursor.
     */
    explicit Cursor(int id);
    ~Cursor();

    /**
     * @brief Marks the cursor as active.
     */
    void activate() const;
};
} // namespace sdl
