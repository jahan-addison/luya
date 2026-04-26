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

#include <luya/physics/joint.h>

#include <luya/physics/body.h>  // for Body
#include <luya/physics/math.h>  // for Vec2, Mat22, operator*, operator-
#include <luya/physics/world.h> // for World

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

void Joint::set(Body* b1, Body* b2, const Vec2& anchor)
{
    body1 = b1;
    body2 = b2;

    Mat22 rot_1(body1->rotation);
    Mat22 rot_2(body2->rotation);
    Mat22 rot_1t = rot_1.transpose();
    Mat22 rot_2t = rot_2.transpose();

    local_anchor1 = rot_1t * (anchor - body1->position);
    local_anchor2 = rot_2t * (anchor - body2->position);

    P.set(0.0f, 0.0f);

    softness = 0.0f;
    bias_factor = 0.2f;
}

void Joint::pre_step(float inv_dt)
{
    // Pre-compute anchors, mass matrix, and bias.
    Mat22 rot_1(body1->rotation);
    Mat22 rot_2(body2->rotation);

    r1 = rot_1 * local_anchor1;
    r2 = rot_2 * local_anchor2;

    // deltaV = deltaV0 + K * impulse
    // invM = [(1/m1 + 1/m2) * eye(2) - skew(r1) * inv_i1 * skew(r1) -
    // skew(r2) * inv_i2 * skew(r2)]
    //      = [1/m1+1/m2     0    ] + inv_i1 * [r1.y*r1.y -r1.x*r1.y] + inv_i2
    //      * [r1.y*r1.y -r1.x*r1.y]
    //        [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]
    //        [-r1.x*r1.y r1.x*r1.x]
    Mat22 K1;
    K1.col1.x = body1->inv_mass + body2->inv_mass;
    K1.col2.x = 0.0f;
    K1.col1.y = 0.0f;
    K1.col2.y = body1->inv_mass + body2->inv_mass;

    Mat22 K2;
    K2.col1.x = body1->inv_i * r1.y * r1.y;
    K2.col2.x = -body1->inv_i * r1.x * r1.y;
    K2.col1.y = -body1->inv_i * r1.x * r1.y;
    K2.col2.y = body1->inv_i * r1.x * r1.x;

    Mat22 K3;
    K3.col1.x = body2->inv_i * r2.y * r2.y;
    K3.col2.x = -body2->inv_i * r2.x * r2.y;
    K3.col1.y = -body2->inv_i * r2.x * r2.y;
    K3.col2.y = body2->inv_i * r2.x * r2.x;

    Mat22 K = K1 + K2 + K3;
    K.col1.x += softness;
    K.col2.y += softness;

    M = K.invert();

    Vec2 p1 = body1->position + r1;
    Vec2 p2 = body2->position + r2;
    Vec2 dp = p2 - p1;

    if (World::position_correction) {
        bias = -bias_factor * inv_dt * dp;
    } else {
        bias.set(0.0f, 0.0f);
    }

    if (World::warm_starting) {
        // Apply accumulated impulse.
        body1->velocity -= body1->inv_mass * P;
        body1->angular_velocity -= body1->inv_i * cross(r1, P);

        body2->velocity += body2->inv_mass * P;
        body2->angular_velocity += body2->inv_i * cross(r2, P);
    } else {
        P.set(0.0f, 0.0f);
    }
}

void Joint::apply_impulse()
{
    Vec2 dv = body2->velocity + cross(body2->angular_velocity, r2) -
              body1->velocity - cross(body1->angular_velocity, r1);

    Vec2 impulse;

    impulse = M * (bias - dv - softness * P);

    body1->velocity -= body1->inv_mass * impulse;
    body1->angular_velocity -= body1->inv_i * cross(r1, impulse);

    body2->velocity += body2->inv_mass * impulse;
    body2->angular_velocity += body2->inv_i * cross(r2, impulse);

    P += impulse;
}

} // namespace physics