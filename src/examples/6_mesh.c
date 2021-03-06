#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/viewport/viewport.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/mesh.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void setCountersInHUD(HUD *hud, Timer *timer) {
    printNumberIntoString(timer->average_frames_per_second,      &hud->lines[0].value);
    printNumberIntoString(timer->average_microseconds_per_frame, &hud->lines[1].value);
}
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
void drawScene(Scene *scene, Viewport *viewport) {
    bool normals = app->controls.is_pressed.ctrl;
    Primitive *prim = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, prim++)
        switch (prim->type) {
            case PrimitiveType_Mesh:
                drawMesh(scene->meshes + prim->id, normals, prim,
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

void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    hud->line_height = 1.2f;
    hud->position = Vec2i(10, 10);
    setCountersInHUD(hud, &app->time.timers.update);
    setString(&hud->lines[0].title, (char*)"Fps    : ");
    setString(&hud->lines[1].title, (char*)"mic-s/f: ");
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

    beginFrame(timer);
        if (!mouse->is_captured) manipulateSelection(scene, viewport, controls);
        if (!controls->is_pressed.alt) updateViewport(viewport, mouse);
        beginDrawing(viewport);
            drawScene(scene, viewport);
            drawSelection(scene, viewport, controls);
            setCountersInHUD(&viewport->hud, timer);
        endDrawing(viewport);
    endFrame(timer, mouse);
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
    if (key == 'M' && !is_pressed)
        app->scene.primitives[1].id = app->scene.primitives[2].id = (app->scene.primitives[1].id + 1) % 2;

    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}
void setupScene(Scene *scene) {
    xform3 *xf = &scene->cameras[0].transform;
    xf->position = Vec3(0, 10, -15);
    rotateXform3(xf, 0, -0.2f, 0);

    Primitive *grid = &scene->primitives[0];
    grid->type = PrimitiveType_Grid;
    grid->scale = Vec3(5, 1, 5);
    initGrid(scene->grids,11, 11);

    Primitive *mesh1 = &scene->primitives[1];
    Primitive *mesh2 = &scene->primitives[2];
    Primitive *mesh3 = &scene->primitives[3];
    mesh1->type = PrimitiveType_Mesh;
    mesh1->id = 0;
    mesh1->color = Yellow;
    mesh1->position = Vec3(8, 5, 0);

    mesh2->type = PrimitiveType_Mesh;
    mesh2->id = 0;
    mesh2->color = Cyan;
    mesh2->position = Vec3(-8, 5, 0);

    mesh3->type = PrimitiveType_Mesh;
    mesh3->id = 1;
    mesh3->color = Blue;
    mesh3->position = Vec3(0, 5, 5);
}

void initApp(Defaults *defaults) {
    static String mesh_files[2];
    static char string_buffers[2][100];
    String *mesh1 = &mesh_files[0];
    String *mesh2 = &mesh_files[1];
    mesh1->char_ptr = string_buffers[0];
    mesh2->char_ptr = string_buffers[1];
    u32 offset = getDirectoryLength((char*)__FILE__);
    mergeString(mesh1, (char*)__FILE__, (char*)"suzanne.mesh", offset);
    mergeString(mesh2, (char*)__FILE__, (char*)"dragon.mesh",  offset);
    defaults->settings.scene.mesh_files = mesh_files;
    defaults->settings.scene.meshes = 2;
    defaults->settings.scene.grids  = 1;
    defaults->settings.scene.primitives = 4;
    defaults->settings.viewport.hud_line_count = 2;
    defaults->settings.viewport.hud_default_color = Green;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
}