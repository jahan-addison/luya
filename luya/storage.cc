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

#include <luya/storage.h>

namespace luya {

/**
 * @brief Mount and initialise the SD card (no-op on host, returns true)
 */
bool Storage::init()
{
#if defined(__IMXRT1062__)
    return sd_.begin(SdioConfig(FIFO_SDIO));
#else
    return true;
#endif
}

} // namespace luya
