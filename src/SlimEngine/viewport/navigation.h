#pragma once

#include "../core/base.h"
#include "../scene/camera.h"

#define NAVIGATION_DEFAULT__MAX_VELOCITY 5
#define NAVIGATION_DEFAULT__ACCELERATION 10
#define NAVIGATION_DEFAULT__TARGET_DISTANCE 15
#define NAVIGATION_SPEED_DEFAULT__TURN 2
#define NAVIGATION_SPEED_DEFAULT__ORIENT 1
#define NAVIGATION_SPEED_DEFAULT__ORBIT 1
#define NAVIGATION_SPEED_DEFAULT__ZOOM 2
#define NAVIGATION_SPEED_DEFAULT__DOLLY 300
#define NAVIGATION_SPEED_DEFAULT__PAN 10

typedef struct NavigationSpeedSettings {
    f32 turn, zoom, dolly, pan, orbit, orient;
} NavigationSpeedSettings;

typedef struct NavigationSettings {
    NavigationSpeedSettings speeds;
    f32 max_velocity, acceleration, target_distance;
} NavigationSettings;

NavigationSettings getDefaultNavigationSettings() {
    NavigationSettings navigation_settings;

    navigation_settings.target_distance = NAVIGATION_DEFAULT__TARGET_DISTANCE;
    navigation_settings.max_velocity    = NAVIGATION_DEFAULT__MAX_VELOCITY;
    navigation_settings.acceleration    = NAVIGATION_DEFAULT__ACCELERATION;

    navigation_settings.speeds.turn   = NAVIGATION_SPEED_DEFAULT__TURN;
    navigation_settings.speeds.orient = NAVIGATION_SPEED_DEFAULT__ORIENT;
    navigation_settings.speeds.orbit  = NAVIGATION_SPEED_DEFAULT__ORBIT;
    navigation_settings.speeds.zoom   = NAVIGATION_SPEED_DEFAULT__ZOOM;
    navigation_settings.speeds.dolly  = NAVIGATION_SPEED_DEFAULT__DOLLY;
    navigation_settings.speeds.pan    = NAVIGATION_SPEED_DEFAULT__PAN;

    return navigation_settings;
}

typedef struct NavigationTurn {
    bool right, left;
} NavigationTurn;

typedef struct NavigationMove {
    bool right, left, up, down, forward, backward;
} NavigationMove;

typedef struct Navigation {
    NavigationSettings settings;
    NavigationMove move;
    NavigationTurn turn;
    vec3 current_velocity;
    f32 zoom, dolly, target_distance;
    bool zoomed, moved, turned;
} Navigation;

void initNavigation(Navigation *navigation, NavigationSettings navigation_settings) {
    navigation->settings = navigation_settings;

    navigation->target_distance = navigation_settings.target_distance;
    navigation->zoom = CAMERA_DEFAULT__FOCAL_LENGTH;
    navigation->dolly = 0;
    navigation->turned = false;
    navigation->moved = false;
    navigation->zoomed = false;
    navigation->current_velocity = getVec3Of(0);

    navigation->move.right = false;
    navigation->move.left = false;
    navigation->move.up = false;
    navigation->move.down = false;
    navigation->move.forward = false;
    navigation->move.backward = false;

    navigation->turn.right = false;
    navigation->turn.left = false;
}

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