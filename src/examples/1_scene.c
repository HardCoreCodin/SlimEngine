#include "../SlimEngine/app.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void updateScene(Scene *scene, f32 delta_time) {
    static float elapsed = 0;
    elapsed += delta_time;

    Primitive *helix_prim = &scene->primitives[0];
    Primitive *coil_prim  = &scene->primitives[1];
    Primitive *box_prim   = &scene->primitives[2];
    Primitive *grid_prim  = &scene->primitives[3];

    helix_prim->scale.y = 3 + sinf(elapsed * 1.7f);
    helix_prim->scale.x = helix_prim->scale.z = 1 - sinf(elapsed * 1.6f + 1) * 0.3f;
    coil_prim->scale.x  = coil_prim->scale.z  = 1 + sinf(elapsed * 1.5f + 2) * 0.2f;

    Curve *coil  = &scene->curves[1];
    coil->thickness = 0.3f - sinf(elapsed * 1.4f + 3) * 0.1f;

    f32 rot_speed = delta_time * 0.5f;
    box_prim->scale = getVec3Of(1 + sinf(elapsed * 2) * 0.2f);
    grid_prim->rotation.axis.y = 0.5f;
    transformPrimitive(box_prim, rot_speed, -rot_speed, rot_speed);
    transformPrimitive(coil_prim, -rot_speed, rot_speed, -rot_speed);
    transformPrimitive(helix_prim, -rot_speed, -rot_speed, rot_speed);
}
void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    startFrameTimer(timer);
    updateScene(&app->scene, timer->delta_time);
    drawSceneToViewport(&app->scene, &app->viewport);
    endFrameTimer(timer);
}
void setupScene(Scene *scene) {
    Primitive *helix_prim = &scene->primitives[0];
    Primitive *coil_prim  = &scene->primitives[1];
    Primitive *box_prim   = &scene->primitives[2];
    Primitive *grid_prim  = &scene->primitives[3];
    helix_prim->type = PrimitiveType_Helix;
    coil_prim->type  = PrimitiveType_Coil;
    grid_prim->type  = PrimitiveType_Grid;
    box_prim->type   = PrimitiveType_Box;
    box_prim->color   = Yellow;
    grid_prim->color  = Green;
    coil_prim->color  = Magenta;
    helix_prim->color = Cyan;
    helix_prim->position.x = -3;
    helix_prim->position.y = 4;
    helix_prim->position.z = 2;
    coil_prim->position.x = 4;
    coil_prim->position.y = 4;
    coil_prim->position.z = 2;
    box_prim->id = grid_prim->id = helix_prim->id = 0;
    coil_prim->id  = 1;
    scene->curves[0].revolution_count = 10;
    scene->curves[1].revolution_count = 15;
    initGrid(scene->grids,
             -5,-5,
             +5,+5,
             11,
             11);
}
void setupCamera(Viewport *viewport) {
    xform3 *xf = &viewport->camera->transform;
    xf->position.y = 7;
    xf->position.z = -11;
    rotateXform3(xf, 0, -0.2f, 0);
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