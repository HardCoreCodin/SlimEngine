#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/viewport/hud.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void setupCamera(Viewport *viewport) {
    xform3 *camera_xform = &viewport->camera->transform;
    camera_xform->position.y = 7;
    camera_xform->position.z = -11;
    rotateXform3(camera_xform, 0, -0.2f, 0);
}
void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale.x = 5;
    grid_primitive->scale.z = 5;
    grid_primitive->position.z = 5;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(scene->grids,11, 11);
}

void updateAndRender() {
    Viewport *viewport = &app->viewport;
    PixelGrid *canvas = viewport->frame_buffer;
    HUD *hud = &viewport->hud;
    Grid *grid = &app->scene.grids[0];
    Primitive *grid_primitive = &app->scene.primitives[0];
    startFrameTimer(&app->time.timers.update);

    fillPixelGrid(canvas, Color(Black));
    drawGrid(viewport, Color(grid_primitive->color),
             grid, grid_primitive);
    if (viewport->settings.show_hud) {
        setCountersInHUD(hud, &app->time.timers.update);
        drawHUD(canvas, Color(Green), hud);
    }
    endFrameTimer(&app->time.timers.update);
}
void updateMouseCoordsInHUD(i32 x, i32 y) {
    setMouseCoordinatesInHUD(&app->viewport.hud,
                             &app->controls.mouse);
}
void updateSizeInHUD(u16 width, u16 height) {
    setDimensionsInHUD(&app->viewport.hud, width, height);
}
void updateHudVisibility(u8 key, bool is_pressed) {
    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}
void initApp(Defaults *defaults) {
    app->on.windowRedraw     = updateAndRender;
    app->on.windowResize     = updateSizeInHUD;
    app->on.keyChanged       = updateHudVisibility;
    app->on.mouseMovementSet = updateMouseCoordsInHUD;
    app->on.sceneReady       = setupScene;
    app->on.viewportReady    = setupCamera;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}