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

#include <luya/display/adafruit.h>

#if defined(__IMXRT1062__)

#include <cstdint> // for uint16_t

namespace luya::display {

void Adafruit_Display::init()
{
    tft_.begin();
    tft_.setRotation(1);
    clear();
}

void Adafruit_Display::clear(uint16_t colour)
{
    tft_.fillScreen(colour);
}

} // namespace luya::display

#endif // defined(__IMXRT1062__)
