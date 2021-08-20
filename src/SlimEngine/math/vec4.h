#pragma once

#include "../core/base.h"

INLINE vec4 getVec4Of(f32 value) {
    vec4 out;

    out.x = out.y = out.z = out.w = value;

    return out;
}

INLINE vec4 invertedVec4(vec4 in) {
    vec4 out;

    out.x = -in.x;
    out.y = -in.y;
    out.z = -in.z;
    out.w = -in.w;

    return out;
}

INLINE vec4 approachVec4(vec4 src, vec4 trg, f32 diff) {
    vec4 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);
    out.z = approach(src.z, trg.z, diff);
    out.w = approach(src.w, trg.w, diff);

    return out;
}

INLINE bool nonZeroVec4(vec4 v) {
    return v.x != 0 ||
           v.y != 0 ||
           v.z != 0 ||
           v.w != 0;
}

INLINE vec4 subVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    out.w = a.w - b.w;

    return out;
}

INLINE vec4 addVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    out.w = a.w + b.w;

    return out;
}

INLINE vec4 mulVec4(vec4 a, vec4 b) {
    vec4 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;
    out.w = a.w * b.w;

    return out;
}

INLINE vec4 scaleVec4(vec4 a, f32 factor) {
    vec4 out;

    out.x = a.x * factor;
    out.y = a.y * factor;
    out.z = a.z * factor;
    out.w = a.w * factor;

    return out;
}
INLINE vec4 scaleAddVec4(vec4 v, f32 factor, vec4 to_be_added) {
    vec4 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);
    out.z = fast_mul_add(v.z, factor, to_be_added.z);
    out.w = fast_mul_add(v.w, factor, to_be_added.w);

    return out;
}

INLINE vec4 mulVec4Mat4(vec4 in, mat4 m) {
    vec4 out;

    out.x = in.x * m.X.x + in.y * m.Y.x + in.z * m.Z.x + in.w * m.W.x;
    out.y = in.x * m.X.y + in.y * m.Y.y + in.z * m.Z.y + in.w * m.W.y;
    out.z = in.x * m.X.z + in.y * m.Y.z + in.z * m.Z.z + in.w * m.W.z;
    out.w = in.x * m.X.w + in.y * m.Y.w + in.z * m.Z.w + in.w * m.W.w;

    return out;
}

INLINE f32 dotVec4(vec4 a, vec4 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y) +
            (a.z * b.z) +
            (a.w * b.w)
    );
}

INLINE f32 squaredLengthVec4(vec4 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y) +
            (v.z * v.z) +
            (v.w * v.w)
    );
}

INLINE f32 lengthVec4(vec4 v) {
    return sqrtf(squaredLengthVec4(v));
}

INLINE vec4 norm4(vec4 v) {
    return scaleVec4(v, 1.0f / lengthVec4(v));
}

INLINE f32 mulVec3Mat4(vec3 in, f32 w, mat4 M, vec3 *out) {
    vec4 v4 = mulVec4Mat4(Vec4fromVec3(in, w), M);
    *out = Vec3fromVec4(v4);
    return v4.w;
}

INLINE vec4 lerpVec4(vec4 from, vec4 to, f32 by) {
    return scaleAddVec4(subVec4(to, from), by, from);
}