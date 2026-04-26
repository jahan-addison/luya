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
 * Joint
 *
 * A position-level distance constraint between two bodies. Call set() with
 * the two bodies and a world-space anchor point; the joint maintains the
 * initial anchor separation each step via an impulse-based correction.
 *
 *  Example:
 *
 *   physics::Joint hinge;
 *   hinge.set(&body_a, &body_b, { 0.0f, 1.0f });
 *   world.add(&hinge);
 *
 * bias_factor controls position correction strength (default 0.2).
 * softness adds constraint compliance, i.e. a spring-like feel (default 0.0).
 *
 ****************************************************************************/

namespace luya::physics {

class Body;

/**
 * @brief
 * Impulse-based distance constraint between two bodies at a fixed anchor.
 */
class Joint
{
  public:
    Joint()
        : P(0.0f, 0.0f)
        , body1(0)
        , body2(0)
        , bias_factor(0.2f)
        , softness(0.0f)
    {
    }

  public:
    void set(Body* body1, Body* body2, const Vec2& anchor);

    void pre_step(float inv_dt);
    void apply_impulse();

  public:
    Mat22 M;
    Vec2 local_anchor1, local_anchor2;
    Vec2 r1, r2;
    Vec2 bias;
    Vec2 P; // accumulated impulse
    Body* body1;
    Body* body2;
    float bias_factor;
    float softness;
};

} // namespace physics