#pragma once

#include "../core/base.h"

INLINE mat2 getMat2Identity() {
    mat2 out;

    out.X.x = 1; out.X.y = 0;
    out.Y.x = 0; out.Y.y = 1;

    return out;
}

INLINE vec2 getVec2Of(f32 value) {
    vec2 out;

    out.x = out.y = value;

    return out;
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

INLINE mat2 addMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X = addVec2(a.X, b.X);
    out.Y = addVec2(a.Y, b.Y);

    return out;
}

INLINE mat2 scaleMat2(mat2 m, f32 factor) {
    mat2 out;

    out.X = scaleVec2(m.X, factor);
    out.Y = scaleVec2(m.Y, factor);

    return out;
}

INLINE mat2 transposedMat2(mat2 m) {
    mat2 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;

    return out;
}

INLINE mat2 mulMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y; // Row 1 | Column 2

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y; // Row 2 | Column 2

    return out;
}

INLINE mat2 invMat2(mat2 m) {
    mat2 out;

    f32 a = m.X.x,  b = m.X.y,
        c = m.Y.x,  d = m.Y.y;

    f32 det = a*d - b*c;
    f32 one_over_det = 1.0f / det;

    out.X.x = +d * one_over_det;
    out.X.y = -b * one_over_det;
    out.Y.x = -c * one_over_det;
    out.Y.y = +a * one_over_det;

    return out;
}

INLINE bool safeInvertMat2(mat2 *m) {
    f32 a = m->X.x,  b = m->X.y,
        c = m->Y.x,  d = m->Y.y;

    f32 det = a*d - b*c;
    if (!det) return false;
    f32 one_over_det = 1.0f / det;

    m->X.x = +d * one_over_det;
    m->X.y = -b * one_over_det;
    m->Y.x = -c * one_over_det;
    m->Y.y = +a * one_over_det;

    return true;
}

INLINE void rotateMat2(f32 amount, mat2* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec2 X = out->X;
    vec2 Y = out->Y;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
}

INLINE void reciprocateVec2(vec2 *orig, vec2 *rcp) {
    rcp->x = 1.0f / orig->x;
    rcp->y = 1.0f / orig->y;
}

INLINE void setRotationMat2(f32 roll, mat2* roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}