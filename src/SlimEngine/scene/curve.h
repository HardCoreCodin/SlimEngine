#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/quat.h"
#include "../shapes/edge.h"
#include "./primitive.h"

#define CURVE_STEPS 3600

void drawCurve(Curve *curve, u32 step_count, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    f32 one_over_step_count = 1.0f / (f32)step_count;
    f32 rotation_step = one_over_step_count * TAU;
    f32 rotation_step_times_rev_count = rotation_step * (f32)curve->revolution_count;

    if (primitive->type == PrimitiveType_Helix)
        rotation_step = rotation_step_times_rev_count;

    vec3 center_to_orbit;
    center_to_orbit.x = 1;
    center_to_orbit.y = center_to_orbit.z = 0;

    vec3 orbit_to_curve;
    orbit_to_curve.x = curve->thickness;
    orbit_to_curve.y = orbit_to_curve.z = 0;

    mat3 rotation;
    rotation.X.x = rotation.Z.z = cosf(rotation_step);
    rotation.X.z = sinf(rotation_step);
    rotation.Z.x = -rotation.X.z;
    rotation.X.y = rotation.Z.y = rotation.Y.x = rotation.Y.z =  0;
    rotation.Y.y = 1;

    mat3 orbit_to_curve_rotation;
    if (primitive->type == PrimitiveType_Coil) {
        orbit_to_curve_rotation.X.x = orbit_to_curve_rotation.Y.y = cosf(rotation_step_times_rev_count);
        orbit_to_curve_rotation.X.y = sinf(rotation_step_times_rev_count);
        orbit_to_curve_rotation.Y.x = -orbit_to_curve_rotation.X.y;
        orbit_to_curve_rotation.X.z = orbit_to_curve_rotation.Y.z = orbit_to_curve_rotation.Z.x = orbit_to_curve_rotation.Z.y =  0;
        orbit_to_curve_rotation.Z.z = 1;
    }

    // Transform vertices positions of edges from view-space to screen-space (w/ culling and clipping):
    mat3 accumulated_orbit_rotation = rotation;
    vec3 current_position, previous_position;
    Edge edge;

    for (u32 i = 0; i < step_count; i++) {
        center_to_orbit = mulVec3Mat3(center_to_orbit, rotation);

        switch (primitive->type) {
            case PrimitiveType_Helix:
                current_position = center_to_orbit;
                current_position.y -= 1;
                break;
            case PrimitiveType_Coil:
                orbit_to_curve  = mulVec3Mat3(orbit_to_curve, orbit_to_curve_rotation);
                current_position = mulVec3Mat3(orbit_to_curve, accumulated_orbit_rotation);
                current_position = addVec3(center_to_orbit, current_position);
                break;
            default:
                break;
        }

        current_position = convertPositionToWorldSpace(current_position, primitive);
        current_position = subVec3(    current_position, viewport->camera->transform.position);
        current_position = mulVec3Quat(current_position, viewport->camera->transform.rotation_inverted);

        if (i) {
            edge.from = previous_position;
            edge.to   = current_position;
            drawEdge(&edge, color, opacity, line_width, viewport);
        }

        switch (primitive->type) {
            case PrimitiveType_Helix:
                center_to_orbit.y += 2 * one_over_step_count;
                break;
            case PrimitiveType_Coil:
                accumulated_orbit_rotation = mulMat3(accumulated_orbit_rotation, rotation);
                break;
            default:
                break;
        }

        previous_position = current_position;
    }
}