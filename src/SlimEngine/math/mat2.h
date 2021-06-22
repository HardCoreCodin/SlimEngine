#pragma once

#include "../core/base.h"

INLINE mat2 getMat2Identity() {
    mat2 out;

    out.X.x = 1; out.X.y = 0;
    out.Y.x = 0; out.Y.y = 1;

    return out;
}

INLINE mat2 addMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;

    return out;
}

INLINE mat2 subMat2(mat2 a, mat2 b) {
    mat2 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;

    return out;
}

INLINE mat2 scaleMat2(mat2 m, f32 factor) {
    mat2 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;

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

INLINE void setRotationMat2(f32 roll, mat2* roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}