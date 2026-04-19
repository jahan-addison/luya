/*****************************************************************************
 * Copyright (c) Jahan Addison
 *
 * This software is dual-licensed under the Apache License, Version 2.0 or
 * the GNU General Public License, Version 3.0 or later.
 *
 * You may use this work, in part or in whole, under the terms of either
 * license.
 *
 * See the LICENSE.Apache-v2 and LICENSE.GPL-v3 files in the project root
 * for the full text of these licenses.
 ****************************************************************************/

#pragma once

#include <cstdint>                // for uint16_t
#include <luya/display/display.h> // for Display

struct SDL_Window;
struct SDL_Renderer;

namespace luya::display {

/**
 * @brief
 *   SDL2 display driver
 *
 *   Opens a desktop window at the ILI9341 logical resolution (320×240)
 *   scaled up by config::scale
 *
 */
class SDL_Display : public Display
{
  public:
    SDL_Display() = default;
    ~SDL_Display() override;

  public:
    void init() override;
    void clear(uint16_t colour = 0x0000) override;

  private:
    SDL_Window* window_{ nullptr };
    SDL_Renderer* renderer_{ nullptr };
};

} // namespace luya::display
