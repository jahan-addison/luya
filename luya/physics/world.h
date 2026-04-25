/*********************************************************
 * box2d-lite - Heavily modified for ETL and Teensy 4.1
 *********************************************************/

/*
 * Copyright (c) 2006-2009 Erin Catto http://www.gphysics.com
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies.
 * Erin Catto makes no representations about the suitability
 * of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */

#pragma once

#include "arbiter.h"
#include "math.h"

#include <etl/map.h>
#include <etl/vector.h>

namespace luya::physics {

class Body;
class Joint;

class World
{
  public:
    World(Vec2 gravity, int iterations)
        : gravity(gravity)
        , iterations(iterations)
    {
    }

  public:
    void Add(Body* body);
    void Add(Joint* joint);
    void Clear();
    void Step(float dt);
    void BroadPhase();

  public:
    etl::vector<Body*, 256> bodies;
    etl::vector<Joint*, 256> joints;
    etl::map<ArbiterKey, Arbiter, 256> arbiters;
    Vec2 gravity;
    int iterations;
    static bool accumulateImpulses;
    static bool warmStarting;
    static bool positionCorrection;
};

} // namespace physics