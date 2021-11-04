#pragma once

#include "../core/base.h"

INLINE vec2 getVec2Of(f32 value) {
    vec2 out;

    out.x = out.y = value;

    return out;
}
INLINE bool isEqualVec2(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

INLINE vec2 clampVec2ToZero(vec2 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;
    return v;
}

INLINE vec2 clampVec2ToUpper(vec2 v, vec2 upper) {
    v.x = v.x < upper.x ? v.x : upper.x;
    v.y = v.y < upper.y ? v.y : upper.y;
    return v;
}

INLINE vec2 clampVec2(vec2 v) {
    v.x = v.x > 0.0f ? v.x : 0.0f;
    v.y = v.y > 0.0f ? v.y : 0.0f;

    v.x = v.x < 1.0f ? v.x : 1.0f;
    v.y = v.y < 1.0f ? v.y : 1.0f;

    return v;
}

INLINE vec2 clampVec2To(vec2 v, const f32 min_value, const f32 max_value) {
    v.x = v.x > min_value ? v.x : min_value;
    v.y = v.y > min_value ? v.y : min_value;

    v.x = v.x < max_value ? v.x : max_value;
    v.y = v.y < max_value ? v.y : max_value;

    return v;
}

INLINE vec2 invertedVec2(vec2 in) {
    vec2 out;

    out.x = -in.x;
    out.y = -in.y;

    return out;
}

INLINE vec2 oneOverVec2(vec2 v) {
    vec2 out;

    out.x = 1.0f / v.x;
    out.y = 1.0f / v.y;

    return out;
}

INLINE vec2 approachVec2(vec2 src, vec2 trg, f32 diff) {
    vec2 out;

    out.x = approach(src.x, trg.x, diff);
    out.y = approach(src.y, trg.y, diff);

    return out;
}

INLINE bool nonZeroVec2(vec2 v) {
    return v.x != 0 ||
           v.y != 0;
}

INLINE vec2 minVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x < b.x ? a.x : b.x;
    out.y = a.y < b.y ? a.y : b.y;

    return out;
}

INLINE vec2 maxVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x > b.x ? a.x : b.x;
    out.y = a.y > b.y ? a.y : b.y;

    return out;
}

INLINE f32 minCoordVec2(vec2 v) {
    f32 out = v.x;
    if (v.y < out) out = v.y;
    return out;
}

INLINE f32 maxCoordVec2(vec2 v) {
    f32 out = v.x;
    if (v.y > out) out = v.y;
    return out;
}

INLINE vec2 subVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x - b.x;
    out.y = a.y - b.y;

    return out;
}

INLINE vec2 addVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x + b.x;
    out.y = a.y + b.y;

    return out;
}

INLINE vec2 mulVec2(vec2 a, vec2 b) {
    vec2 out;

    out.x = a.x * b.x;
    out.y = a.y * b.y;

    return out;
}

INLINE vec2 mulAddVec2(vec2 v, vec2 factors, vec2 to_be_added) {
    vec2 out;

    out.x = fast_mul_add(v.x, factors.x, to_be_added.x);
    out.y = fast_mul_add(v.y, factors.y, to_be_added.y);

    return out;
}

INLINE vec2 scaleAddVec2(vec2 v, f32 factor, vec2 to_be_added) {
    vec2 out;

    out.x = fast_mul_add(v.x, factor, to_be_added.x);
    out.y = fast_mul_add(v.y, factor, to_be_added.y);

    return out;
}

INLINE vec2 scaleVec2(vec2 a, f32 factor) {
    vec2 out;

    out.x = a.x * factor;
    out.y = a.y * factor;

    return out;
}

INLINE vec2 mulVec2Mat2(vec2 in, mat2 m) {
    vec2 out;

    out.x = in.x * m.X.x + in.y * m.Y.x;
    out.y = in.x * m.X.y + in.y * m.Y.y;

    return out;
}

INLINE f32 dotVec2(vec2 a, vec2 b) {
    return (
            (a.x * b.x) +
            (a.y * b.y)
    );
}

INLINE f32 squaredLengthVec2(vec2 v) {
    return (
            (v.x * v.x) +
            (v.y * v.y)
    );
}

INLINE f32 lengthVec2(vec2 v) {
    return sqrtf(squaredLengthVec2(v));
}

INLINE vec2 normVec2(vec2 v) {
    return scaleVec2(v, 1.0f / lengthVec2(v));
}

INLINE f32 DotVec2(vec2 a, vec2 b) { return clampValue(dotVec2(a, b)); }

INLINE mat2 outerVec2(vec2 a, vec2 b) {
    mat2 out;

    out.X = scaleVec2(a, b.x);
    out.Y = scaleVec2(a, b.y);

    return out;
}

INLINE vec2 reflectVec2(vec2 V, vec2 N) {
    vec2 out = scaleVec2(N, -2 * dotVec2(N, V));
         out = addVec2(out, V);
    return out;
}