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

#include <luya/display/sdl.h>

#include <SDL2/SDL.h> // for SDL_Init, SDL_CreateWindow, SDL_CreateRenderer
#include <cstdint>    // for uint8_t, uint16_t

namespace luya::display {

SDL_Display::~SDL_Display()
{
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void SDL_Display::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow("Luya \xe2\x80\x94 SDL2 Display",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config::width * config::scale,
        config::height * config::scale,
        SDL_WINDOW_SHOWN);
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    // SDL handles upscaling
    SDL_RenderSetLogicalSize(renderer_, config::width, config::height);
    clear();
}

void SDL_Display::clear(uint16_t color)
{
    // Expand RGB565 to RGB888
    uint8_t const r = static_cast<uint8_t>(((color >> 11) & 0x1F) << 3);
    uint8_t const g = static_cast<uint8_t>(((color >> 5) & 0x3F) << 2);
    uint8_t const b = static_cast<uint8_t>((color & 0x1F) << 3);
    SDL_SetRenderDrawColor(renderer_, r, g, b, 0xFF);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

} // namespace luya::display
