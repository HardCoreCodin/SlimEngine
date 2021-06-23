#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/viewport/navigation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

#include "./_common.h"

void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));
    drawGrid(viewport, Color(scene->primitives->color), scene->grids, scene->primitives);
}

void onButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff.x = 0;
    app->controls.mouse.pos_raw_diff.y = 0;
}
void onDoubleClick(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onButtonDown(mouse_button);
    }
}
void updateViewport(Viewport *vp, Mouse *mouse, f32 dt) {
    if (mouse->is_captured) {
        if (mouse->moved)         orientViewport(vp, mouse, dt);
        if (mouse->wheel_scrolled)  zoomViewport(vp, mouse, dt);
    } else {
        if (mouse->wheel_scrolled) dollyViewport(vp, mouse, dt);
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)  panViewport(vp, mouse, dt);
            if (mouse->right_button.is_pressed) orbitViewport(vp, mouse, dt);
        }
    }
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
void setupCamera(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = 7;
    xf->position.z = -11;
    rotateXform3(xf, 0, -0.2f, 0);
}
void updateAndRender() {
    startFrameTimer(&app->time.timers.update);
    float delta_time = app->time.timers.update.delta_time;

    updateViewport(&app->viewport, &app->controls.mouse, delta_time);
    navigateViewport(&app->viewport, delta_time);

    drawSceneToViewport(&app->scene, &app->viewport);
    drawMouseAndKeyboard(&app->viewport, &app->controls.mouse);

    resetMouseChanges(&app->controls.mouse);
    endFrameTimer(&app->time.timers.update);
}
void onKeyChanged(u8 key, bool is_pressed) {
    NavigationMove *move = &app->viewport.navigation.move;
    NavigationTurn *turn = &app->viewport.navigation.turn;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
    if (key == 'Q') turn->left     = is_pressed;
    if (key == 'E') turn->right    = is_pressed;
}
void initApp(Defaults *defaults) {
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupCamera;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}