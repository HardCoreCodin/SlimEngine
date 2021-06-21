#pragma once

#include "../core/base.h"
#include "../math/math3D.h"
#include "../math/quat.h"

#define IS_VISIBLE ((u8)1)
#define IS_TRANSLATED ((u8)2)
#define IS_ROTATED ((u8)4)
#define IS_SCALED ((u8)8)
#define IS_SCALED_NON_UNIFORMLY ((u8)16)

enum PrimitiveType {
    PrimitiveType_None = 0,
    PrimitiveType_Quad,
    PrimitiveType_Box,
    PrimitiveType_Helix,
    PrimitiveType_Coil,
    PrimitiveType_Tetrahedron
};

typedef struct Primitive {
    quat rotation;
    vec3 position, scale;
    u32 id;
    enum PrimitiveType type;
    u8 flags;
} Primitive;

void initPrimitive(Primitive *primitive) {
    primitive->type = PrimitiveType_None;
    primitive->flags = IS_VISIBLE;
    primitive->id = 0;

    primitive->scale    = getVec3Of(1);
    primitive->position = getVec3Of(0);
    primitive->rotation = getIdentityQuaternion();
}

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

AABB getPrimitiveAABB(Primitive *primitive) {
    AABB aabb;

    aabb.max = getVec3Of(primitive->type == PrimitiveType_Tetrahedron ? SQRT3 / 3 : 1);
    aabb.min = invertedVec3(aabb.max);

    return aabb;
}

void transformAABB(AABB *aabb, Primitive *primitive) {
    f32 x = aabb->min.x;
    f32 y = aabb->min.y;
    f32 z = aabb->min.z;
    f32 X = aabb->max.x;
    f32 Y = aabb->max.y;
    f32 Z = aabb->max.z;

    vec3 v[8] = {
            {x, y, z},
            {x, y, Z},
            {x, Y, z},
            {x, Y, Z},
            {X, y, z},
            {X, y, Z},
            {X, Y, z},
            {X, Y, Z}
    };

    x = y = z = +INFINITY;
    X = Y = Z = -INFINITY;

    vec3 position;
    for (u8 i = 0; i < 8; i++) {
        position = convertPositionToWorldSpace(v[i], primitive);

        if (position.x > X) X = position.x;
        if (position.y > Y) Y = position.y;
        if (position.z > Z) Z = position.z;
        if (position.x < x) x = position.x;
        if (position.y < y) y = position.y;
        if (position.z < z) z = position.z;
    }

    aabb->min.x = x;
    aabb->min.y = y;
    aabb->min.z = z;
    aabb->max.x = X;
    aabb->max.y = Y;
    aabb->max.z = Z;
}