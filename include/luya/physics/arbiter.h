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

#include "math.h"

/****************************************************************************
 * Arbiter
 *
 * Contact manifold between two bodies. An Arbiter is keyed by
 * a pair of body pointers (Arbiter_Key) and cached in World::arbiters across
 * frames to support warm-starting.
 *
 * Contact points carry accumulated normal impulse pn, tangent impulse pt,
 * and position-bias impulse pnb. pre_step() computes effective mass and
 * Baumgarte bias; apply_impulse() resolves the constraint for the current
 * solver iteration.
 *
 * World::broad_phase() creates and updates Arbiters automatically. To detect
 * that two bodies are in contact, inspect World::arbiters:
 *
 *   if (!world.arbiters.empty()) {
 *       // at least one contact pair is active this step
 *   }
 *
 * You should not need to construct or update Arbiters directly.
 *
 ****************************************************************************/

namespace luya::physics {

class Body;

/**
 * @brief
 * Encodes which box edges are involved in a contact point.
 */
union Feature_Pair
{
    struct Edges
    {
        char in_edge1;
        char out_edge1;
        char in_edge2;
        char out_edge2;
    } e;
    int value;
};

/**
 * @brief
 * A single contact point between two bodies, carrying position, normal,
 * accumulated impulses, and effective mass.
 */
struct Contact
{
    Contact()
        : pn(0.0f)
        , pt(0.0f)
        , pnb(0.0f)
        , separation(0.0f)
        , mass_normal(0.0f)
        , mass_tangent(0.0f)
        , bias(0.0f)
        , feature{ .value = 0 }
    {
    }

    float pn;  // accumulated normal impulse
    float pt;  // accumulated tangent impulse
    float pnb; // accumulated normal impulse for position bias

    float separation;
    float mass_normal;
    float mass_tangent;
    float bias;

    Feature_Pair feature;

    Vec2 position{};
    Vec2 normal{};
    Vec2 r1{}, r2{};
};

/**
 * @brief
 * Canonical key for the arbiter map — ordered pair of body pointers.
 */
struct Arbiter_Key
{
    Arbiter_Key(Body* b1, Body* b2)
    {
        if (b1 < b2) {
            body1 = b1;
            body2 = b2;
        } else {
            body1 = b2;
            body2 = b1;
        }
    }

    Body* body1;
    Body* body2;
};

/**
 * @brief
 * Contact manifold between two bodies. Cached across frames to support
 * warm-starting; updated each step by World::broad_phase().
 */
struct Arbiter
{
    enum
    {
        MAX_POINTS = 2
    };

    Arbiter(Body* b1, Body* b2);

    void update(Contact* contacts, int num_contacts);

    void pre_step(float inv_dt);
    void apply_impulse();

    Contact contacts[MAX_POINTS];
    int num_contacts;

    Body* body1;
    Body* body2;

    // Combined friction
    float friction;
};

// This is used by std::set
inline bool operator<(const Arbiter_Key& a1, const Arbiter_Key& a2)
{
    if (a1.body1 < a2.body1)
        return true;

    if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
        return true;

    return false;
}

int collide(Contact* contacts, Body const* bodyA, Body const* bodyB);
} // namespace physics