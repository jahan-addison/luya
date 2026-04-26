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

#include "arbiter.h"    // for ArbiterKey, operator<
#include "math.h"       // for Vec2
#include <cstddef>      // for size_t
#include <etl/map.h>    // for map
#include <etl/vector.h> // for vector

namespace luya::physics {
class Body;
} // lines 28-28
namespace luya::physics {
class Joint;
} // lines 29-29

namespace luya::physics {

constexpr std::size_t MAX_PHYSICS_BODIES = 256;

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
    etl::vector<Body*, MAX_PHYSICS_BODIES> bodies;
    etl::vector<Joint*, MAX_PHYSICS_BODIES> joints;
    etl::map<ArbiterKey, Arbiter, MAX_PHYSICS_BODIES> arbiters;
    Vec2 gravity;
    int iterations;
    static bool accumulateImpulses;
    static bool warmStarting;
    static bool positionCorrection;
};

} // namespace physics