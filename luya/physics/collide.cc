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

#include <luya/physics/arbiter.h> // for Feature_Pair, Contact, collide
#include <luya/physics/body.h>    // for Body
#include <luya/physics/math.h>    // for Vec2, Mat22, operator*, dot, opera...

/****************************************************************************
 * Collide
 *
 * Box-vs-box contact generation using the separating axis theorem. Selects
 * the least-penetrating axis, identifies the reference and incident edges,
 * clips the incident edge to the reference face planes, and writes up to
 * two Contact points with separation, position, and feature data.
 *
 ****************************************************************************/

namespace luya::physics {

// Box vertex and edge numbering:
//
//        ^ y
//        |
//        e1
//   v2 ------ v1
//    |        |
// e2 |        | e4  --> x
//    |        |
//   v3 ------ v4
//        e3

enum Axis
{
    FACE_A_X,
    FACE_A_Y,
    FACE_B_X,
    FACE_B_Y
};

enum EdgeNumbers
{
    NO_EDGE = 0,
    EDGE1,
    EDGE2,
    EDGE3,
    EDGE4
};

struct ClipVertex
{
    ClipVertex() { fp.value = 0; }
    Vec2 v;
    Feature_Pair fp;
};

void flip(Feature_Pair& fp)
{
    swap_val(fp.e.in_edge1, fp.e.in_edge2);
    swap_val(fp.e.out_edge1, fp.e.out_edge2);
}

int clip_segment_to_line(ClipVertex v_out[2],
    ClipVertex v_in[2],
    const Vec2& normal,
    float offset,
    char clip_edge)
{
    // Start with no output points
    int num_out = 0;

    // Calculate the distance of end points to the line
    float distance0 = dot(normal, v_in[0].v) - offset;
    float distance1 = dot(normal, v_in[1].v) - offset;

    // If the points are behind the plane
    if (distance0 <= 0.0f)
        v_out[num_out++] = v_in[0];
    if (distance1 <= 0.0f)
        v_out[num_out++] = v_in[1];

    // If the points are on different sides of the plane
    if (distance0 * distance1 < 0.0f) {
        // Find intersection point of edge and plane
        float interp = distance0 / (distance0 - distance1);
        v_out[num_out].v = v_in[0].v + interp * (v_in[1].v - v_in[0].v);
        if (distance0 > 0.0f) {
            v_out[num_out].fp = v_in[0].fp;
            v_out[num_out].fp.e.in_edge1 = clip_edge;
            v_out[num_out].fp.e.in_edge2 = NO_EDGE;
        } else {
            v_out[num_out].fp = v_in[1].fp;
            v_out[num_out].fp.e.out_edge1 = clip_edge;
            v_out[num_out].fp.e.out_edge2 = NO_EDGE;
        }
        ++num_out;
    }

    return num_out;
}

static void compute_incident_edge(ClipVertex c[2],
    const Vec2& h,
    const Vec2& pos,
    const Mat22& rot,
    const Vec2& normal)
{
    // The normal is from the reference box. Convert it
    // to the incident boxe's frame and flip sign.
    Mat22 rot_t = rot.transpose();
    Vec2 n = -(rot_t * normal);
    Vec2 n_abs = abs_val(n);

    if (n_abs.x > n_abs.y) {
        if (sign(n.x) > 0.0f) {
            c[0].v.set(h.x, -h.y);
            c[0].fp.e.in_edge2 = EDGE3;
            c[0].fp.e.out_edge2 = EDGE4;

            c[1].v.set(h.x, h.y);
            c[1].fp.e.in_edge2 = EDGE4;
            c[1].fp.e.out_edge2 = EDGE1;
        } else {
            c[0].v.set(-h.x, h.y);
            c[0].fp.e.in_edge2 = EDGE1;
            c[0].fp.e.out_edge2 = EDGE2;

            c[1].v.set(-h.x, -h.y);
            c[1].fp.e.in_edge2 = EDGE2;
            c[1].fp.e.out_edge2 = EDGE3;
        }
    } else {
        if (sign(n.y) > 0.0f) {
            c[0].v.set(h.x, h.y);
            c[0].fp.e.in_edge2 = EDGE4;
            c[0].fp.e.out_edge2 = EDGE1;

            c[1].v.set(-h.x, h.y);
            c[1].fp.e.in_edge2 = EDGE1;
            c[1].fp.e.out_edge2 = EDGE2;
        } else {
            c[0].v.set(-h.x, -h.y);
            c[0].fp.e.in_edge2 = EDGE2;
            c[0].fp.e.out_edge2 = EDGE3;

            c[1].v.set(h.x, -h.y);
            c[1].fp.e.in_edge2 = EDGE3;
            c[1].fp.e.out_edge2 = EDGE4;
        }
    }

    c[0].v = pos + rot * c[0].v;
    c[1].v = pos + rot * c[1].v;
}

// The normal points from A to B
int collide(Contact* contacts, Body const* bodyA, Body const* bodyB)
{
    // Setup
    Vec2 h_a = 0.5f * bodyA->width;
    Vec2 h_b = 0.5f * bodyB->width;

    Vec2 pos_a = bodyA->position;
    Vec2 pos_b = bodyB->position;

    Mat22 rot_a(bodyA->rotation), rot_b(bodyB->rotation);

    Mat22 rot_at = rot_a.transpose();
    Mat22 rot_bt = rot_b.transpose();

    Vec2 dp = pos_b - pos_a;
    Vec2 d_a = rot_at * dp;
    Vec2 d_b = rot_bt * dp;

    Mat22 C = rot_at * rot_b;
    Mat22 abs_c = abs_val(C);
    Mat22 abs_ct = abs_c.transpose();

    // Box A faces
    Vec2 face_a = abs_val(d_a) - h_a - abs_c * h_b;
    if (face_a.x > 0.0f || face_a.y > 0.0f)
        return 0;

    // Box B faces
    Vec2 face_b = abs_val(d_b) - abs_ct * h_a - h_b;
    if (face_b.x > 0.0f || face_b.y > 0.0f)
        return 0;

    // Find best axis
    Axis axis;
    float separation;
    Vec2 normal;

    // Box A faces
    axis = FACE_A_X;
    separation = face_a.x;
    normal = d_a.x > 0.0f ? rot_a.col1 : -rot_a.col1;

    const float relative_tol = 0.95f;
    const float absolute_tol = 0.01f;

    if (face_a.y > relative_tol * separation + absolute_tol * h_a.y) {
        axis = FACE_A_Y;
        separation = face_a.y;
        normal = d_a.y > 0.0f ? rot_a.col2 : -rot_a.col2;
    }

    // Box B faces
    if (face_b.x > relative_tol * separation + absolute_tol * h_b.x) {
        axis = FACE_B_X;
        separation = face_b.x;
        normal = d_b.x > 0.0f ? rot_b.col1 : -rot_b.col1;
    }

    if (face_b.y > relative_tol * separation + absolute_tol * h_b.y) {
        axis = FACE_B_Y;
        separation = face_b.y;
        normal = d_b.y > 0.0f ? rot_b.col2 : -rot_b.col2;
    }

    // Setup clipping plane data based on the separating axis
    Vec2 front_normal, side_normal;
    ClipVertex incident_edge[2];
    float front, neg_side, pos_side;
    char neg_edge, pos_edge;

    // Compute the clipping lines and the line segment to be clipped.
    switch (axis) {
        case FACE_A_X: {
            front_normal = normal;
            front = dot(pos_a, front_normal) + h_a.x;
            side_normal = rot_a.col2;
            float side = dot(pos_a, side_normal);
            neg_side = -side + h_a.y;
            pos_side = side + h_a.y;
            neg_edge = EDGE3;
            pos_edge = EDGE1;
            compute_incident_edge(
                incident_edge, h_b, pos_b, rot_b, front_normal);
        } break;

        case FACE_A_Y: {
            front_normal = normal;
            front = dot(pos_a, front_normal) + h_a.y;
            side_normal = rot_a.col1;
            float side = dot(pos_a, side_normal);
            neg_side = -side + h_a.x;
            pos_side = side + h_a.x;
            neg_edge = EDGE2;
            pos_edge = EDGE4;
            compute_incident_edge(
                incident_edge, h_b, pos_b, rot_b, front_normal);
        } break;

        case FACE_B_X: {
            front_normal = -normal;
            front = dot(pos_b, front_normal) + h_b.x;
            side_normal = rot_b.col2;
            float side = dot(pos_b, side_normal);
            neg_side = -side + h_b.y;
            pos_side = side + h_b.y;
            neg_edge = EDGE3;
            pos_edge = EDGE1;
            compute_incident_edge(
                incident_edge, h_a, pos_a, rot_a, front_normal);
        } break;

        case FACE_B_Y: {
            front_normal = -normal;
            front = dot(pos_b, front_normal) + h_b.y;
            side_normal = rot_b.col1;
            float side = dot(pos_b, side_normal);
            neg_side = -side + h_b.x;
            pos_side = side + h_b.x;
            neg_edge = EDGE2;
            pos_edge = EDGE4;
            compute_incident_edge(
                incident_edge, h_a, pos_a, rot_a, front_normal);
        } break;
    }

    // clip other face with 5 box planes (1 face plane, 4 edge planes)

    ClipVertex clip_points1[2];
    ClipVertex clip_points2[2];
    int np;

    // Clip to box side 1
    np = clip_segment_to_line(
        clip_points1, incident_edge, -side_normal, neg_side, neg_edge);

    if (np < 2)
        return 0;

    // Clip to negative box side 1
    np = clip_segment_to_line(
        clip_points2, clip_points1, side_normal, pos_side, pos_edge);

    if (np < 2)
        return 0;

    // Now clip_points2 contains the clipping points.
    // Due to roundoff, it is possible that clipping removes all points.

    int num_contacts = 0;
    for (int i = 0; i < 2; ++i) {
        float c_separation = dot(front_normal, clip_points2[i].v) - front;

        if (c_separation <= 0) {
            contacts[num_contacts].separation = c_separation;
            contacts[num_contacts].normal = normal;
            // slide contact point onto reference face (easy to cull)
            contacts[num_contacts].position =
                clip_points2[i].v - c_separation * front_normal;
            contacts[num_contacts].feature = clip_points2[i].fp;
            if (axis == FACE_B_X || axis == FACE_B_Y)
                flip(contacts[num_contacts].feature);
            ++num_contacts;
        }
    }

    return num_contacts;
}

} // namespace physics