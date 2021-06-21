#pragma once

#include "../core/pixels.h"
#include "../math/math3D.h"
#include "../math/quat.h"

#include "./edge.h"

typedef struct Helix {
    u32 revolution_count;
} Helix;

void initHelix(Helix *helix) {
    helix->revolution_count = 10;
}

void drawHelix(Viewport *viewport, RGBA color, Helix *helix, Primitive *primitive, u32 step_count) {
    f32 one_over_step_count = 1.0f / (f32)step_count;
    f32 rotation_step = one_over_step_count * TAU * (f32)helix->revolution_count;

    vec3 center_to_orbit;
    center_to_orbit.x = 1;
    center_to_orbit.y = center_to_orbit.z = 0;

    mat3 rotation;
    rotation.X.x = rotation.Z.z = cosf(rotation_step);
    rotation.X.z = sinf(rotation_step);
    rotation.Z.x = -rotation.X.z;
    rotation.X.y = rotation.Z.y = rotation.Y.x = rotation.Y.z =  0;
    rotation.Y.y = 1;

    vec3 current_position, previous_position;
    Edge edge;
    for (u32 i = 0; i < step_count; i++) {
        center_to_orbit  = mulVec3Mat3(center_to_orbit, rotation);

        current_position = center_to_orbit;

        current_position = convertPositionToWorldSpace(current_position, primitive);
        current_position = subVec3(    current_position, viewport->camera->transform.position);
        current_position = mulVec3Quat(current_position, viewport->camera->transform.rotation_inverted);

        if (i) {
            edge.from = previous_position;
            edge.to   = current_position;
            drawEdge(viewport, color, &edge);
        }

        center_to_orbit.y += one_over_step_count;
        previous_position = current_position;
    }
}