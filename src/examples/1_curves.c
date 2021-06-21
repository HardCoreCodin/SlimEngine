#include "../SlimEngine/app.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/curve.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void updateShapes(f32 delta_time) {
    static float elapsed = 0;
    elapsed += delta_time;

    Primitive *helix_prim = app->scene.primitives + 0;
    Primitive *coil_prim  = app->scene.primitives + 1;
    Primitive *box_prim   = app->scene.primitives + 2;
    Primitive *grid_prim  = app->scene.primitives + 3;

    helix_prim->scale.y = 5 + sinf(elapsed * 1.7f);
    helix_prim->scale.x = helix_prim->scale.z = 1 - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil_prim->scale.x  = coil_prim->scale.z  = 1 + sinf(elapsed * 1.5f + 2) * 0.2f;

    Curve *coil  = app->scene.curves + 1;
    coil->thickness = 0.3f - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box_prim->scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    grid_prim->scale = getVec3Of(1 + sinf(elapsed + 1) * 0.2f);

    vec3 axis;
    axis.x = 1; axis.y = 0; axis.y = 0;

    transformPrimitive(box_prim, rot_speed, rot_speed, rot_speed);
}

void updateAndRender() {
    startFrameTimer(&app->time.timers.update);
    updateShapes(app->time.timers.update.delta_time);
    drawSceneToViewport(&app->scene, &app->viewport);
    endFrameTimer(&app->time.timers.update);
}

void setupCamera(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = +5;
    xf->position.z = -5;
    rotateXform3(xf, 0, -0.3f, 0);
}

void setupScene(Scene *scene) {
    Primitive *helix_prim = scene->primitives + 0;
    Primitive *coil_prim  = scene->primitives + 1;
    Primitive *box_prim   = scene->primitives + 2;
    Primitive *grid_prim  = scene->primitives + 3;

    helix_prim->type = PrimitiveType_Helix;
    coil_prim->type  = PrimitiveType_Coil;
    grid_prim->type  = PrimitiveType_Grid;
    box_prim->type   = PrimitiveType_Box;

    box_prim->id   = 0;
    grid_prim->id  = 0;
    helix_prim->id = 0;
    coil_prim->id  = 1;

    box_prim->color = Yellow;
    grid_prim->color = Green;
    coil_prim->color = Magenta;
    helix_prim->color = Cyan;

    u8 flags = IS_VISIBLE |
               IS_TRANSLATED |
               IS_ROTATED |
               IS_SCALED |
               IS_SCALED_NON_UNIFORMLY;
    coil_prim->flags = helix_prim->flags = box_prim->flags = grid_prim->flags = flags;
    coil_prim->position.x = coil_prim->position.x = 2;

    Curve *helix = scene->curves + 0;
    Curve *coil  = scene->curves + 1;
    helix->revolution_count = coil->revolution_count = 10;

    Grid *grid = app->scene.grids;
    initGrid(grid,
             -5, -5,
             +5, +5,
             11,
             11);
}

void initApp(Defaults *defaults) {
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupCamera;

    defaults->settings.scene.boxes      = 1;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 4;
}