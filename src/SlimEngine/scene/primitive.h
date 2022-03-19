#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/mat4.h"
#include "../math/quat.h"

INLINE void convertPositionAndDirectionToObjectSpace(
    vec3 position, 
    vec3 dir, 
    Primitive *primitive,
    vec3 *out_position,
    vec3 *out_direction
) {
    *out_position = primitive->flags & IS_TRANSLATED ?
                    subVec3(position, primitive->position) :
                    position;

    if (primitive->flags & IS_ROTATED) {
        quat inv_rotation = conjugate(primitive->rotation);
        *out_position = mulVec3Quat(*out_position, inv_rotation);
        *out_direction = mulVec3Quat(dir, inv_rotation);
    } else
        *out_direction = dir;

    if (primitive->flags & IS_SCALED) {
        vec3 inv_scale = oneOverVec3(primitive->scale);
        *out_position = mulVec3(*out_position, inv_scale);
        if (primitive->flags & IS_SCALED_NON_UNIFORMLY)
            *out_direction = normVec3(mulVec3(*out_direction, inv_scale));
    }
}

INLINE vec3 convertPositionToWorldSpace(vec3 position, Primitive *primitive) {
    if (primitive->flags & IS_SCALED)     position = mulVec3(    position, primitive->scale);
    if (primitive->flags & IS_ROTATED)    position = mulVec3Quat(position, primitive->rotation);
    if (primitive->flags & IS_TRANSLATED) position = addVec3(    position, primitive->position);
    return position;
}
INLINE vec3 convertPositionToObjectSpace(vec3 position, Primitive *primitive) {
    if (primitive->flags & IS_TRANSLATED) position = subVec3(    position, primitive->position);
    if (primitive->flags & IS_ROTATED)    position = mulVec3Quat(position, conjugate(primitive->rotation));
    if (primitive->flags & IS_SCALED)     position = mulVec3(position, oneOverVec3(primitive->scale));
    return position;
}

INLINE vec3 convertDirectionToWorldSpace(vec3 direction, Primitive *primitive) {
    if (primitive->flags & IS_SCALED_NON_UNIFORMLY) direction = mulVec3(direction, oneOverVec3(primitive->scale));
    if (primitive->flags & IS_ROTATED)              direction = mulVec3Quat(direction,         primitive->rotation);
    return direction;
}

void rotatePrimitive(Primitive *primitive, f32 yaw, f32 pitch, f32 roll) {
    quat rotation;
    if (roll) {
        rotation.amount = roll;
        rotation.axis.x = 0;
        rotation.axis.y = 0;
        rotation.axis.z = 1;
    } else
        rotation = getIdentityQuaternion();

    if (pitch) {
        quat pitch_rotation;
        pitch_rotation.amount = pitch;
        pitch_rotation.axis.x = 1;
        pitch_rotation.axis.y = 0;
        pitch_rotation.axis.z = 0;
        pitch_rotation = normQuat(pitch_rotation);
        rotation = mulQuat(rotation, pitch_rotation);
        rotation = normQuat(rotation);
    }

    if (yaw) {
        quat yaw_rotation;
        yaw_rotation.amount = yaw;
        yaw_rotation.axis.x = 0;
        yaw_rotation.axis.y = 1;
        yaw_rotation.axis.z = 0;
        yaw_rotation = normQuat(yaw_rotation);
        rotation = mulQuat(rotation, yaw_rotation);
        rotation = normQuat(rotation);
    }

    primitive->rotation = mulQuat(primitive->rotation, rotation);
    primitive->rotation = normQuat(primitive->rotation);
}