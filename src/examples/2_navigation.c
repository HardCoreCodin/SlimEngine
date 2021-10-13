#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/viewport/viewport.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/viewport/navigation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

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
    Mouse *mouse = &app->controls.mouse;
    Viewport *viewport = &app->viewport;
    Grid      *grid = app->scene.grids;
    Primitive *prim = app->scene.primitives;

    beginFrame(timer);
        updateViewport(viewport, mouse);
        beginDrawing(viewport);
            drawGrid(grid, prim, Color(prim->color),
                     0.5f, 0, viewport);
            drawMouseAndKeyboard(mouse, viewport);
        endDrawing(viewport);
    endFrame(timer, mouse);
}

void setupScene(Scene *scene) {
    Grid *grid = scene->grids;
    initGrid(grid,11, 11);
    Primitive *grid_prim = scene->primitives;
    grid_prim->type = PrimitiveType_Grid;
    grid_prim->scale = Vec3(5, 1, 5);
    grid_prim->position.z = 5;
    rotatePrimitive(grid_prim, 0.5f, 0, 0);

    xform3 *camera_xform = &scene->cameras[0].transform;
    camera_xform->position = Vec3(0, 7, -11);
    rotateXform3(camera_xform, 0, -0.2f, 0);
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