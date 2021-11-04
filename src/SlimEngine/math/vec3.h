#pragma once

#include "../core/base.h"

INLINE bool isEqualVec3(vec3 a, vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

INLINE vec3 clampVec3ToZero(vec3 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    v.z = v.z > 0.0f ? v.z : 0.0f;
    return v;
}

INLINE vec3 clampVec3ToUpper(vec3 v, vec3 upper) {
    v.x = v.x < upper.x ? v.x : upper.x;
    v.y = v.y < upper.y ? v.y : upper.y;
    v.z = v.z < upper.z ? v.z : upper.z;
    return v;
}

INLINE vec3 clampVec3(vec3 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    v.z = v.z > 0.0f ? v.z : 0.0f;

    v.x = v.x < 1.0f ? v.x : 1.0f;
    v.y = v.y < 1.0f ? v.y : 1.0f;
    v.z = v.z < 1.0f ? v.z : 1.0f;

    return v;
}

INLINE vec3 clampVec3To(vec3 v, const f32 min_value, const f32 max_value) {
    v.x = v.x > min_value ? v.x : min_value;
    v.y = v.y > min_value ? v.y : min_value;
    v.z = v.z > min_value ? v.z : min_value;

    v.x = v.x < max_value ? v.x : max_value;
    v.y = v.y < max_value ? v.y : max_value;
    v.z = v.z < max_value ? v.z : max_value;

    return v;
}

INLINE vec3 getVec3Of(f32 value) {
    vec3 out;

    out.x = out.y = out.z = value;

    return out;
}

INLINE vec3 invertedVec3(vec3 in) {
    vec3 out;

    out.x = -in.x;
    out.y = -in.y;
    out.z = -in.z;

    return out;
}

INLINE vec3 oneOverVec3(vec3 v) {
    vec3 out;

    out.x = 1.0f / v.x;
    out.y = 1.0f / v.y;
    out.z = 1.0f / v.z;

    return out;
}

INLINE vec3 approachVec3(vec3 src, vec3 trg, f32 diff) {
    vec3 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);
    out.z = approach(src.z, trg.z, diff);

    return out;
}

INLINE bool nonZeroVec3(vec3 v) {
    return v.x != 0 ||
           v.y != 0 ||
           v.z != 0;
}

INLINE vec3 minVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x < b.x ? a.x : b.x;
    out.y = a.y < b.y ? a.y : b.y;
    out.z = a.z < b.z ? a.z : b.z;

    return out;
}

INLINE vec3 maxVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x > b.x ? a.x : b.x;
    out.y = a.y > b.y ? a.y : b.y;
    out.z = a.z > b.z ? a.z : b.z;

    return out;
}

INLINE f32 minCoordVec3(vec3 v) {
    f32 out = v.x;
    if (v.y < out) out = v.y;
    if (v.z < out) out = v.z;
    return out;
}

INLINE f32 maxCoordVec3(vec3 v) {
    f32 out = v.x;
    if (v.y > out) out = v.y;
    if (v.z > out) out = v.z;
    return out;
}

INLINE vec3 setPointOnUnitSphere(f32 s, f32 t) {
    vec3 out;

    f32 t_squared = t * t;
    f32 s_squared = s * s;
    f32 factor = 1 / ( t_squared + s_squared + 1);

    out.x = 2*s * factor;
    out.y = 2*t * factor;
    out.z = (t_squared + s_squared - 1) * t_squared;

    return out;
}

INLINE vec3 subVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;

    return out;
}

INLINE vec3 addVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;

    return out;
}

INLINE vec3 mulVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;

    return out;
}

INLINE vec3 mulAddVec3(vec3 v, vec3 factors, vec3 to_be_added) {
    vec3 out;

    out.x = fast_mul_add(v.x, factors.x, to_be_added.x);
    out.y = fast_mul_add(v.y, factors.y, to_be_added.y);
    out.z = fast_mul_add(v.z, factors.z, to_be_added.z);

    return out;
}

INLINE vec3 scaleAddVec3(vec3 v, f32 factor, vec3 to_be_added) {
    vec3 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);
    out.z = fast_mul_add(v.z, factor, to_be_added.z);

    return out;
}

INLINE vec3 scaleVec3(vec3 a, f32 factor) {
    vec3 out;

    out.x = a.x * factor;
    out.y = a.y * factor;
    out.z = a.z * factor;

    return out;
}

INLINE vec3 mulVec3Mat3(vec3 in, mat3 m) {
    vec3 out;

    out.x = in.x * m.X.x + in.y * m.Y.x + in.z * m.Z.x;
    out.y = in.x * m.X.y + in.y * m.Y.y + in.z * m.Z.y;
    out.z = in.x * m.X.z + in.y * m.Y.z + in.z * m.Z.z;

    return out;
}

INLINE f32 dotVec3(vec3 a, vec3 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y) +
            (a.z * b.z)
    );
}

INLINE vec3 crossVec3(vec3 a, vec3 b) {
    vec3 out;

    out.x = (a.y * b.z) - (a.z * b.y);
    out.y = (a.z * b.x) - (a.x * b.z);
    out.z = (a.x * b.y) - (a.y * b.x);

    return out;
}

INLINE f32 squaredLengthVec3(vec3 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y) +
            (v.z * v.z)
    );
}

INLINE f32 lengthVec3(vec3 v) {
    return sqrtf(squaredLengthVec3(v));
}

INLINE vec3 normVec3(vec3 v) {
    return scaleVec3(v, 1.0f / lengthVec3(v));
}

INLINE f32 DotVec3(vec3 a, vec3 b) { return clampValue(dotVec3(a, b)); }

INLINE mat3 outerVec3(vec3 a, vec3 b) {
    mat3 out;

    out.X = scaleVec3(a, b.x);
    out.Y = scaleVec3(a, b.y);
    out.Z = scaleVec3(a, b.z);

    return out;
}

INLINE vec3 reflectVec3(vec3 V, vec3 N) {
    vec3 out = scaleVec3(N, -2 * dotVec3(N, V));
         out = addVec3(out, V);
    return out;
}

INLINE vec3 lerpVec3(vec3 from, vec3 to, f32 by) {
    return scaleAddVec3(subVec3(to, from), by, from);
}