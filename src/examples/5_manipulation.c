#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/curve.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"
// Or using the single-header file:
//#include "../SlimEngine.h"

#include "./_common.h"

void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    Primitive *primitive = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, primitive++) {
        switch (primitive->type) {
            case PrimitiveType_Coil:
            case PrimitiveType_Helix:
                drawCurve(viewport, Color(primitive->color),
                          scene->curves + primitive->id, primitive,
                          CURVE_STEPS, 0);
                break;
            case PrimitiveType_Box:
                drawBox(viewport, Color(primitive->color),
                        scene->boxes + primitive->id, primitive,
                        BOX__ALL_SIDES, 0);
                break;
            case PrimitiveType_Grid:
                drawGrid(viewport, Color(primitive->color),
                         scene->grids + primitive->id, primitive, 0);
                break;
            default:
                break;
        }
    }
}

void onButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff = Vec2i(0 ,0);
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
    Controls *controls = &app->controls;
    Viewport *viewport = &app->viewport;
    Mouse *mouse = &controls->mouse;
    Scene *scene = &app->scene;

    startFrameTimer(timer);

    if (!mouse->is_captured)
        manipulateSelection(scene, viewport, controls);

    if (!controls->is_pressed.alt)
        updateViewport(viewport, mouse);

    drawSceneToViewport(scene, viewport);
    drawSelection(scene, viewport, controls);
    drawMouseAndKeyboard(viewport, mouse);

    resetMouseChanges(mouse);
    endFrameTimer(timer);
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
void setupScene(Scene *scene) {
    xform3 *xf = &scene->cameras[0].transform;
    xf->position = Vec3(0, 7, -11);
    rotateXform3(xf, 0, -0.2f, 0);

    Primitive *helix = &scene->primitives[0];
    Primitive *coil  = &scene->primitives[1];
    Primitive *box   = &scene->primitives[2];
    Primitive *grid  = &scene->primitives[3];
    rotatePrimitive(grid, 0.5f, 0, 0);
    initGrid(scene->grids, 11, 11);

    helix->type = PrimitiveType_Helix;
    coil->type  = PrimitiveType_Coil;
    grid->type  = PrimitiveType_Grid;
    box->type   = PrimitiveType_Box;
    box->color   = Yellow;
    grid->color  = Green;
    coil->color  = Magenta;
    helix->color = Cyan;
    helix->position = Vec3(-3, 4, 2);
    coil->position  = Vec3(4, 4, 2);
    grid->scale     = Vec3(5, 1, 5);
    helix->id = box->id = grid->id = 0;
    coil->id  = 1;
    scene->curves[0].revolution_count = 10;
    scene->curves[1].revolution_count = 30;
}
void initApp(Defaults *defaults) {
    defaults->settings.scene.boxes      = 1;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 4;
    app->on.sceneReady    = setupScene;
    app->on.windowRedraw  = updateAndRender;;
    app->on.sceneReady = setupScene;
    app->on.keyChanged = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
}