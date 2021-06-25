#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/viewport/navigation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

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
void updateViewport(Viewport *viewport, Mouse *mouse) {
    if (mouse->is_captured) {
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
void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    Primitive *prim = &scene->primitives[0];
    drawGrid(viewport, Color(prim->color), scene->grids, prim);

    u8 camera_id = viewport->camera == scene->cameras ? 1 : 0;
    Camera *camera = &scene->cameras[camera_id];
    drawCamera(viewport, Color(camera_id ? Yellow : Cyan), camera);
}
void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Scene *scene = &app->scene;
    Mouse *mouse = &app->controls.mouse;
    Viewport *viewport = &app->viewport;

    startFrameTimer(timer);

    updateViewport(viewport, mouse);
    navigateViewport(viewport, timer->delta_time);

    drawSceneToViewport(scene, viewport);
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
    grid_primitive->scale.x = grid_primitive->scale.z = 5;
    grid_primitive->position.z = 5;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(scene->grids,11, 11);
    xform3 *xf = &scene->cameras[1].transform;
    xf->position.y = 6;
    xf->position.z = 21;
    rotateXform3(xf, 8, -0.1f, 0);
    zoomCamera(&scene->cameras[1], -2);
}
void setupCamera(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = 7;
    xf->position.z = -11;
    rotateXform3(xf, 0, -0.2f, 0);
}
void initApp(Defaults *defaults) {
    defaults->settings.scene.cameras    = 2;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupCamera;
}