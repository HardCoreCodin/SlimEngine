#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/viewport/viewport.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/scene/box.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/curve.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void drawScene(Scene *scene, Viewport *viewport) {
    Primitive *prim = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, prim++)
        switch (prim->type) {
            case PrimitiveType_Coil:
            case PrimitiveType_Helix:
                drawCurve(scene->curves + prim->id, CURVE_STEPS, prim,
                          Color(prim->color), 0.5f, 0, viewport);
                break;
            case PrimitiveType_Box:
                drawBox(scene->boxes + prim->id, BOX__ALL_SIDES, prim,
                        Color(prim->color), 0.5f, 0, viewport);
                break;
            case PrimitiveType_Grid:
                drawGrid(scene->grids + prim->id, prim,
                         Color(prim->color), 0.5f, 0, viewport);
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

    beginFrame(timer);
        updateScene(scene, timer->delta_time);
        beginDrawing(viewport);
            drawScene(scene, viewport);
        endDrawing(viewport);
    endFrame(timer, &app->controls.mouse);
}
void setupScene(Scene *scene) {
    xform3 *xf = &scene->cameras[0].transform;
    xf->position = Vec3(0, 8, -13);
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
    app->on.windowRedraw  = updateAndRender;
}