#include "../SlimEngine/app.h"
#include "../SlimEngine/core/text.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    HUD *hud = &app->viewport.hud;
    startFrameTimer(timer);

    drawSceneToViewport(&app->scene, &app->viewport);
    if (app->viewport.settings.show_hud) {
        setCountersInHUD(hud, timer);
        drawHUD(app->viewport.frame_buffer,
                Color(Green), hud);
    }

    endFrameTimer(timer);
}
void updateMouseCoordsInHUD(i32 x, i32 y) {
    setMouseCoordinatesInHUD(&app->viewport.hud, &app->controls.mouse);
}
void updateSizeInHUD(u16 width, u16 height) {
    setDimensionsInHUD(&app->viewport.hud, width, height);
}
void updateHudVisibility(u8 key, bool is_pressed) {
    if (!is_pressed && key == app->controls.key_map.tab)
        app->viewport.settings.show_hud = !app->viewport.settings.show_hud;
}
void setupCamera(Viewport *viewport) {
    xform3 *camera_xform = &viewport->camera->transform;
    camera_xform->position.y = 7;
    camera_xform->position.z = -11;
    rotateXform3(camera_xform, 0, -0.2f, 0);
}
void setupScene(Scene *scene) {
    Primitive *grid = scene->primitives;
    grid->type = PrimitiveType_Grid;
    grid->position.z = 5;
    rotatePrimitive(grid,0.5f,0,0);
    initGrid(scene->grids,
             -5,-5,
             +5,+5,
             11, 11);
}
void initApp(Defaults *defaults) {
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupCamera;
    app->on.windowRedraw = updateAndRender;
    app->on.windowResize = updateSizeInHUD;
    app->on.keyChanged   = updateHudVisibility;
    app->on.mouseMovementSet = updateMouseCoordsInHUD;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}