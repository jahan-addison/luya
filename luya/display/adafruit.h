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

#if defined(__IMXRT1062__)

#include <cstdint>                // for uint16_t, uint8_t
#include <luya/display/display.h> // for Display

#if defined(LUYA_USE_ILI9341_T3N)
#include <ILI9341_t3n.h> // for ILI9341_t3n
#else
#include <ILI9341_t3.h> // for ILI9341_t3
#endif

namespace luya::display {

/**
 * @brief
 *   Adafruit 2.8" TFT (ILI9341) pin assignments for Teensy 4.1 SPI0 bus
 */
namespace pins {
inline constexpr uint8_t TFT_CS = 10;
inline constexpr uint8_t TFT_DC = 9;
inline constexpr uint8_t TFT_RST = 8;
inline constexpr uint8_t TFT_MOSI = 11;
inline constexpr uint8_t TFT_SCK = 13;
inline constexpr uint8_t TFT_MISO = 12;
} // namespace pins

/**
 * @brief
 *   Adafruit 2.8" ILI9341 TFT driver
 *
 *   ILI9341_t3 or, when LUYA_USE_ILI9341_T3N is defined, the
 *   DMA-capable ILI9341_t3n variant.
 */
class Adafruit_Display : public Display
{
  public:
    Adafruit_Display::Adafruit_Display()
        : tft_(pins::TFT_CS,
              pins::TFT_DC,
              pins::TFT_RST,
              pins::TFT_MOSI,
              pins::TFT_SCK,
              pins::TFT_MISO)
    {
    }

  public:
    void init() override;
    void clear(uint16_t color = 0x0000) override;

  private:
#if defined(LUYA_USE_ILI9341_T3N)
    ILI9341_t3n tft_;
#else
    ILI9341_t3 tft_;
#endif
};

} // namespace luya::display

#endif // defined(__IMXRT1062__)
