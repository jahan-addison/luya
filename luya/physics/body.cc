/*********************************************************
 * box2d-lite - Heavily modified for ETL and Teensy 4.1
 *********************************************************/

/*
 * Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erin Catto makes no representations about the suitability
 * of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */

#include <luya/physics/body.h>

#include <float.h>             // for FLT_MAX
#include <luya/physics/math.h> // for Vec2

/****************************************************************************
 * Body
 *
 * A rigid "body" in the physics engine. Each body carries its position,
 * velocity, accumulated force, and inertial properties. Call set() with
 * box half-extents and mass before adding to a World. position may be
 * assigned directly after set().
 *
 *  Example:
 *
 *   physics::Body b{};
 *   b.set({ 0.5f, 0.5f }, 10.0f);  // 1x1 unit box, 10 kg
 *   b.position = { 0.0f, 3.0f };   // start 3 units above origin
 *   world.add(&b);
 *
 * A mass of FLT_MAX is treated as infinite, i.e. the body is static and
 * will not move under impulse. The default constructor produces a static
 * body; call set() with a finite mass to make it dynamic.
 *
 ****************************************************************************/

namespace luya::physics {

Body::Body()
{
    position.set(0.0f, 0.0f);
    rotation = 0.0f;
    velocity.set(0.0f, 0.0f);
    angular_velocity = 0.0f;
    force.set(0.0f, 0.0f);
    torque = 0.0f;
    friction = 0.2f;

    width.set(1.0f, 1.0f);
    mass = FLT_MAX;
    inv_mass = 0.0f;
    I = FLT_MAX;
    inv_i = 0.0f;
}

void Body::set(const Vec2& w, float m)
{
    position.set(0.0f, 0.0f);
    rotation = 0.0f;
    velocity.set(0.0f, 0.0f);
    angular_velocity = 0.0f;
    force.set(0.0f, 0.0f);
    torque = 0.0f;
    friction = 0.2f;

    width = w;
    mass = m;

    if (mass < FLT_MAX) {
        inv_mass = 1.0f / mass;
        I = mass * (width.x * width.x + width.y * width.y) / 12.0f;
        inv_i = 1.0f / I;
    } else {
        inv_mass = 0.0f;
        I = FLT_MAX;
        inv_i = 0.0f;
    }
}

} // namespace physics