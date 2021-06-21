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

void panViewportWithMouse(Viewport *viewport, Mouse *mouse) {
    panCamera(viewport->camera, &viewport->navigation,
              -(f32)mouse->pos_raw_diff.x * viewport->navigation.settings.speeds.pan,
               +(f32)mouse->pos_raw_diff.y * viewport->navigation.settings.speeds.pan);

    mouse->pos_raw_diff.x = 0;
    mouse->pos_raw_diff.y = 0;
    mouse->moved = false;
}

void zoomViewportWithMouse(Viewport *viewport, Mouse *mouse) {
    zoomCamera(viewport->camera, &viewport->navigation,
               mouse->wheel_scroll_amount * viewport->navigation.settings.speeds.zoom);

    mouse->wheel_scrolled = false;
    mouse->wheel_scroll_amount = 0;
}

void dollyViewportWithMouse(Viewport *viewport, Mouse *mouse) {
    dollyCamera(viewport->camera, &viewport->navigation,
                viewport->navigation.settings.speeds.dolly * mouse->wheel_scroll_amount,
                     viewport->navigation.settings.target_distance);

    mouse->wheel_scroll_amount = 0;
    mouse->wheel_scrolled = false;
}

void orientViewportWithMouse(Viewport *viewport, Mouse *mouse) {
    turnCamera(viewport->camera, &viewport->navigation,
               -(f32)mouse->pos_raw_diff.x * viewport->navigation.settings.speeds.orient,
               -(f32)mouse->pos_raw_diff.y * viewport->navigation.settings.speeds.orient);

    mouse->pos_raw_diff.x = 0;
    mouse->pos_raw_diff.y = 0;
    mouse->moved = false;
}

void orbitViewportWithMouse(Viewport *viewport, Mouse *mouse) {
    orbitCamera(viewport->camera, &viewport->navigation,
                -(f32)mouse->pos_raw_diff.x * viewport->navigation.settings.speeds.orbit,
                -(f32)mouse->pos_raw_diff.y * viewport->navigation.settings.speeds.orbit);

    mouse->pos_raw_diff.x = 0;
    mouse->pos_raw_diff.y = 0;
    mouse->moved = false;
}

void navigateViewportWithKeyboard(Viewport *viewport, f32 delta_time) {
    navigateCamera(viewport->camera, &viewport->navigation, delta_time);
}