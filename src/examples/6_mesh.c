#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/mesh.h"
#include "../SlimEngine/viewport/hud.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void setCountersInHUD(HUD *hud, Timer *timer) {
    printNumberIntoString(timer->average_frames_per_second,      &hud->lines[0].value);
    printNumberIntoString(timer->average_microseconds_per_frame, &hud->lines[1].value);
}
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
void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));

    bool normals = app->controls.is_pressed.ctrl;
    Primitive *primitive = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, primitive++)
        switch (primitive->type) {
            case PrimitiveType_Mesh:
                drawMesh(viewport, Color(primitive->color),
                         &scene->meshes[primitive->id], primitive, normals);
                break;
            case PrimitiveType_Grid:
                drawGrid(viewport, Color(primitive->color),
                         &scene->grids[primitive->id], primitive);
                break;
            default:
                break;
        }
}

void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    hud->line_height = 1.2f;
    hud->position.x = hud->position.y = 10;
    setCountersInHUD(hud, &app->time.timers.update);
    setString(&hud->lines[0].title, "Fps    : ");
    setString(&hud->lines[1].title, "mic-s/f: ");
    hud->lines[0].title_color = hud->lines[1].title_color = Green;
    hud->lines[0].value_color = hud->lines[1].value_color = Green;

    xform3 *camera_xform = &viewport->camera->transform;
    camera_xform->position.y = 7;
    camera_xform->position.z = -11;
    rotateXform3(camera_xform, 0, -0.2f, 0);
}
void updateViewport(Viewport *viewport, Mouse *mouse) {
    if (mouse->is_captured) {
        if (mouse->moved)         orientViewport(viewport, mouse);
        if (mouse->wheel_scrolled)  zoomViewport(viewport, mouse);
    } else {
        if (mouse->wheel_scrolled) dollyViewport(viewport, mouse);
        if (mouse->moved) {
            if (mouse->middle_button.is_pressed)
                panViewport(viewport, mouse);

            if (mouse->right_button.is_pressed &&
                !app->controls.is_pressed.alt)
                orbitViewport(viewport, mouse);
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

    if (mouse->is_captured)
        navigateViewport(viewport, timer->delta_time);
    else
        manipulateSelection(scene, viewport, controls);

    if (!controls->is_pressed.alt)
        updateViewport(viewport, mouse);

    drawSceneToViewport(scene, viewport);
    drawSelection(scene, viewport, controls);

    if (viewport->settings.show_hud) {
        setCountersInHUD(&viewport->hud, timer);
        drawHUD(viewport->frame_buffer, &viewport->hud);
    }

    resetMouseChanges(mouse);
    endFrameTimer(timer);
}
void onKeyChanged(u8 key, bool is_pressed) {
    NavigationMove *move = &app->viewport.navigation.move;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;

    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}
void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    Primitive *suzanne1_prim  = &scene->primitives[1];
    Primitive *suzanne2_prim  = &scene->primitives[2];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale.x = 5;
    grid_primitive->scale.z = 5;
    initGrid(scene->grids,11, 11);

    suzanne1_prim->position.x = 10;
    suzanne1_prim->position.z = 5;
    suzanne1_prim->position.y = 4;
    suzanne1_prim->type = PrimitiveType_Mesh;
    suzanne1_prim->color = Magenta;

    *suzanne2_prim = *suzanne1_prim;
    suzanne2_prim->position.x = -10;
    suzanne2_prim->color = Cyan;
}

String mesh_file;
char string_buffer[100];

void initApp(Defaults *defaults) {
    mesh_file.char_ptr = string_buffer;
    u32 offset = getDirectoryLength(__FILE__);
    mergeString(&mesh_file, __FILE__, "suzanne.mesh", offset);
    defaults->settings.scene.mesh_files = &mesh_file;
    defaults->settings.scene.meshes = 1;
    defaults->settings.scene.grids  = 1;
    defaults->settings.scene.primitives = 3;
    defaults->settings.viewport.hud_line_count = 2;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
}