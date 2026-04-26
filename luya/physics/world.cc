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

#include <luya/physics/world.h>

#include <etl/map.h>              // for map, operator!=, operator==
#include <etl/utility.h>          // for pair
#include <etl/vector.h>           // for vector
#include <luya/physics/arbiter.h> // for Arbiter_Key, operator<, Arbiter
#include <luya/physics/body.h>    // for Body
#include <luya/physics/joint.h>   // for Joint
#include <luya/physics/math.h>    // for Vec2, operator*, operator+

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

typedef etl::map<Arbiter_Key, Arbiter, MAX_PHYSICS_BODIES>::iterator ArbIter;
typedef etl::pair<Arbiter_Key, Arbiter> ArbPair;

bool World::accumulate_impulses = true;
bool World::warm_starting = true;
bool World::position_correction = true;

void World::add(Body* body)
{
    bodies.push_back(body);
}

void World::add(Joint* joint)
{
    joints.push_back(joint);
}

void World::clear()
{
    bodies.clear();
    joints.clear();
    arbiters.clear();
}

void World::broad_phase()
{
    // O(n^2) broad-phase
    for (int i = 0; i < (int)bodies.size(); ++i) {
        Body* bi = bodies[i];

        for (int j = i + 1; j < (int)bodies.size(); ++j) {
            Body* bj = bodies[j];

            if (bi->inv_mass == 0.0f && bj->inv_mass == 0.0f)
                continue;

            Arbiter new_arb(bi, bj);
            Arbiter_Key key(bi, bj);

            if (new_arb.num_contacts > 0) {
                ArbIter iter = arbiters.find(key);
                if (iter == arbiters.end()) {
                    arbiters.insert(ArbPair(key, new_arb));
                } else {
                    iter->second.update(new_arb.contacts, new_arb.num_contacts);
                }
            } else {
                arbiters.erase(key);
            }
        }
    }
}

void World::step(float dt)
{
    float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

    // Determine overlapping bodies and update contact points.
    broad_phase();

    // Integrate forces.
    for (int i = 0; i < (int)bodies.size(); ++i) {
        Body* b = bodies[i];

        if (b->inv_mass == 0.0f)
            continue;

        b->velocity += dt * (gravity + b->inv_mass * b->force);
        b->angular_velocity += dt * b->inv_i * b->torque;
    }

    // Perform pre-steps.
    for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb) {
        arb->second.pre_step(inv_dt);
    }

    for (int i = 0; i < (int)joints.size(); ++i) {
        joints[i]->pre_step(inv_dt);
    }

    // Perform iterations
    for (int i = 0; i < iterations; ++i) {
        for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb) {
            arb->second.apply_impulse();
        }

        for (int j = 0; j < (int)joints.size(); ++j) {
            joints[j]->apply_impulse();
        }
    }

    // Integrate Velocities
    for (int i = 0; i < (int)bodies.size(); ++i) {
        Body* b = bodies[i];

        b->position += dt * b->velocity;
        b->rotation += dt * b->angular_velocity;

        b->force.set(0.0f, 0.0f);
        b->torque = 0.0f;
    }
}

} // namespace physics