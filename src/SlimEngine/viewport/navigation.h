#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../scene/xform.h"

void zoomCamera(Camera *camera, f32 zoom) {
    f32 new_zoom = camera->zoom + zoom;
    camera->focal_length = new_zoom > 1 ? new_zoom : (new_zoom < -1 ? (-1 / new_zoom) : 1);
    camera->zoom = new_zoom;
}

void dollyCamera(Camera *camera, f32 dolly) {
    vec3 target_position = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    target_position = addVec3(camera->transform.position, target_position);

    // Compute new target distance:
    camera->dolly += dolly;
    camera->target_distance = powf(2, camera->dolly / -200) * CAMERA_DEFAULT__TARGET_DISTANCE;

    // Back-track from target position to new current position:
    camera->transform.position = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = subVec3(target_position,camera->transform.position);
}

void orbitCamera(Camera *camera, f32 azimuth, f32 altitude) {
    // Move the camera forward to the position of it's target:
    vec3 movement = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = addVec3(camera->transform.position, movement);

    // Reorient the camera while it is at the position of it's target:
    rotateXform3(&camera->transform, azimuth, altitude, 0);

    // Back the camera away from it's target position using the updated forward direction:
    movement = scaleVec3(*camera->transform.forward_direction, camera->target_distance);
    camera->transform.position = subVec3(camera->transform.position, movement);
}

void panCamera(Camera *camera, f32 right, f32 up) {
    vec3 up_movement    = scaleVec3(*camera->transform.up_direction, up);
    vec3 right_movement = scaleVec3(*camera->transform.right_direction, right);
    camera->transform.position = addVec3(camera->transform.position, up_movement);
    camera->transform.position = addVec3(camera->transform.position, right_movement);
}

void panViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.pan * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.pan * +(f32)mouse->pos_raw_diff.y;
    panCamera(viewport->camera, x, y);

    viewport->navigation.moved = true;
    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void zoomViewport(Viewport *viewport, Mouse *mouse) {
    f32 z = viewport->navigation.settings.speeds.zoom * mouse->wheel_scroll_amount;
    zoomCamera(viewport->camera, z);

    viewport->navigation.zoomed = true;
    mouse->wheel_scroll_handled = true;
}

void dollyViewport(Viewport *viewport, Mouse *mouse) {
    f32 z = viewport->navigation.settings.speeds.dolly * mouse->wheel_scroll_amount;
    dollyCamera(viewport->camera, z);
    viewport->navigation.moved = true;
    mouse->wheel_scroll_handled = true;
}

void orientViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.orient * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.orient * -(f32)mouse->pos_raw_diff.y;
    rotateXform3(&viewport->camera->transform, x, y, 0);

    mouse->moved = false;
    mouse->raw_movement_handled = true;
    viewport->navigation.turned = true;
}

void orbitViewport(Viewport *viewport, Mouse *mouse) {
    f32 x = viewport->navigation.settings.speeds.orbit * -(f32)mouse->pos_raw_diff.x;
    f32 y = viewport->navigation.settings.speeds.orbit * -(f32)mouse->pos_raw_diff.y;
    orbitCamera(viewport->camera, x, y);

    viewport->navigation.turned = true;
    viewport->navigation.moved = true;
    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void navigateViewport(Viewport *viewport, f32 delta_time) {
    vec3 target_velocity = getVec3Of(0);
    Navigation *navigation = &viewport->navigation;
    Camera *camera = viewport->camera;

    if (navigation->move.right)    target_velocity.x += navigation->settings.max_velocity;
    if (navigation->move.left)     target_velocity.x -= navigation->settings.max_velocity;
    if (navigation->move.up)       target_velocity.y += navigation->settings.max_velocity;
    if (navigation->move.down)     target_velocity.y -= navigation->settings.max_velocity;
    if (navigation->move.forward)  target_velocity.z += navigation->settings.max_velocity;
    if (navigation->move.backward) target_velocity.z -= navigation->settings.max_velocity;
    if (navigation->turn.left) {
        rotateXform3(&camera->transform, delta_time * +navigation->settings.speeds.turn, 0, 0);
        navigation->turned = true;
    }
    if (navigation->turn.right) {
        rotateXform3(&camera->transform, delta_time * -navigation->settings.speeds.turn, 0, 0);
        navigation->turned = true;
    }

    // Update the current velocity:
    f32 velocity_difference = navigation->settings.acceleration * delta_time;
    camera->current_velocity = approachVec3(camera->current_velocity, target_velocity, velocity_difference);

    navigation->moved = nonZeroVec3(camera->current_velocity);
    if (navigation->moved) { // Update the current position:
        vec3 movement = scaleVec3(camera->current_velocity, delta_time);
        movement = mulVec3Mat3(movement, camera->transform.rotation_matrix);
        camera->transform.position = addVec3(camera->transform.position, movement);
    }
}