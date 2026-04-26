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

#include <luya/engine.h>

#include <luya/audio.h>           // for Audio
#include <luya/display/display.h> // for Display
#include <luya/physics/world.h>   // for World
#include <luya/renderer.h>        // for Renderer
#include <luya/storage.h>         // for Storage

namespace luya {

/**
 * @brief One-time hardware and component initialization
 */
void Engine::init()
{
    storage_.init();
    audio_.init();
    display_->init();
}

/**
 * @brief Per-frame update, drive rendering and logic each loop() tick
 */
void Engine::tick(physics::World& world)
{
    renderer_.clear();
    renderer_.draw(world);
    renderer_.render();
}

} // namespace luya
