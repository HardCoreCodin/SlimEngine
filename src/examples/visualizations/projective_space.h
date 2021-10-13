#pragma once

#include "./core.h"
#include "./transition.h"

#define PROJECTION_LINES_COUNT 36
#define PROJECTION_LINE_INC_SPEED 0.3f

void drawProjectiveSpace(Viewport *viewport, Transition *transition, bool colorize) {
    Edge edge;
    f32 step = TAU / (PROJECTION_LINES_COUNT * 2);
    vec2 sin_cos = Vec2(cosf(step), sinf(step));

    vec3 color = getVec3Of(((f32)MAX_COLOR_VALUE / 2));
    vec3 x_axis = Vec3(1, 0, 0);
    vec3 y_axis = Vec3(0, 1, 0);
    quat azimuth_rotation;
    quat altitude_rotation = getIdentityQuaternion();
    quat altitude_rotation_step = getRotationAroundAxisBySinCon(x_axis, sin_cos);
    quat azimuth_rotation_step  = getRotationAroundAxisBySinCon(y_axis, sin_cos);

    for (i32 z = 0; z < PROJECTION_LINES_COUNT; z++) {
        azimuth_rotation = getIdentityQuaternion();
        for (i32 x = 0; x < PROJECTION_LINES_COUNT; x++) {
            edge.from = getVec3Of(0);
            edge.to = mulVec3Quat(Vec3(1, 0, 0), mulQuat(altitude_rotation, azimuth_rotation));
            if (colorize) {
                color.r = (f32)MAX_COLOR_VALUE / 4 + color.g * edge.to.x;
                color.b = (f32)MAX_COLOR_VALUE / 4 + color.g * edge.to.z;
            }

            edge.to = scaleVec3(edge.to, 15 * transition->eased_t);
            convertEdgeFromSecondaryToMain(&edge);

            drawEdge(&edge, color, PROJECTION_LINES_OPACITY, PROJECTION_LINE_WIDTH, viewport);

            azimuth_rotation = mulQuat(azimuth_rotation, azimuth_rotation_step);
        }
        altitude_rotation = mulQuat(altitude_rotation, altitude_rotation_step);
    }
}