#pragma once

#include "../core/pixels.h"
#include "../scene/camera.h"
#include "./hud.h"
#include "./navigation.h"

#define VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE 0.1f
#define VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE 1000.0f

typedef struct ViewportSettings {
    f32 near_clipping_plane_distance,
         far_clipping_plane_distance;
    bool show_hud;
} ViewportSettings;

ViewportSettings getDefaultViewportSettings() {
    ViewportSettings default_viewport_settings;

    default_viewport_settings.near_clipping_plane_distance = VIEWPORT_DEFAULT__NEAR_CLIPPING_PLANE_DISTANCE;
    default_viewport_settings.far_clipping_plane_distance  = VIEWPORT_DEFAULT__FAR_CLIPPING_PLANE_DISTANCE;
    default_viewport_settings.show_hud = false;

    return default_viewport_settings;
}

typedef struct Viewport {
    ViewportSettings settings;
    Navigation navigation;
    HUD hud;
    Camera *camera;
    PixelGrid *frame_buffer;
} Viewport;

void initViewport(Viewport *viewport,
                  ViewportSettings viewport_settings,
                  NavigationSettings navigation_settings,
                  Camera *camera,
                  PixelGrid *frame_buffer) {
    viewport->camera = camera;
    viewport->settings = viewport_settings;
    viewport->frame_buffer = frame_buffer;
    initHUD(&viewport->hud,
            viewport->frame_buffer->dimensions.width,
            viewport->frame_buffer->dimensions.height);
    initNavigation(&viewport->navigation, navigation_settings);
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