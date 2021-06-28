#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/curve.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    Primitive *primitive = scene->primitives;
    for (u32 i = 0; i < scene->counts.primitives; i++, primitive++)
        switch (primitive->type) {
            case PrimitiveType_Coil:
            case PrimitiveType_Helix:
                drawCurve(viewport, Color(primitive->color),
                          &scene->curves[primitive->id], primitive,
                          CURVE_STEPS);
                break;
            case PrimitiveType_Box:
                drawBox(viewport, Color(primitive->color),
                        &scene->boxes[primitive->id], primitive,
                        BOX__ALL_SIDES);
                break;
            case PrimitiveType_Grid:
                drawGrid(viewport, Color(primitive->color),
                         &scene->grids[primitive->id], primitive);
                break;
            default:
                break;
        }
}

void updateScene(Scene *scene, f32 delta_time) {
    static float elapsed = 0;
    elapsed += delta_time;

    Primitive *helix_prim = &scene->primitives[0];
    Primitive *coil_prim  = &scene->primitives[1];
    Primitive *box_prim   = &scene->primitives[2];

    helix_prim->scale.y = 3 + sinf(elapsed * 1.7f);
    helix_prim->scale.x = 1 - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil_prim->scale.x  = 1 + sinf(elapsed * 1.5f + 2) * 0.2f;
    helix_prim->scale.z = helix_prim->scale.x;
    coil_prim->scale.z  = coil_prim->scale.x;

    Curve *coil  = &scene->curves[1];
    coil->thickness = 0.3f - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box_prim->scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    rotatePrimitive(box_prim, rot_speed, -rot_speed, rot_speed);
    rotatePrimitive(coil_prim, -rot_speed, rot_speed, -rot_speed);
    rotatePrimitive(helix_prim, -rot_speed, -rot_speed, rot_speed);
}
void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Scene *scene = &app->scene;
    Viewport *viewport = &app->viewport;

    startFrameTimer(timer);

    updateScene(scene, timer->delta_time);
    drawSceneToViewport(scene, viewport);

    endFrameTimer(timer);
}
void setupViewport(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = 7;
    xf->position.z = -11;
    rotateXform3(xf, 0, -0.2f, 0);
}
void setupScene(Scene *scene) {
    Primitive *helix_primitive = &scene->primitives[0];
    Primitive *coil_primitive  = &scene->primitives[1];
    Primitive *box_primitive   = &scene->primitives[2];
    Primitive *grid_primitive  = &scene->primitives[3];
    helix_primitive->type = PrimitiveType_Helix;
    coil_primitive->type  = PrimitiveType_Coil;
    grid_primitive->type  = PrimitiveType_Grid;
    box_primitive->type   = PrimitiveType_Box;
    box_primitive->color   = Yellow;
    grid_primitive->color  = Green;
    coil_primitive->color  = Magenta;
    helix_primitive->color = Cyan;
    box_primitive->id = grid_primitive->id = helix_primitive->id = 0;
    helix_primitive->position.x = -3;
    helix_primitive->position.y = 4;
    helix_primitive->position.z = 2;
    coil_primitive->position.x = 4;
    coil_primitive->position.y = 4;
    coil_primitive->position.z = 2;
    coil_primitive->id  = 1;
    grid_primitive->scale.x = 5;
    grid_primitive->scale.z = 5;
    Grid *grid = &scene->grids[0];
    Curve *helix = &scene->curves[0];
    Curve *coil  = &scene->curves[1];
    helix->revolution_count = 10;
    coil->revolution_count = 30;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(grid, 11, 11);
}
void initApp(Defaults *defaults) {
    app->on.windowRedraw  = updateAndRender;
    app->on.viewportReady = setupViewport;
    app->on.sceneReady    = setupScene;
    defaults->settings.scene.boxes      = 1;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 4;
}