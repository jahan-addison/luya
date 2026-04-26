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

#include <luya/renderer.h>

#include <cstring>              // for memset
#include <luya/physics/body.h>  // for Body
#include <luya/physics/math.h>  // for Vec2
#include <luya/physics/world.h> // for World

/****************************************************************************
 * Renderer
 *
 * Maintains a full-screen RGB565 framebuffer. Each frame:
 *   1. clear()  — fill the framebuffer with a background color
 *   2. draw()   — rasterize active physics bodies and queued sprites
 *   3. render() — blit the finished framebuffer to the display driver
 *
 *  Example:
 *
 *   luya::Engine engine{};
 *   engine.init();
 *
 *   auto& renderer = engine.renderer();
 *   renderer.clear();
 *   renderer.draw(world);
 *   renderer.render();
 *
 ****************************************************************************/

namespace luya {

/**
 * @brief Fill the framebuffer with a solid RGB565 color
 */
void Renderer::clear(uint16_t color)
{
    if (color == 0x0000) {
        framebuffer_.fill(0);
        return;
    }
    framebuffer_.fill(color);
}

/**
 * @brief Queue a sprite to be drawn at (x, y) on the next render pass
 */
void Renderer::add_sprite(Sprite const* sprite, int16_t x, int16_t y)
{
    if (sprite && !sprite_queue_.full()) {
        sprite_queue_.push_back({ sprite, x, y });
    }
}

/**
 * @brief Rasterize all active bodies in the world and queued sprites
 *        into the framebuffer, then drain the sprite queue
 */
void Renderer::draw(physics::World const& world)
{
    for (const auto* body : world.bodies) {
        draw_body(body);
    }
    for (const auto& entry : sprite_queue_) {
        draw_sprite(entry.sprite, entry.x, entry.y);
    }
    sprite_queue_.clear();
}

/**
 * @brief Blit the framebuffer to the display driver
 */
void Renderer::render()
{
    display_.blit(&framebuffer_, k_width * k_height);
}

/**
 * @brief Project a world-space position to screen pixel coordinates
 */
Renderer::Screen Renderer::world_to_screen(float wx, float wy) const
{
    return { static_cast<int16_t>(
                 static_cast<int>(wx * k_pixels_per_unit) + k_center_x),
        static_cast<int16_t>(
            k_center_y - static_cast<int>(wy * k_pixels_per_unit)) };
}

/**
 * @brief Rasterize a body's axis-aligned bounding box as a filled white
 * rectangle
 */
void Renderer::draw_body(physics::Body const* body)
{
    // Y is negated so that positive physics-Y maps upward on screen
    int const cx =
        static_cast<int>(body->position.x * k_pixels_per_unit) + k_center_x;
    int const cy =
        k_center_y - static_cast<int>(body->position.y * k_pixels_per_unit);

    // body->width holds the box half-extents
    int const hw = static_cast<int>(body->width.x * k_pixels_per_unit);
    int const hh = static_cast<int>(body->width.y * k_pixels_per_unit);

    fill_rect(cx - hw, cy - hh, 2 * hw, 2 * hh, 0xFFFF);
}

/**
 * @brief Blit a sprite's pixel data into the framebuffer at (sx, sy), clipping
 * to screen bounds
 */
void Renderer::draw_sprite(Sprite const* sprite, int16_t sx, int16_t sy)
{
    for (uint16_t row = 0; row < sprite->height; ++row) {
        int const dy = sy + static_cast<int>(row);
        if (dy < 0 || dy >= k_height)
            continue;

        for (uint16_t col = 0; col < sprite->width; ++col) {
            int const dx = sx + static_cast<int>(col);
            if (dx < 0 || dx >= k_width)
                continue;

            framebuffer_[dy * k_width + dx] =
                sprite->data[row * sprite->width + col];
        }
    }
}

/**
 * @brief Fill an axis-aligned rectangle in the framebuffer, clipping to screen
 * bounds
 */
void Renderer::fill_rect(int x, int y, int w, int h, uint16_t color)
{
    for (int row = y; row < y + h; ++row) {
        if (row < 0 || row >= k_height)
            continue;
        for (int col = x; col < x + w; ++col) {
            if (col < 0 || col >= k_width)
                continue;
            framebuffer_[row * k_width + col] = color;
        }
    }
}

} // namespace luya
