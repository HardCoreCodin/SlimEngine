#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/viewport/navigation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

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

    Primitive *prim = &scene->primitives[0];
    drawGrid(viewport, Color(prim->color), scene->grids, prim);

    Camera *camera1 = &scene->cameras[0];
    Camera *camera2 = &scene->cameras[1];
    if (viewport->camera == camera1)
        drawCamera(viewport, Color(Yellow), camera2);
    else
        drawCamera(viewport, Color(Cyan  ), camera1);

    resetMouseChanges(mouse);
    endFrameTimer(timer);
}
void onKeyChanged(u8 key, bool is_pressed) {
    Viewport *viewport = &app->viewport;
    Camera *cameras = app->scene.cameras;
    NavigationMove *move = &viewport->navigation.move;
    if (key == '1') viewport->camera = &cameras[0];
    if (key == '2') viewport->camera = &cameras[1];
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
}
void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale = Vec3(5, 1, 5);
    grid_primitive->position.z = 5;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(&scene->grids[0],11, 11);

    xform3 *camera_xform = &scene->cameras[0].transform;
    camera_xform->position = Vec3(0, 7, -11);
    rotateXform3(camera_xform, 0, -0.2f, 0);

    camera_xform = &scene->cameras[1].transform;
    camera_xform->position = Vec3(0, 6, 21);
    rotateXform3(camera_xform, 8, -0.1f, 0);
    zoomCamera(&scene->cameras[1], -2);
}
void initApp(Defaults *defaults) {
    defaults->settings.scene.cameras    = 2;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
    app->on.sceneReady    = setupScene;
    app->on.windowRedraw  = updateAndRender;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
}