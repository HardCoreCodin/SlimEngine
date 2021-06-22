#pragma once

#include "../core/base.h"
#include "../math/vec3.h"
#include "../math/mat3.h"
#include "../math/quat.h"

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