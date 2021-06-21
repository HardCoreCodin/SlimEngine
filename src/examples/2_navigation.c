#include "../SlimEngine/app.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/shapes/coil.h"
#include "../SlimEngine/shapes/helix.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

#define CURVE_STEPS 3600
static float elapsed;

Box box;
Grid grid;
Coil coil;
Helix helix;
f32 height, radius, thickness;

void updateShapes(f32 delta_time) {
    elapsed += delta_time;

    helix.height = height + sinf(elapsed * 1.7f) * 0.2f;
    helix.radius = radius - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil.radius  = radius + sinf(elapsed * 1.5f + 2) * 0.2f;
    coil.thickness = thickness - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box.transform.scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    rotateXform3(&box.transform, rot_speed, rot_speed, rot_speed);
}

void resetRawMouseInput() {
    app->controls.mouse.pos_raw_diff.x = 0;
    app->controls.mouse.pos_raw_diff.y = 0;
}

void onKeyChanged(u8 key, bool is_pressed) {
    NavigationMove *move = &app->viewport.navigation.move;
    NavigationTurn *turn = &app->viewport.navigation.turn;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'Q') turn->left     = is_pressed;
    if (key == 'E') turn->right    = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
    if (key == app->controls.key_map.ctrl) resetRawMouseInput();
}
void onMouseDown(MouseButton *mb) { resetRawMouseInput(); }
void updateViewport(Viewport *viewport, Mouse *mouse, f32 delta_time) {
    if (app->controls.is_pressed.ctrl) {
        navigateViewportWithKeyboard(viewport, delta_time);
        if (mouse->moved)        orientViewportWithMouse(viewport, mouse);
        if (mouse->wheel_scrolled) zoomViewportWithMouse(viewport, mouse);
    } else {
        if (mouse->wheel_scrolled) dollyViewportWithMouse(viewport, mouse);
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)  panViewportWithMouse(viewport, mouse);
            if (mouse->right_button.is_pressed) orbitViewportWithMouse(viewport, mouse);
        }
    }
}

void drawShapes() {
    startFrameTimer(&app->time.timers.update);
    f32 delta_time = app->time.timers.update.delta_time;
    updateShapes(delta_time);
    updateViewport(&app->viewport, &app->controls.mouse, delta_time);

    Viewport *viewport = &app->viewport;
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    drawBox(  viewport, Color(Yellow),  &box, BOX__ALL_SIDES);
    drawCoil( viewport, Color(Magenta), &coil,  CURVE_STEPS);
    drawHelix(viewport, Color(Cyan),    &helix, CURVE_STEPS);
    drawGrid( viewport, Color(Green),   &grid);

    endFrameTimer(&app->time.timers.update);
}

void initApp(Defaults *defaults) {
    app->on.windowRedraw = drawShapes;
    app->on.mouseButtonDown = onMouseDown;
    app->on.keyChanged = onKeyChanged;

    initCoil(&coil);
    initHelix(&helix);
    coil.transform.position.x = -1;
    helix.transform.position.x = 2;
    helix.transform.position.y = coil.transform.position.y = -1.5f;
    helix.transform.position.z = coil.transform.position.z = 8;

    height = helix.height;
    radius = helix.radius;
    thickness = coil.thickness;

    vec3 box_min = getVec3Of(-1);
    vec3 box_max = getVec3Of(+1);
    initBox(&box, box_min, box_max);
    initGrid(&grid,
             -5, +5,
             -5, +5,
             11,
             11);

    box.transform.position.x = -2;
    box.transform.position.y = 1;
    box.transform.position.z = 10;
    grid.transform.position.x = 0;
    grid.transform.position.y = -5;
    grid.transform.position.z = 20;
}