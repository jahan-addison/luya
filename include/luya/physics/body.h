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

#pragma once

#include "math.h"

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

/**
 * @brief
 * A simulated rigid box body. Call set() to configure half-extents and mass,
 * then assign position before adding to a World.
 */
class Body
{
  public:
    Body();
    void set(const Vec2& w, float m);

    void add_force(const Vec2& f) { force += f; }

    Vec2 position;
    float rotation;

    Vec2 velocity;
    float angular_velocity;

    Vec2 force;
    float torque;

    Vec2 width;

  public:
    float friction;
    float mass, inv_mass;
    float I, inv_i;
};

} // namespace physics