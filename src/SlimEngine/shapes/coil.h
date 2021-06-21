#pragma once

#include "../core/pixels.h"
#include "../math/math3D.h"
#include "../scene/primitive.h"
#include "./edge.h"


typedef struct Coil {
    f32 thickness;
    u32 revolution_count;
} Coil;

void initCoil(Coil *coil) {
    coil->thickness = 0.3f;
    coil->revolution_count = 10;
}

void drawCoil(Viewport *viewport, RGBA color, Coil *coil, Primitive *primitive, u32 step_count) {
    f32 orbit_angle_step = TAU / (f32)step_count;
    f32 angle_step = orbit_angle_step * (f32)coil->revolution_count;

    vec3 center_to_orbit;
    center_to_orbit.x = 1;
    center_to_orbit.y = center_to_orbit.z = 0;

    vec3 orbit_to_curve;
    orbit_to_curve.x = coil->thickness;
    orbit_to_curve.y = orbit_to_curve.z = 0;

    mat3 orbit_rotation;
    orbit_rotation.X.x = orbit_rotation.Z.z = cosf(orbit_angle_step);
    orbit_rotation.X.z = sinf(orbit_angle_step);
    orbit_rotation.Z.x = -orbit_rotation.X.z;
    orbit_rotation.X.y = orbit_rotation.Z.y = orbit_rotation.Y.x = orbit_rotation.Y.z =  0;
    orbit_rotation.Y.y = 1;

    mat3 rotation;
    rotation.X.x = rotation.Y.y = cosf(angle_step);
    rotation.X.y = sinf(angle_step);
    rotation.Y.x = -rotation.X.y;
    rotation.X.z = rotation.Y.z = rotation.Z.x = rotation.Z.y =  0;
    rotation.Z.z = 1;

    // Transform vertices positions of edges from view-space to screen-space (w/ culling and clipping):
    mat3 accumulated_orbit_rotation = orbit_rotation;
    vec3 current_position, previous_position;
    Edge edge;

    for (u32 i = 0; i < step_count; i++) {
        center_to_orbit = mulVec3Mat3(center_to_orbit, orbit_rotation);
        orbit_to_curve  = mulVec3Mat3(orbit_to_curve, rotation);
        current_position = mulVec3Mat3(orbit_to_curve, accumulated_orbit_rotation);
        current_position = addVec3(center_to_orbit, current_position);
        current_position = convertPositionToWorldSpace(current_position, primitive);
        current_position = subVec3(    current_position, viewport->camera->transform.position);
        current_position = mulVec3Quat(current_position, viewport->camera->transform.rotation_inverted);

        if (i) {
            edge.from = previous_position;
            edge.to   = current_position;
            drawEdge(viewport, color, &edge);
        }

        accumulated_orbit_rotation = mulMat3(accumulated_orbit_rotation, orbit_rotation);
        previous_position = current_position;
    }
}