#pragma once

#include "../core/base.h"

INLINE mat4 getMat4Identity() {
    mat4 out;

    out.X.x = 1; out.X.y = 0; out.X.z = 0; out.X.w = 0;
    out.Y.x = 0; out.Y.y = 1; out.Y.z = 0; out.Y.w = 0;
    out.Z.x = 0; out.Z.y = 0; out.Z.z = 1; out.Z.w = 0;
    out.W.x = 0; out.W.y = 0; out.W.z = 0; out.W.w = 1;

    return out;
}

INLINE mat4 transposeMat4(mat4 m) {
    mat4 out;

    out.X.x = m.X.x;  out.X.y = m.Y.x;  out.X.z = m.Z.x;  out.X.w = m.W.x;
    out.Y.x = m.X.y;  out.Y.y = m.Y.y;  out.Y.z = m.Z.y;  out.Y.w = m.W.y;
    out.Z.x = m.X.z;  out.Z.y = m.Y.z;  out.Z.z = m.Z.z;  out.Z.w = m.W.z;
    out.W.x = m.X.w;  out.W.y = m.Y.w;  out.W.z = m.Z.w;  out.W.w = m.W.w;

    return out;
}

INLINE mat4 addMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x + b.X.x;
    out.X.y = a.X.y + b.X.y;
    out.X.z = a.X.z + b.X.z;
    out.X.w = a.X.w + b.X.w;

    out.Y.x = a.Y.x + b.Y.x;
    out.Y.y = a.Y.y + b.Y.y;
    out.Y.z = a.Y.z + b.Y.z;
    out.Y.w = a.Y.w + b.Y.w;

    out.Z.x = a.Z.x + b.Z.x;
    out.Z.y = a.Z.y + b.Z.y;
    out.Z.z = a.Z.x + b.Z.x;
    out.Z.w = a.Z.w + b.Z.w;

    out.W.x = a.W.x + b.W.x;
    out.W.y = a.W.y + b.W.y;
    out.W.z = a.W.x + b.W.x;
    out.W.w = a.W.w + b.W.w;

    return out;
}

INLINE mat4 subMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x - b.X.x;
    out.X.y = a.X.y - b.X.y;
    out.X.z = a.X.z - b.X.z;
    out.X.w = a.X.w - b.X.w;

    out.Y.x = a.Y.x - b.Y.x;
    out.Y.y = a.Y.y - b.Y.y;
    out.Y.z = a.Y.z - b.Y.z;
    out.Y.w = a.Y.w - b.Y.w;

    out.Z.x = a.Z.x - b.Z.x;
    out.Z.y = a.Z.y - b.Z.y;
    out.Z.z = a.Z.x - b.Z.x;
    out.Z.w = a.Z.w - b.Z.w;

    out.W.x = a.W.x - b.W.x;
    out.W.y = a.W.y - b.W.y;
    out.W.z = a.W.x - b.W.x;
    out.W.w = a.W.w - b.W.w;

    return out;
}

INLINE mat4 scaleMat4(mat4 m, f32 factor) {
    mat4 out;

    out.X.x = m.X.x * factor;
    out.X.y = m.X.y * factor;
    out.X.z = m.X.z * factor;
    out.X.w = m.X.w * factor;

    out.Y.x = m.Y.x * factor;
    out.Y.y = m.Y.y * factor;
    out.Y.z = m.Y.z * factor;
    out.Y.w = m.Y.w * factor;

    out.Z.x = m.Z.x * factor;
    out.Z.y = m.Z.y * factor;
    out.Z.z = m.Z.z * factor;
    out.Z.w = m.Z.w * factor;

    out.W.x = m.W.x * factor;
    out.W.y = m.W.y * factor;
    out.W.z = m.W.z * factor;
    out.W.w = m.W.w * factor;

    return out;
}

INLINE mat4 mulMat4(mat4 a, mat4 b) {
    mat4 out;

    out.X.x = a.X.x*b.X.x + a.X.y*b.Y.x + a.X.z*b.Z.x + a.X.w*b.W.x; // Row 1 | Column 1
    out.X.y = a.X.x*b.X.y + a.X.y*b.Y.y + a.X.z*b.Z.y + a.X.w*b.W.y; // Row 1 | Column 2
    out.X.z = a.X.x*b.X.z + a.X.y*b.Y.z + a.X.z*b.Z.z + a.X.w*b.W.z; // Row 1 | Column 3
    out.X.w = a.X.x*b.X.w + a.X.y*b.Y.w + a.X.z*b.Z.w + a.X.w*b.W.w; // Row 1 | Column 4

    out.Y.x = a.Y.x*b.X.x + a.Y.y*b.Y.x + a.Y.z*b.Z.x + a.Y.w*b.W.x; // Row 2 | Column 1
    out.Y.y = a.Y.x*b.X.y + a.Y.y*b.Y.y + a.Y.z*b.Z.y + a.Y.w*b.W.y; // Row 2 | Column 2
    out.Y.z = a.Y.x*b.X.z + a.Y.y*b.Y.z + a.Y.z*b.Z.z + a.Y.w*b.W.z; // Row 2 | Column 3
    out.Y.w = a.Y.x*b.X.w + a.Y.y*b.Y.w + a.Y.z*b.Z.w + a.Y.w*b.W.w; // Row 2 | Column 4

    out.Z.x = a.Z.x*b.X.x + a.Z.y*b.Y.x + a.Z.z*b.Z.x + a.Z.w*b.W.x; // Row 3 | Column 1
    out.Z.y = a.Z.x*b.X.y + a.Z.y*b.Y.y + a.Z.z*b.Z.y + a.Z.w*b.W.y; // Row 3 | Column 2
    out.Z.z = a.Z.x*b.X.z + a.Z.y*b.Y.z + a.Z.z*b.Z.z + a.Z.w*b.W.z; // Row 3 | Column 3
    out.Z.w = a.Z.x*b.X.w + a.Z.y*b.Y.w + a.Z.z*b.Z.w + a.Z.w*b.W.w; // Row 3 | Column 4

    out.W.x = a.W.x*b.X.x + a.W.y*b.Y.x + a.W.z*b.Z.x + a.W.w*b.W.x; // Row 4 | Column 1
    out.W.y = a.W.x*b.X.y + a.W.y*b.Y.y + a.W.z*b.Z.y + a.W.w*b.W.y; // Row 4 | Column 2
    out.W.z = a.W.x*b.X.z + a.W.y*b.Y.z + a.W.z*b.Z.z + a.W.w*b.W.z; // Row 4 | Column 3
    out.W.w = a.W.x*b.X.w + a.W.y*b.Y.w + a.W.z*b.Z.w + a.W.w*b.W.w; // Row 4 | Column 4

    return out;
}

INLINE mat4 invMat4(mat4 m) {
    mat4 out;

    f32 m11 = m.X.x,  m12 = m.X.y,  m13 = m.X.z, m14 = m.X.w,
        m21 = m.Y.x,  m22 = m.Y.y,  m23 = m.Y.z, m24 = m.Y.w,
        m31 = m.Z.x,  m32 = m.Z.y,  m33 = m.Z.z, m34 = m.Z.w,
        m41 = m.W.x,  m42 = m.W.y,  m43 = m.W.z, m44 = m.W.w;

    out.X.x = +m22*m33*m44 - m22*m34*m43 - m32*m23*m44 + m32*m24*m43 + m42*m23*m34 - m42*m24*m33;
    out.X.y = -m12*m33*m44 + m12*m34*m43 + m32*m13*m44 - m32*m14*m43 - m42*m13*m34 + m42*m14*m33;
    out.X.z = +m12*m23*m44 - m12*m24*m43 - m22*m13*m44 + m22*m14*m43 + m42*m13*m24 - m42*m14*m23;
    out.X.w = -m12*m23*m34 + m12*m24*m33 + m22*m13*m34 - m22*m14*m33 - m32*m13*m24 + m32*m14*m23;

    out.Y.x = -m21*m33*m44 + m21*m34*m43 + m31*m23*m44 - m31*m24*m43 - m41*m23*m34 + m41*m24*m33;
    out.Y.y = +m11*m33*m44 - m11*m34*m43 - m31*m13*m44 + m31*m14*m43 + m41*m13*m34 - m41*m14*m33;
    out.Y.z = -m11*m23*m44 + m11*m24*m43 + m21*m13*m44 - m21*m14*m43 - m41*m13*m24 + m41*m14*m23;
    out.Y.w = +m11*m23*m34 - m11*m24*m33 - m21*m13*m34 + m21*m14*m33 + m31*m13*m24 - m31*m14*m23;

    out.Z.x = +m21*m32*m44 - m21*m34*m42 - m31*m22*m44 + m31*m24*m42 + m41*m22*m34 - m41*m24*m32;
    out.Z.y = -m11*m32*m44 + m11*m34*m42 + m31*m12*m44 - m31*m14*m42 - m41*m12*m34 + m41*m14*m32;
    out.Z.z = +m11*m22*m44 - m11*m24*m42 - m21*m12*m44 + m21*m14*m42 + m41*m12*m24 - m41*m14*m22;
    out.Z.w = -m11*m22*m34 + m11*m24*m32 + m21*m12*m34 - m21*m14*m32 - m31*m12*m24 + m31*m14*m22;

    out.W.x = -m21*m32*m43 + m21*m33*m42 + m31*m22*m43 - m31*m23*m42 - m41*m22*m33 + m41*m23*m32;
    out.W.y = +m11*m32*m43 - m11*m33*m42 - m31*m12*m43 + m31*m13*m42 + m41*m12*m33 - m41*m13*m32;
    out.W.z = -m11*m22*m43 + m11*m23*m42 + m21*m12*m43 - m21*m13*m42 - m41*m12*m23 + m41*m13*m22;
    out.W.w = +m11*m22*m33 - m11*m23*m32 - m21*m12*m33 + m21*m13*m32 + m31*m12*m23 - m31*m13*m22;

    f32 det = m11*out.X.x + m12*out.Y.x + m13*out.Z.x + m14*out.W.x;
    if (!det) return m;

    out = scaleMat4(out, 1.0f / det);

    return out;
}

INLINE void yawMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.x = xy.x * X.x - xy.y * X.z;
    out->Y.x = xy.x * Y.x - xy.y * Y.z;
    out->Z.x = xy.x * Z.x - xy.y * Z.z;

    out->X.z = xy.x * X.z + xy.y * X.x;
    out->Y.z = xy.x * Y.z + xy.y * Y.x;
    out->Z.z = xy.x * Z.z + xy.y * Z.x;
}

INLINE void pitchMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.y = xy.x * X.y + xy.y * X.z;
    out->Y.y = xy.x * Y.y + xy.y * Y.z;
    out->Z.y = xy.x * Z.y + xy.y * Z.z;

    out->X.z = xy.x * X.z - xy.y * X.y;
    out->Y.z = xy.x * Y.z - xy.y * Y.y;
    out->Z.z = xy.x * Z.z - xy.y * Z.y;
}

INLINE void rollMat4(f32 amount, mat4 *out) {
    vec2 xy = getPointOnUnitCircle(amount);

    vec4 X = out->X;
    vec4 Y = out->Y;
    vec4 Z = out->Z;

    out->X.x = xy.x * X.x + xy.y * X.y;
    out->Y.x = xy.x * Y.x + xy.y * Y.y;
    out->Z.x = xy.x * Z.x + xy.y * Z.y;

    out->X.y = xy.x * X.y - xy.y * X.x;
    out->Y.y = xy.x * Y.y - xy.y * Y.x;
    out->Z.y = xy.x * Z.y - xy.y * Z.x;
}

INLINE void setYawMat4(f32 yaw, mat4 *yaw_matrix) {
    vec2 xy = getPointOnUnitCircle(yaw);

    yaw_matrix->X.x = yaw_matrix->Z.z = xy.x;
    yaw_matrix->X.z = +xy.y;
    yaw_matrix->Z.x = -xy.y;
}

INLINE void setPitchMat4(f32 pitch, mat4 *pitch_matrix) {
    vec2 xy = getPointOnUnitCircle(pitch);

    pitch_matrix->Z.z = pitch_matrix->Y.y = xy.x;
    pitch_matrix->Y.z = -xy.y;
    pitch_matrix->Z.y = +xy.y;
}

INLINE void setRollMat4(f32 roll, mat4 *roll_matrix) {
    vec2 xy = getPointOnUnitCircle(roll);

    roll_matrix->X.x = roll_matrix->Y.y = xy.x;
    roll_matrix->X.y = -xy.y;
    roll_matrix->Y.x = +xy.y;
}

INLINE mat4 mat4fromMat3(mat3 m3) {
    mat4 out = getMat4Identity();
    for (u8 row = 0; row < 3; row++)
        for (u8 col = 0; col < 3; col++)
            out.axis[row].components[col] = m3.axis[row].components[col];

    return out;
}