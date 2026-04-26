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
#include <Audio.h> // for AudioMemory, AudioOutputI2S, AudioControlSGTL5000
#include <SPI.h>   // for SPI (required by Teensy audio shield)
#include <Wire.h>  // for Wire (required by Teensy audio shield i2c)
#endif

namespace luya {

/**
 * @brief
 * Teensy audio shield I2S output via SGTL5000. No-op in SDL2 builds.
 */
class Audio
{
  public:
    Audio() = default;
    Audio(Audio const&) = delete;
    Audio& operator=(Audio const&) = delete;

  public:
    void init();

  private:
#if defined(__IMXRT1062__)
    static constexpr int k_audio_memory_blocks = 8;

    AudioOutputI2S out_;
    AudioControlSGTL5000 codec_;
#endif
};

} // namespace luya
