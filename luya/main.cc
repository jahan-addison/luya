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

#include <luya/engine.h> // for Engine

/*****************************************************
 *   _   _   _ __   __ _
 *  | | | | | |\ \ / // \
 *  | | | | | | \ V // _ \
 *  | |_| |_| |  | |/ ___ \
 *  |___|\___/   |_/_/   \_\
 *
 *        2D Gaming Engine
 *          on Teensy 4.1
 *****************************************************/

#if defined(LUYA_USE_SDL2)

#include <SDL2/SDL.h> // for SDL_PollEvent, SDL_QUIT

int main()
{
    luya::Engine engine;
    engine.init();

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        engine.tick();
    }

    return 0;
}

#else

// Teensy entry point
static luya::Engine engine;

void setup()
{
    engine.init();
}

void loop()
{
    engine.tick();
}

#endif
