#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/mesh.h"
#include "../SlimEngine/scene/curve.h"
#include "../SlimEngine/viewport/hud.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

u64 scene_io_time;
bool last_scene_io_is_save;

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
    Primitive *primitive = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, primitive++)
        switch (primitive->type) {
            case PrimitiveType_Mesh:
                drawMesh(viewport, Color(primitive->color),
                         &scene->meshes[primitive->id], primitive, false);
                break;
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
    f64 now = (f64)app->time.getTicks();
    f64 tps = (f64)app->time.ticks.per_second;
    if ((now - (f64)scene_io_time) / tps <= 2.0) {
        PixelGrid *canvas = viewport->frame_buffer;
        char *message;
        RGBA color;
        if (last_scene_io_is_save) {
            message = "Scene saved to: this.scene";
            color = Color(Yellow);
        } else {
            message = "Scene loaded from: this.scene";
            color = Color(Cyan);
        }
        i32 x = canvas->dimensions.width / 2 - 150;
        i32 y = 20;
        drawText(canvas, color, message, x, y);
    }
    resetMouseChanges(mouse);
    endFrameTimer(timer);
}
void setupScene(Scene *scene) {
    Primitive *grid_primitive  = &scene->primitives[0];
    Primitive *dragon_prim     = &scene->primitives[1];
    Primitive *suzanne1_prim   = &scene->primitives[2];
    Primitive *suzanne2_prim   = &scene->primitives[3];
    Primitive *helix_primitive = &scene->primitives[4];
    Primitive *coil_primitive  = &scene->primitives[5];
    Primitive *box_primitive   = &scene->primitives[6];

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
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(grid, 11, 11);

    Curve *helix = &scene->curves[0];
    Curve *coil  = &scene->curves[1];
    helix->revolution_count = 10;
    coil->revolution_count = 30;

    suzanne1_prim->position.x = 10;
    suzanne1_prim->position.z = 5;
    suzanne1_prim->position.y = 4;
    suzanne1_prim->type = PrimitiveType_Mesh;
    suzanne1_prim->color = Magenta;

    *suzanne2_prim = *suzanne1_prim;
    suzanne2_prim->position.x = -10;
    suzanne2_prim->color = Cyan;

    *dragon_prim = *suzanne1_prim;
    dragon_prim->id = 1;
    dragon_prim->position.z = 10;
    dragon_prim->color = Blue;
}
void onKeyChanged(u8 key, bool is_pressed) {
    Scene *scene = &app->scene;
    Platform *platform = &app->platform;
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

    if (app->controls.is_pressed.ctrl &&
        !is_pressed && key == 'S' || key == 'Z') {
        last_scene_io_is_save = key == 'S';
        char *file = scene->settings.file.char_ptr;
        if (last_scene_io_is_save)
            saveSceneToFile(  scene, file, platform);
        else
            loadSceneFromFile(scene, file, platform);
        scene_io_time = app->time.getTicks();
    }
}
String file_paths[2];
char string_buffers[3][100];
void initApp(Defaults *defaults) {
    String *scene_file = &defaults->settings.scene.file;
    String *mesh_file1 = &file_paths[0];
    String *mesh_file2 = &file_paths[1];
    mesh_file1->char_ptr = string_buffers[0];
    mesh_file2->char_ptr = string_buffers[1];
    scene_file->char_ptr = string_buffers[2];
    u32 offset = getDirectoryLength(__FILE__);
    mergeString(mesh_file1, __FILE__, "suzanne.mesh", offset);
    mergeString(mesh_file2, __FILE__, "dragon.mesh",  offset);
    mergeString(scene_file, __FILE__, "this.scene",   offset);
    defaults->settings.scene.mesh_files = file_paths;
    defaults->additional_memory_size  = Megabytes(3);
    defaults->additional_memory_size += Kilobytes(64);
    defaults->settings.scene.meshes     = 2;
    defaults->settings.scene.boxes      = 1;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 7;
    defaults->settings.viewport.hud_line_count = 2;
    scene_io_time = 0;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
}