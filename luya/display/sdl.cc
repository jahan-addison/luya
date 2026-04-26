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

#include <SDL2/SDL.h>             // for SDL_Init, SDL_Quit, SDL_INIT_VIDEO
#include <SDL_render.h>           // for SDL_CreateRenderer, SDL_DestroyRen...
#include <SDL_video.h>            // for SDL_CreateWindow, SDL_DestroyWindow
#include <cstdint>                // for uint8_t, uint16_t
#include <luya/display/display.h> // for height, scale, width

/****************************************************************************
 * SDL_Display
 *
 * Desktop display driver. Opens a 320x240 SDL2 window scaled up by
 * config::scale, creates a streaming RGB565 texture, and presents via
 * SDL_RenderCopy each frame.
 *
 ****************************************************************************/

namespace luya::display {

/**
 * @brief Destroy SDL texture, renderer, and window; call SDL_Quit
 */
SDL_Display::~SDL_Display()
{
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

/**
 * @brief Create window, renderer, and streaming RGB565 texture; clear to black
 */
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
    texture_ = SDL_CreateTexture(renderer_,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        config::width,
        config::height);
    clear();
}

/**
 * @brief Upload framebuffer to the SDL texture and present the frame
 */
void SDL_Display::blit(frame_buffer_t const* framebuffer,
    [[maybe_unused]] int len)
{
    SDL_UpdateTexture(texture_,
        nullptr,
        framebuffer,
        config::width * static_cast<int>(sizeof(uint16_t)));
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

/**
 * @brief Expand RGB565 color to RGB888 and clear the SDL renderer
 */
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
