#include "../SlimEngine/app.h"
#include "../SlimEngine/shapes/box.h"
#include "../SlimEngine/shapes/grid.h"
#include "../SlimEngine/shapes/coil.h"
#include "../SlimEngine/shapes/curve.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

#define CURVE_STEPS 3600
static float elapsed;

Box box;
Grid grid;

void updateShapes(f32 delta_time) {
    elapsed += delta_time;

    Primitive *helix_prim = app->scene.primitives;
    Primitive *coil_prim  = app->scene.primitives + 1;

    helix_prim->scale.y = 5 + sinf(elapsed * 1.7f);
    helix_prim->scale.x = helix_prim->scale.z = 1 - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil_prim->scale.x  = coil_prim->scale.z  = 1 + sinf(elapsed * 1.5f + 2) * 0.2f;

    Curve *coil  = app->scene.curves + 1;
    coil->thickness = 0.3f - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box.transform.scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    grid.transform.scale = getVec3Of(1 + sinf(elapsed + 1) * 0.2f);
    rotateXform3(&box.transform, rot_speed, rot_speed, rot_speed);
}

void onWindowRedraw() {
    startFrameTimer(&app->time.timers.update);
    updateShapes(app->time.timers.update.delta_time);

    Viewport *viewport = &app->viewport;
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    Curve *helix = app->scene.curves;
    Curve *coil  = app->scene.curves + 1;
    Primitive *helix_prim = app->scene.primitives;
    Primitive *coil_prim  = app->scene.primitives + 1;

    drawBox(  viewport, Color(Yellow),  &box, BOX__ALL_SIDES);
    drawCurve(viewport, Color(Magenta), coil,  coil_prim, CURVE_STEPS);
    drawCurve(viewport, Color(Cyan),    helix, helix_prim, CURVE_STEPS);
    drawGrid( viewport, Color(Green),   &grid);

    endFrameTimer(&app->time.timers.update);
}

void onViewportReady() {
    xform3 *xf = &app->viewport.camera->transform;
    xf->position.y = +5;
    xf->position.z = -5;
    rotateXform3(xf, 0, -0.3f, 0);
}

void onSceneReady() {
    Curve *helix = app->scene.curves;
    Curve *coil  = app->scene.curves + 1;
    Primitive *helix_prim = app->scene.primitives;
    Primitive *coil_prim  = app->scene.primitives + 1;

    helix->revolution_count = 10;
    helix_prim->type = PrimitiveType_Helix;

    coil->revolution_count = 10;
    coil_prim->type = PrimitiveType_Coil;
    coil_prim->flags = helix_prim->flags = IS_TRANSLATED | IS_ROTATED | IS_SCALED | IS_SCALED_NON_UNIFORMLY;
    coil_prim->position.x = coil_prim->position.x = 2;
//    helix.transform.position.x = 2;
//    helix.transform.position.y = coil.transform.position.y = -1.5f;
//    helix.transform.position.z = coil.transform.position.z = 2;
}

void initApp(Defaults *defaults) {
    app->on.windowRedraw  = onWindowRedraw;
    app->on.sceneReady    = onSceneReady;
    app->on.viewportReady = onViewportReady;

    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 2;

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