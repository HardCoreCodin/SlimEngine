#pragma once

#include "../core/base.h"
#include "./vec3.h"

INLINE quat getIdentityQuaternion() {
    quat out;

    out.axis = getVec3Of(0);
    out.amount = 1;

    return out;
}

INLINE quat normQuat(quat q) {
    quat out;

    f32 factor = 1.0f / sqrtf(q.axis.x * q.axis.x + q.axis.y * q.axis.y + q.axis.z * q.axis.z + q.amount * q.amount);
    out.axis = scaleVec3(q.axis, factor);
    out.amount = q.amount * factor;

    return out;
}

INLINE vec3 mulVec3Quat(const vec3 v, quat q) {
    vec3 out = crossVec3(q.axis, v);
    vec3 qqv = crossVec3(q.axis, out);
    out = scaleAddVec3(out, q.amount, qqv);
    out = scaleAddVec3(out, 2, v);
    return out;
}

INLINE quat mulQuat(quat a, quat b) {
    quat out;

    out.amount = a.amount * b.amount - a.axis.x * b.axis.x - a.axis.y * b.axis.y - a.axis.z * b.axis.z;
    out.axis.x = a.amount * b.axis.x + a.axis.x * b.amount + a.axis.y * b.axis.z - a.axis.z * b.axis.y;
    out.axis.y = a.amount * b.axis.y - a.axis.x * b.axis.z + a.axis.y * b.amount + a.axis.z * b.axis.x;
    out.axis.z = a.amount * b.axis.z + a.axis.x * b.axis.y - a.axis.y * b.axis.x + a.axis.z * b.amount;

    return out;
}

INLINE quat conjugate(quat q) {
    quat out;

    out.amount = q.amount;
    out.axis = invertedVec3(q.axis);

    return out;
}

INLINE quat convertRotationMatrixToQuaternion(mat3 rotation_matrix) {
    quat out;

    f32 fourXSquaredMinus1 = rotation_matrix.X.x - rotation_matrix.Y.y - rotation_matrix.Z.z;
    f32 fourYSquaredMinus1 = rotation_matrix.Y.y - rotation_matrix.X.x - rotation_matrix.Z.z;
    f32 fourZSquaredMinus1 = rotation_matrix.Z.z - rotation_matrix.X.x - rotation_matrix.Y.y;
    f32 fourWSquaredMinus1 = rotation_matrix.X.x + rotation_matrix.Y.y + rotation_matrix.Z.z;

    int biggestIndex = 0;
    f32 fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
    }
    if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
    }
    if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
    }

    f32 biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
    f32 mult = 0.25f / biggestVal;

    switch(biggestIndex) {
        case 0:
            out.amount = biggestVal;
            out.axis.x = (rotation_matrix.Y.z - rotation_matrix.Z.y) * mult;
            out.axis.y = (rotation_matrix.Z.x - rotation_matrix.X.z) * mult;
            out.axis.z = (rotation_matrix.X.y - rotation_matrix.Y.x) * mult;
            break;
        case 1:
            out.amount = (rotation_matrix.Y.z - rotation_matrix.Z.y) * mult;
            out.axis.x = biggestVal;
            out.axis.y = (rotation_matrix.X.y + rotation_matrix.Y.x) * mult;
            out.axis.z = (rotation_matrix.Z.x + rotation_matrix.X.z) * mult;
            break;
        case 2:
            out.amount = (rotation_matrix.Z.x - rotation_matrix.X.z) * mult;
            out.axis.x = (rotation_matrix.X.y + rotation_matrix.Y.x) * mult;
            out.axis.y = biggestVal;
            out.axis.z = (rotation_matrix.Y.z + rotation_matrix.Z.y) * mult;
            break;
        case 3:
            out.amount = (rotation_matrix.X.y - rotation_matrix.Y.x) * mult;
            out.axis.x = (rotation_matrix.Z.x + rotation_matrix.X.z) * mult;
            out.axis.y = (rotation_matrix.Y.z + rotation_matrix.Z.y) * mult;
            out.axis.z = biggestVal;
            break;
    }

    return out;
}

INLINE mat3 convertQuaternionToRotationMatrix(quat q) {
    mat3 out;

    f32 q0 = q.amount;
    f32 q1 = q.axis.x;
    f32 q2 = q.axis.y;
    f32 q3 = q.axis.z;

    out.X.x = 2 * (q0 * q0 + q1 * q1) - 1;
    out.X.y = 2 * (q1 * q2 - q0 * q3);
    out.X.z = 2 * (q1 * q3 + q0 * q2);

    out.Y.x = 2 * (q1 * q2 + q0 * q3);
    out.Y.y = 2 * (q0 * q0 + q2 * q2) - 1;
    out.Y.z = 2 * (q2 * q3 - q0 * q1);

    out.Z.x = 2 * (q1 * q3 - q0 * q2);
    out.Z.y = 2 * (q2 * q3 + q0 * q1);
    out.Z.z = 2 * (q0 * q0 + q3 * q3) - 1;

    return out;
}

INLINE quat getRotationAroundAxis(vec3 axis, f32 amount) {
    vec2 sin_cos = getPointOnUnitCircle(amount);
    quat out;
    out.axis = scaleVec3(axis, sin_cos.y);
    out.amount = sin_cos.x;

    return normQuat(out);
}

INLINE quat getRotationAroundAxisBySinCon(vec3 axis, vec2 sin_cos) {
    quat out;
    out.axis = scaleVec3(axis, sin_cos.y);
    out.amount = sin_cos.x;

    return normQuat(out);
}

INLINE quat rotateAroundAxisBySinCos(quat q, vec3 axis, vec2 sin_cos) {
    quat rotation = getRotationAroundAxisBySinCon(axis, sin_cos);
    return mulQuat(q, rotation);
}

INLINE quat rotateAroundAxis(quat q, vec3 axis, f32 amount) {
    quat rotation = getRotationAroundAxis(axis, amount);
    return mulQuat(q, rotation);
}