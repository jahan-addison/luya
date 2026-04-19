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

#include <luya/audio.h>

namespace luya {

/**
 * @brief Allocate audio buffers and start the codec (no-op on host)
 */
void Audio::init()
{
#if defined(__IMXRT1062__)
    AudioMemory(kAudio_Memory_Blocks);
    codec_.enable();
    codec_.volume(0.5f);
#endif
}

} // namespace luya
