//#include "../SlimEngine/app.h"
//#include "../SlimEngine/core/time.h"
//#include "../SlimEngine/scene/grid.h"
//#include "../SlimEngine/viewport/navigation.h"
// Or using the single-header file:
 #include "../SlimEngine.h"

#include "./_common.h"

void onButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff = Vec2i(0, 0);
}
void onDoubleClick(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onButtonDown(mouse_button);
    }
}
void updateViewport(Viewport *viewport, Mouse *mouse) {
    if (mouse->is_captured) {
        navigateViewport(viewport, app->time.timers.update.delta_time);
        if (mouse->moved)         orientViewport(viewport, mouse);
        if (mouse->wheel_scrolled)  zoomViewport(viewport, mouse);
    } else {
        if (mouse->wheel_scrolled) dollyViewport(viewport, mouse);
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)  panViewport(viewport, mouse);
            if (mouse->right_button.is_pressed) orbitViewport(viewport, mouse);
        }
    }
}
void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Scene *scene = &app->scene;
    Mouse *mouse = &app->controls.mouse;
    Viewport *viewport = &app->viewport;

    startFrameTimer(timer);

    updateViewport(viewport, mouse);

    fillPixelGrid(viewport->frame_buffer, Color(Black));
    drawGrid(viewport, Color(scene->primitives->color),
             &scene->grids[0], &scene->primitives[0]);
    drawMouseAndKeyboard(viewport, mouse);

    resetMouseChanges(mouse);
    endFrameTimer(timer);
}

void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale = Vec3(5, 1, 5);
    grid_primitive->position.z = 5;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(&scene->grids[0],11, 11);

    xform3 *xf = &scene->cameras[0].transform;
    xf->position = Vec3(0, 7, -11);
    rotateXform3(xf, 0, -0.2f, 0);
}
void onKeyChanged(u8 key, bool is_pressed) {
    NavigationMove *move = &app->viewport.navigation.move;
    NavigationTurn *turn = &app->viewport.navigation.turn;
    if (key == 'Q') turn->left     = is_pressed;
    if (key == 'E') turn->right    = is_pressed;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'D') move->right    = is_pressed;
}
void initApp(Defaults *defaults) {
    app->on.sceneReady    = setupScene;
    app->on.windowRedraw  = updateAndRender;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}