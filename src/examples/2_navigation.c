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

void onMouseButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff.x = 0;
    app->controls.mouse.pos_raw_diff.y = 0;
}

void onMouseButtonDoubleClicked(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onMouseButtonDown(mouse_button);
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
void updateAndRender() {
    startFrameTimer(&app->time.timers.update);

    float delta_time = app->time.timers.update.delta_time;
    Mouse *mouse = &app->controls.mouse;

    updateViewport(&app->viewport, mouse, delta_time);
    navigateViewport(&app->viewport, delta_time);

    drawSceneToViewport(&app->scene, &app->viewport);
    drawMouseAndKeyboard(&app->viewport, mouse);

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
void setupScene(Scene *scene) {
    scene->primitives->type = PrimitiveType_Grid;
    scene->primitives->position.z = 5;
    scene->primitives->rotation.axis.y = 0.5f;
    initGrid(scene->grids,-5,-5,+5,+5,
             11, 11);
}
void setupCamera(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = 7;
    xf->position.z = -11;
    rotateXform3(xf, 0, -0.2f, 0);
}
void initApp(Defaults *defaults) {
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupCamera;
    app->on.sceneReady = setupScene;
    app->on.keyChanged = onKeyChanged;
    app->on.mouseButtonDown          = onMouseButtonDown;
    app->on.mouseButtonDoubleClicked = onMouseButtonDoubleClicked;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}