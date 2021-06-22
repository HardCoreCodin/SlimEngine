#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../scene/xform.h"

void zoomCamera(Camera *camera, Navigation *navigation, f32 zoom) {
    f32 new_zoom = navigation->zoom + zoom;
    camera->focal_length = new_zoom > 1 ? new_zoom : (new_zoom < -1 ? (-1 / new_zoom) : 1);
    navigation->zoom = new_zoom;
    navigation->zoomed = true;
}

void dollyCamera(Camera *camera, Navigation *navigation, f32 dolly, f32 max_distance) {
    vec3 target_position = scaleVec3(*camera->transform.forward_direction, navigation->target_distance);
    target_position = addVec3(camera->transform.position, target_position);

    // Compute new target distance:
    navigation->dolly += dolly;
    navigation->target_distance = powf(2, navigation->dolly / -200) * max_distance;

    // Back-track from target position to new current position:
    camera->transform.position = scaleVec3(*camera->transform.forward_direction, navigation->target_distance);
    camera->transform.position = subVec3(target_position,camera->transform.position);

    navigation->moved = true;
}

void turnCamera(Camera *camera, Navigation *navigation, f32 yaw, f32 pitch) {
    rotateXform3(&camera->transform, yaw, pitch, 0);

    navigation->turned = true;
}

void orbitCamera(Camera *camera, Navigation *navigation, f32 azimuth, f32 altitude) {
    // Move the camera forward to the position of it's target:
    vec3 movement = scaleVec3(*camera->transform.forward_direction, navigation->target_distance);
    camera->transform.position = addVec3(camera->transform.position, movement);

    // Reorient the camera while it is at the position of it's target:
    turnCamera(camera, navigation, azimuth, altitude);

    // Back the camera away from it's target position using the updated forward direction:
    movement = scaleVec3(*camera->transform.forward_direction, navigation->target_distance);
    camera->transform.position = subVec3(camera->transform.position, movement);

    navigation->moved = true;
}

void panCamera(Camera *camera, Navigation *navigation, f32 right, f32 up) {
    vec3 up_movement    = scaleVec3(*camera->transform.up_direction, up);
    vec3 right_movement = scaleVec3(*camera->transform.right_direction, right);
    camera->transform.position = addVec3(camera->transform.position, up_movement);
    camera->transform.position = addVec3(camera->transform.position, right_movement);

    navigation->moved = true;
}

void navigateCamera(Camera *camera, Navigation *navigation, f32 delta_time) {
    vec3 target_velocity = getVec3Of(0);

    if (navigation->move.right)    target_velocity.x += navigation->settings.max_velocity;
    if (navigation->move.left)     target_velocity.x -= navigation->settings.max_velocity;
    if (navigation->move.up)       target_velocity.y += navigation->settings.max_velocity;
    if (navigation->move.down)     target_velocity.y -= navigation->settings.max_velocity;
    if (navigation->move.forward)  target_velocity.z += navigation->settings.max_velocity;
    if (navigation->move.backward) target_velocity.z -= navigation->settings.max_velocity;
    if (navigation->turn.left)  turnCamera(camera, navigation, delta_time * +navigation->settings.speeds.turn, 0);
    if (navigation->turn.right) turnCamera(camera, navigation, delta_time * -navigation->settings.speeds.turn, 0);

    // Update the current velocity:
    f32 velocity_difference = navigation->settings.acceleration * delta_time;
    navigation->current_velocity = approachVec3(navigation->current_velocity, target_velocity, velocity_difference);

    navigation->moved = nonZeroVec3(navigation->current_velocity);
    if (navigation->moved) { // Update the current position:
        vec3 movement = scaleVec3(navigation->current_velocity, delta_time);
        movement = mulVec3Mat3(movement, camera->transform.rotation_matrix);
        camera->transform.position = addVec3(camera->transform.position, movement);
    }
}

void panViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    f32 speed = viewport->navigation.settings.speeds.pan * delta_time;
    panCamera(viewport->camera, &viewport->navigation,
              -(f32)mouse->pos_raw_diff.x * speed,
              +(f32)mouse->pos_raw_diff.y * speed);

    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void zoomViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    f32 speed = viewport->navigation.settings.speeds.zoom * delta_time;
    zoomCamera(viewport->camera, &viewport->navigation,
               mouse->wheel_scroll_amount * speed);

    mouse->wheel_scroll_handled = true;
}

void dollyViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    f32 speed = viewport->navigation.settings.speeds.dolly * delta_time;
    dollyCamera(viewport->camera, &viewport->navigation,
                mouse->wheel_scroll_amount * speed,
                viewport->navigation.settings.target_distance);

    mouse->wheel_scroll_handled = true;
}

void orientViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    f32 speed = viewport->navigation.settings.speeds.orient * delta_time;
    turnCamera(viewport->camera, &viewport->navigation,
               -(f32)mouse->pos_raw_diff.x * speed,
               -(f32)mouse->pos_raw_diff.y * speed);

    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void orbitViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    f32 speed = viewport->navigation.settings.speeds.orbit * delta_time;
    orbitCamera(viewport->camera, &viewport->navigation,
                -(f32)mouse->pos_raw_diff.x * speed,
                -(f32)mouse->pos_raw_diff.y * speed);

    mouse->raw_movement_handled = true;
    mouse->moved = false;
}

void navigateViewport(Viewport *viewport, f32 delta_time) {
    navigateCamera(viewport->camera, &viewport->navigation, delta_time);
}