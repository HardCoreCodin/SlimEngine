#pragma once

#include "../core/base.h"
#include "../math/math3D.h"
#include "./xform.h"

#define CAMERA_DEFAULT__FOCAL_LENGTH 2.0f

typedef struct Camera {
    f32 focal_length;
    xform3 transform;
} Camera;

void initCamera(Camera* camera) {
    camera->focal_length = CAMERA_DEFAULT__FOCAL_LENGTH;
    initXform3(&camera->transform);
}