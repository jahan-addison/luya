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

namespace luya::physics {

class Body;

class Joint
{
  public:
    Joint()
        : P(0.0f, 0.0f)
        , body1(0)
        , body2(0)
        , biasFactor(0.2f)
        , softness(0.0f)
    {
    }

  public:
    void Set(Body* body1, Body* body2, const Vec2& anchor);

    void PreStep(float inv_dt);
    void ApplyImpulse();

  public:
    Mat22 M;
    Vec2 localAnchor1, localAnchor2;
    Vec2 r1, r2;
    Vec2 bias;
    Vec2 P; // accumulated impulse
    Body* body1;
    Body* body2;
    float biasFactor;
    float softness;
};

} // namespace physics