#pragma once

#include "../core/base.h"

INLINE mat3 getMat3Identity() {
    mat3 out;

    out.X.x = 1; out.X.y = 0; out.X.z = 0;
    out.Y.x = 0; out.Y.y = 1; out.Y.z = 0;
    out.Z.x = 0; out.Z.y = 0; out.Z.z = 1;

    return out;
}

INLINE mat3 addMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;
    out.X.z = a.X.z + b.X.z;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;
    out.Y.z = a.Y.z + b.Y.z;

    out.Z.x = a.Z.x + b.Z.x;
    out.Z.y = a.Z.y + b.Z.y;
    out.Z.z = a.Z.x + b.Z.x;

    return out;
}

INLINE mat3 subMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;
    out.X.z = a.X.z - b.X.z;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;
    out.Y.z = a.Y.z - b.Y.z;

    out.Z.x = a.Z.x - b.Z.x;
    out.Z.y = a.Z.y - b.Z.y;
    out.Z.z = a.Z.x - b.Z.x;

    return out;
}

INLINE mat3 scaleMat3(mat3 m, f32 factor) {
    mat3 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;
    out.X.z = m.X.z * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;
    out.Y.z = m.Y.z * factor;

    out.Z.x = m.Z.x * factor;
    out.Z.y = m.Z.y * factor;
    out.Z.z = m.Z.z * factor;

    return out;
}

INLINE mat3 transposedMat3(mat3 m) {
    mat3 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;  out.X.z = m.Z.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;  out.Y.z = m.Z.y;
    out.Z.x = m.X.z;  out.Z.y = m.Y.z;  out.Z.z = m.Z.z;

    return out;
}

INLINE mat3 mulMat3(mat3 a, mat3 b) {
    mat3 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x + a.X.z*b.Z.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y + a.X.z*b.Z.y; // Row 1 | Column 2
    out.X.z = a.X.x*b.X.z + a.X.y*b.Y.z + a.X.z*b.Z.z; // Row 1 | Column 3

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x + a.Y.z*b.Z.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y + a.Y.z*b.Z.y; // Row 2 | Column 2
    out.Y.z = a.Y.x*b.X.z + a.Y.y*b.Y.z + a.Y.z*b.Z.z; // Row 2 | Column 3

    out.Z.x = a.Z.x*b.X.x + a.Z.y*b.Y.x + a.Z.z*b.Z.x; // Row 3 | Column 1
    out.Z.y = a.Z.x*b.X.y + a.Z.y*b.Y.y + a.Z.z*b.Z.y; // Row 3 | Column 2
    out.Z.z = a.Z.x*b.X.z + a.Z.y*b.Y.z + a.Z.z*b.Z.z; // Row 3 | Column 3

    return out;
}

INLINE mat3 invMat3(mat3 m) {
    mat3 out;

    f32 one_over_determinant = 1.0f / (
        + m.X.x * (m.Y.y * m.Z.z - m.Z.y * m.Y.z)
        - m.Y.x * (m.X.y * m.Z.z - m.Z.y * m.X.z)
        + m.Z.x * (m.X.y * m.Y.z - m.Y.y * m.X.z)
    );

    out.X.x = + (m.Y.y * m.Z.z - m.Z.y * m.Y.z) * one_over_determinant;
    out.Y.x = - (m.Y.x * m.Z.z - m.Z.x * m.Y.z) * one_over_determinant;
    out.Z.x = + (m.Y.x * m.Z.y - m.Z.x * m.Y.y) * one_over_determinant;
    out.X.y = - (m.X.y * m.Z.z - m.Z.y * m.X.z) * one_over_determinant;
    out.Y.y = + (m.X.x * m.Z.z - m.Z.x * m.X.z) * one_over_determinant;
    out.Z.y = - (m.X.x * m.Z.y - m.Z.x * m.X.y) * one_over_determinant;
    out.X.z = + (m.X.y * m.Y.z - m.Y.y * m.X.z) * one_over_determinant;
    out.Y.z = - (m.X.x * m.Y.z - m.Y.x * m.X.z) * one_over_determinant;
    out.Z.z = + (m.X.x * m.Y.y - m.Y.x * m.X.y) * one_over_determinant;

    return out;
}

INLINE bool safeInvertMat3(mat3 *m) {
    f32 m11 = m->X.x,  m12 = m->X.y,  m13 = m->X.z,
        m21 = m->Y.x,  m22 = m->Y.y,  m23 = m->Y.z,
        m31 = m->Z.x,  m32 = m->Z.y,  m33 = m->Z.z,

        c11 = m22*m33 -
              m23*m32,

        c12 = m13*m32 -
              m12*m33,

        c13 = m12*m23 -
              m13*m22,


        c21 = m23*m31 -
              m21*m33,

        c22 = m11*m33 -
              m13*m31,

        c23 = m13*m21 -
              m11*m23,


        c31 = m21*m32 -
              m22*m31,

        c32 = m12*m31 -
              m11*m32,

        c33 = m11*m22 -
              m12*m21,

        d = c11 + c12 + c13 +
            c21 + c22 + c23 +
            c31 + c32 + c33;

    if (!d) return false;

    d = 1 / d;

    m->X.x = d * c11;  m->X.y = d * c12;  m->X.z = d * c13;
    m->Y.x = d * c21;  m->Y.y = d * c22;  m->Y.z = d * c23;
    m->Z.x = d * c31;  m->Z.y = d * c32;  m->Z.z = d * c33;

    return true;
}

INLINE void yawMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.x = xy.x * X.x - xy.y * X.z;
    out->Y.x = xy.x * Y.x - xy.y * Y.z;
    out->Z.x = xy.x * Z.x - xy.y * Z.z;

    out->X.z = xy.x * X.z + xy.y * X.x;
    out->Y.z = xy.x * Y.z + xy.y * Y.x;
    out->Z.z = xy.x * Z.z + xy.y * Z.x;
}

INLINE void pitchMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.y = xy.x * X.y + xy.y * X.z;
    out->Y.y = xy.x * Y.y + xy.y * Y.z;
    out->Z.y = xy.x * Z.y + xy.y * Z.z;

    out->X.z = xy.x * X.z - xy.y * X.y;
    out->Y.z = xy.x * Y.z - xy.y * Y.y;
    out->Z.z = xy.x * Z.z - xy.y * Z.y;
}

INLINE void rollMat3(f32 amount, mat3* out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec3 X = out->X;
    vec3 Y = out->Y;
    vec3 Z = out->Z;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;
    out->Z.x = xy.x * Z.x + xy.y * Z.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
    out->Z.y = xy.x * Z.y - xy.y * Z.x;
}

INLINE void setYawMat3(f32 yaw, mat3* yaw_matrix) {
    vec2 xy = getPointOnUnitCircle(yaw);

    yaw_matrix->X.x = yaw_matrix->Z.z = xy.x;
    yaw_matrix->X.z = +xy.y;
    yaw_matrix->Z.x = -xy.y;
}

INLINE void setPitchMat3(f32 pitch, mat3* pitch_matrix) {
    vec2 xy = getPointOnUnitCircle(pitch);

    pitch_matrix->Z.z = pitch_matrix->Y.y = xy.x;
    pitch_matrix->Y.z = -xy.y;
    pitch_matrix->Z.y = +xy.y;
}

INLINE void setRollMat3(f32 roll, mat3* roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}