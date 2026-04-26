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
#include <SdFat.h> // for SdFs, SdioConfig, FIFO_SDIO, BUILTIN_SDCARD
#endif

namespace luya {

/**
 * @brief
 *   SdFat storage component
 *
 *   SdFs wrapper for access to the Teensy 4.1 built-in SDIO SD card slot.
 *   Uses FIFO_SDIO mode for maximum throughput.
 *
 *   In SDL2 builds this is a no-op so the engine
 *   compiles and runs without Teensy hardware.
 */
class Storage
{
  public:
    Storage() = default;
    Storage(Storage const&) = delete;
    Storage& operator=(Storage const&) = delete;

  public:
    bool init();

  private:
#if defined(__IMXRT1062__)
    // Teensy 4.1 has a dedicated built-in SD card slot; no chip-select pin
    // is required — BUILTIN_SDCARD maps to the SDIO peripheral.
    SdFs sd_;
#endif
};

} // namespace luya
