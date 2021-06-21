#include "../SlimEngine/app.h"
#include "../SlimEngine/shapes/box.h"
#include "../SlimEngine/shapes/grid.h"
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
Primitive coil_prim, helix_prim;
f32 height, radius, thickness;

void updateShapes(f32 delta_time) {
    elapsed += delta_time;

    helix_prim.scale.y = 5 + sinf(elapsed * 1.7f);
    helix_prim.scale.x = helix_prim.scale.z = 1 - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil_prim.scale.x = coil_prim.scale.z = 1 + sinf(elapsed * 1.5f + 2) * 0.2f;
    coil.thickness = thickness - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box.transform.scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    grid.transform.scale = getVec3Of(1 + sinf(elapsed + 1) * 0.2f);
    rotateXform3(&box.transform, rot_speed, rot_speed, rot_speed);
}

void drawShapes() {
    startFrameTimer(&app->time.timers.update);
    updateShapes(app->time.timers.update.delta_time);

    Viewport *viewport = &app->viewport;
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    drawBox(  viewport, Color(Yellow),  &box, BOX__ALL_SIDES);
    drawCoil( viewport, Color(Magenta), &coil,  &coil_prim, CURVE_STEPS);
    drawHelix(viewport, Color(Cyan),    &helix, &helix_prim, CURVE_STEPS);
    drawGrid( viewport, Color(Green),   &grid);

    endFrameTimer(&app->time.timers.update);
}

void onViewportReady() {
    xform3 *xf = &app->viewport.camera->transform;
    xf->position.y = +5;
    xf->position.z = -5;
    rotateXform3(xf, 0, -0.3f, 0);
}

void initApp(Defaults *defaults) {
    app->on.windowRedraw = drawShapes;
    app->on.viewportReady = onViewportReady;

    initCoil(&coil);
    initHelix(&helix);
    initPrimitive(&coil_prim);
    initPrimitive(&helix_prim);

    coil_prim.flags = helix_prim.flags = IS_TRANSLATED | IS_ROTATED | IS_SCALED | IS_SCALED_NON_UNIFORMLY;
    coil_prim.position.x = coil_prim.position.x = 2;
//    helix.transform.position.x = 2;
//    helix.transform.position.y = coil.transform.position.y = -1.5f;
//    helix.transform.position.z = coil.transform.position.z = 2;
    thickness = coil.thickness;

    vec3 box_min = getVec3Of(-1);
    vec3 box_max = getVec3Of(+1);
    initBox(&box, box_min, box_max);
    initGrid(&grid,
            -5, +5,
            -5, +5,
            11,
            11);

//    box.transform.position.x = -2;
//    box.transform.position.y = 1;
//    box.transform.position.z = 10;
}