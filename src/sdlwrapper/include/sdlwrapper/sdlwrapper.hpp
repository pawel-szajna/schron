#pragma once

#include <cstdint>

namespace sdl
{
/**
 * @brief Initializes SDL.
 *
 * Calls SDL_Init, IMG_Init, and TTF_Init. Has to be called before
 * using any SDL functionality.
 */
void initialize();

/**
 * @brief Deinitializes SDL.
 *
 * Calls TTF_Quit, IMG_Quit, and SDL_Quit. Invalidates all created
 * SDL types.
 */
void teardown();

/**
 * @brief Pauses program execution.
 * @param ms Length of the pause.
 */
void delay(int ms);

/**
 * @brief Changes the mouse cursor to the default one.
 */
void resetCursor();

/**
 * @brief Returns time since application start.
 * @return Milliseconds since application start.
 */
uint64_t currentTime();

/**
 * @brief Returns time since application start.
 * @return Nanoseconds since application start.
 */
uint64_t currentTimeNs();

/**
 * @brief Returns the keyboard state.
 * @return Pointer to the C-style array with keys state.
 * @deprecated
 */
[[deprecated]] const uint8_t* keyboard();

/**
 * @brief Shows the mouse cursor.
 */
void showCursor();

/**
 * @brief Hides the mouse cursor.
 */
void hideCursor();
}
