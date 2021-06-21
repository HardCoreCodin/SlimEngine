#pragma once

#include "../core/base.h"
#include "../math/quat.h"

typedef struct xform3 {
    mat3 matrix,
         yaw_matrix,
         pitch_matrix,
         roll_matrix,
         rotation_matrix,
         rotation_matrix_inverted;
    quat rotation,
         rotation_inverted;
    vec3 position, scale,
         *up_direction,
         *right_direction,
         *forward_direction;
} xform3;

void initXform3(xform3 *xform) {
    mat3 I = getMat3Identity();
    xform->matrix = xform->yaw_matrix = xform->pitch_matrix = xform->roll_matrix = xform->rotation_matrix = xform->rotation_matrix_inverted = I;
    xform->right_direction   = &xform->rotation_matrix.X;
    xform->up_direction      = &xform->rotation_matrix.Y;
    xform->forward_direction = &xform->rotation_matrix.Z;
    xform->scale             = getVec3Of(1);
    xform->position          = getVec3Of(0);
    xform->rotation          = getIdentityQuaternion();
    xform->rotation_inverted = getIdentityQuaternion();
}

INLINE void rotateXform3(xform3 *xform, f32 yaw, f32 pitch, f32 roll) {
    if (yaw)   yawMat3(  yaw,   &xform->yaw_matrix);
    if (pitch) pitchMat3(pitch, &xform->pitch_matrix);
    if (roll)  rollMat3( roll,  &xform->roll_matrix);

    xform->rotation_matrix = mulMat3(mulMat3(xform->pitch_matrix, xform->yaw_matrix), xform->roll_matrix);
    xform->rotation_matrix_inverted = transposedMat3(xform->rotation_matrix);

    xform->rotation          = convertRotationMatrixToQuaternion(xform->rotation_matrix);
    xform->rotation_inverted = convertRotationMatrixToQuaternion(xform->rotation_matrix_inverted);

    xform->matrix = mulMat3(xform->matrix, xform->rotation_matrix);
}