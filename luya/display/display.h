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

#include <cstdint> // for uint16_t
#include <memory>  // for unique_ptr

namespace luya::display {

/**
 * @brief
 *   ILI9341 native display resolution and SDL2 desktop scale factor
 */
namespace config {
inline constexpr int width = 320;
inline constexpr int height = 240;
inline constexpr int scale = 3; // SDL2 window scale (960x720)
} // namespace config

/**
 * @brief
 *   Abstract display driver interface
 */
class Display
{
  protected:
    Display() = default;

  public:
    virtual ~Display() = default;
    Display(Display const&) = delete;
    Display& operator=(Display const&) = delete;

  public:
    /**
     * @brief Initialise and configure the display hardware or window
     */
    virtual void init() = 0;

    /**
     * @brief Fill the entire screen with a 16-bit RGB565 colour
     */
    virtual void clear(uint16_t colour = 0x0000) = 0;
};

[[nodiscard]] std::unique_ptr<Display> make();

} // namespace luya::display
