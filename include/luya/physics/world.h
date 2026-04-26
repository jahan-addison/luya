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

#include "arbiter.h"    // for Arbiter_Key, operator<
#include "math.h"       // for Vec2
#include <cstddef>      // for size_t
#include <etl/map.h>    // for map
#include <etl/vector.h> // for vector

/****************************************************************************
 * World
 *
 * Simulates a set of rigid bodies and constraints under gravity using
 * sequential impulse solving over a fixed timestep. add bodies and joints
 * with add(), then drive the simulation by calling step(dt) once per frame.
 *
 *  Example:
 *
 *   physics::World world({ 0.0f, -10.0f }, 10);
 *
 *   physics::Body floor;
 *   floor.position = { 0.0f, -4.0f };   // static, inv_mass = 0
 *
 *   physics::Body box;
 *   box.set({ 0.5f, 0.5f }, 1.0f);
 *   box.position = { 0.0f, 2.0f };
 *
 *   world.add(&floor);
 *   world.add(&box);
 *
 *   world.step(1.0f / 60.0f);  // advance one frame at 60 Hz
 *
 * Collision detection runs in broad_phase() and produces Arbiter entries.
 * world.arbiters is non-empty for each active overlapping body pair this
 * step. Call clear() to remove all bodies and joints between scenes.
 *
 ****************************************************************************/

namespace luya::physics {

constexpr std::size_t MAX_PHYSICS_BODIES = 256;

class Body;
class Joint;

/**
 * @brief
 * Physics simulation context. Owns the body, joint, and arbiter lists;
 * advances the simulation via step().
 */
class World
{
  public:
    World(Vec2 gravity, int iterations)
        : gravity(gravity)
        , iterations(iterations)
    {
    }

  public:
    void add(Body* body);
    void add(Joint* joint);
    void clear();
    void step(float dt);
    void broad_phase();

  public:
    etl::vector<Body*, MAX_PHYSICS_BODIES> bodies;
    etl::vector<Joint*, MAX_PHYSICS_BODIES> joints;
    etl::map<Arbiter_Key, Arbiter, MAX_PHYSICS_BODIES> arbiters;
    Vec2 gravity;
    int iterations;
    static bool accumulate_impulses;
    static bool warm_starting;
    static bool position_correction;
};

} // namespace physics