#include "../SlimEngine/app.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

#include "./_common.h"

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
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
}
void setupScene(Scene *scene) {
    scene->primitives->type = PrimitiveType_Grid;
    initGrid(scene->grids
             ,-5,-5
             ,+5,+5,
             11, 11);
}
void initApp(Defaults *defaults) {
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
    app->on.windowRedraw = updateAndRender;
    app->on.sceneReady = setupScene;
    app->on.keyChanged = onKeyChanged;
    app->on.mouseButtonDown          = onMouseButtonDown;
    app->on.mouseButtonDoubleClicked = onMouseButtonDoubleClicked;
}