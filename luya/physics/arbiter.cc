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

#include <luya/physics/arbiter.h>

#include <luya/physics/body.h>  // for Body
#include <luya/physics/math.h>  // for Vec2, cross, dot, operator*, operator-
#include <luya/physics/world.h> // for World
#include <math.h>               // for sqrtf

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

Arbiter::Arbiter(Body* b1, Body* b2)
{
    if (b1 < b2) {
        body1 = b1;
        body2 = b2;
    } else {
        body1 = b2;
        body2 = b1;
    }

    num_contacts = collide(contacts, body1, body2);

    friction = sqrtf(body1->friction * body2->friction);
}

void Arbiter::update(Contact* new_contacts, int num_new_contacts)
{
    Contact merged_contacts[2];

    for (int i = 0; i < num_new_contacts; ++i) {
        Contact const* c_new = new_contacts + i;
        int k = -1;
        for (int j = 0; j < num_contacts; ++j) {
            Contact const* c_old = contacts + j;
            if (c_new->feature.value == c_old->feature.value) {
                k = j;
                break;
            }
        }

        if (k > -1) {
            Contact* c = merged_contacts + i;
            Contact const* c_old = contacts + k;
            *c = *c_new;
            if (World::warm_starting) {
                c->pn = c_old->pn;
                c->pt = c_old->pt;
                c->pnb = c_old->pnb;
            } else {
                c->pn = 0.0f;
                c->pt = 0.0f;
                c->pnb = 0.0f;
            }
        } else {
            merged_contacts[i] = new_contacts[i];
        }
    }

    for (int i = 0; i < num_new_contacts; ++i)
        contacts[i] = merged_contacts[i];

    num_contacts = num_new_contacts;
}

void Arbiter::pre_step(float inv_dt)
{
    const float k_allowed_penetration = 0.01f;
    float k_bias_factor = World::position_correction ? 0.2f : 0.0f;

    for (int i = 0; i < num_contacts; ++i) {
        Contact* c = contacts + i;

        Vec2 r1 = c->position - body1->position;
        Vec2 r2 = c->position - body2->position;

        // Precompute normal mass, tangent mass, and bias.
        float rn1 = dot(r1, c->normal);
        float rn2 = dot(r2, c->normal);
        float k_normal = body1->inv_mass + body2->inv_mass;
        k_normal += body1->inv_i * (dot(r1, r1) - rn1 * rn1) +
                    body2->inv_i * (dot(r2, r2) - rn2 * rn2);
        c->mass_normal = 1.0f / k_normal;

        Vec2 tangent = cross(c->normal, 1.0f);
        float rt1 = dot(r1, tangent);
        float rt2 = dot(r2, tangent);
        float k_tangent = body1->inv_mass + body2->inv_mass;
        k_tangent += body1->inv_i * (dot(r1, r1) - rt1 * rt1) +
                     body2->inv_i * (dot(r2, r2) - rt2 * rt2);
        c->mass_tangent = 1.0f / k_tangent;

        c->bias = -k_bias_factor * inv_dt *
                  min_val(0.0f, c->separation + k_allowed_penetration);

        if (World::accumulate_impulses) {
            // Apply normal + friction impulse
            Vec2 P = c->pn * c->normal + c->pt * tangent;

            body1->velocity -= body1->inv_mass * P;
            body1->angular_velocity -= body1->inv_i * cross(r1, P);

            body2->velocity += body2->inv_mass * P;
            body2->angular_velocity += body2->inv_i * cross(r2, P);
        }
    }
}

void Arbiter::apply_impulse()
{
    Body* b1 = body1;
    Body* b2 = body2;

    for (int i = 0; i < num_contacts; ++i) {
        Contact* c = contacts + i;
        c->r1 = c->position - b1->position;
        c->r2 = c->position - b2->position;

        // Relative velocity at contact
        Vec2 dv = b2->velocity + cross(b2->angular_velocity, c->r2) -
                  b1->velocity - cross(b1->angular_velocity, c->r1);

        // Compute normal impulse
        float vn = dot(dv, c->normal);

        float d_pn = c->mass_normal * (-vn + c->bias);

        if (World::accumulate_impulses) {
            // clamp the accumulated impulse
            float pn0 = c->pn;
            c->pn = max_val(pn0 + d_pn, 0.0f);
            d_pn = c->pn - pn0;
        } else {
            d_pn = max_val(d_pn, 0.0f);
        }

        // Apply contact impulse
        Vec2 pn = d_pn * c->normal;

        b1->velocity -= b1->inv_mass * pn;
        b1->angular_velocity -= b1->inv_i * cross(c->r1, pn);

        b2->velocity += b2->inv_mass * pn;
        b2->angular_velocity += b2->inv_i * cross(c->r2, pn);

        // Relative velocity at contact
        dv = b2->velocity + cross(b2->angular_velocity, c->r2) - b1->velocity -
             cross(b1->angular_velocity, c->r1);

        Vec2 tangent = cross(c->normal, 1.0f);
        float vt = dot(dv, tangent);
        float d_pt = c->mass_tangent * (-vt);

        if (World::accumulate_impulses) {
            // Compute friction impulse
            float max_pt = friction * c->pn;

            // clamp friction
            float old_tangent_impulse = c->pt;
            c->pt = clamp(old_tangent_impulse + d_pt, -max_pt, max_pt);
            d_pt = c->pt - old_tangent_impulse;
        } else {
            float max_pt = friction * d_pn;
            d_pt = clamp(d_pt, -max_pt, max_pt);
        }

        // Apply contact impulse
        Vec2 pt = d_pt * tangent;

        b1->velocity -= b1->inv_mass * pt;
        b1->angular_velocity -= b1->inv_i * cross(c->r1, pt);

        b2->velocity += b2->inv_mass * pt;
        b2->angular_velocity += b2->inv_i * cross(c->r2, pt);
    }
}

} // namespace physics