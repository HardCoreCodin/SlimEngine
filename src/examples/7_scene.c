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
void drawSceneToViewport(Scene *scene, Viewport *viewport) {
    fillPixelGrid(viewport->frame_buffer, Color(Black));
    Primitive *primitive = scene->primitives;
    for (u32 i = 0; i < scene->settings.primitives; i++, primitive++)
        switch (primitive->type) {
            case PrimitiveType_Mesh:
                drawMesh(viewport, Color(primitive->color),
                         &scene->meshes[primitive->id], primitive, false, 0);
                break;
            case PrimitiveType_Coil:
            case PrimitiveType_Helix:
                drawCurve(viewport, Color(primitive->color),
                          &scene->curves[primitive->id], primitive,
                          CURVE_STEPS, 0);
                break;
            case PrimitiveType_Box:
                drawBox(viewport, Color(primitive->color),
                        &scene->boxes[primitive->id], primitive,
                        BOX__ALL_SIDES, 0);
                break;
            case PrimitiveType_Grid:
                drawGrid(viewport, Color(primitive->color),
                         &scene->grids[primitive->id], primitive, 0);
                break;
            default:
                break;
        }
}
void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    hud->line_height = 1.2f;
    hud->position = Vec2i(10, 10);
    HUDLine *line = hud->lines;
    for (u8 i = 0; i < hud->line_count; i++, line++) {
        setString(&line->alternate_value, "On");
        setString(&line->value.string, "Off");
        line->alternate_value_color = Green;
        line->value_color = DarkGreen;
        line->use_alternate = i ?
                &viewport->settings.depth_sort :
                &viewport->settings.antialias;
        setString(&line->title, i ? "Z-Sort : " : "AALines: ");
    }
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

    startFrameTimer(timer);

    if (!mouse->is_captured)
        manipulateSelection(scene, viewport, controls);

    if (!controls->is_pressed.alt)
        updateViewport(viewport, mouse);

    drawSceneToViewport(scene, viewport);
    drawSelection(scene, viewport, controls);

    if (viewport->settings.show_hud)
        drawHUD(viewport->frame_buffer, &viewport->hud);

    f64 now = (f64)app->time.getTicks();
    f64 tps = (f64)app->time.ticks.per_second;
    if ((now - (f64)scene->last_io_ticks) / tps <= 2.0) {
        PixelGrid *canvas = viewport->frame_buffer;
        char *message;
        RGBA color;
        if (scene->last_io_is_save) {
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
    Primitive *dragon   = &scene->primitives[1];
    Primitive *suzanne1 = &scene->primitives[2];
    Primitive *suzanne2 = &scene->primitives[3];
    Primitive *grid  = &scene->primitives[0];
    Primitive *helix = &scene->primitives[4];
    Primitive *coil  = &scene->primitives[5];
    Primitive *box   = &scene->primitives[6];

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

    xform3 *xf = &scene->cameras[0].transform;
    xf->position = Vec3(0, 7, -11);
    rotateXform3(xf, 0, -0.2f, 0);

    initGrid(scene->grids,11, 11);
    rotatePrimitive(grid, 0.5f, 0, 0);

    suzanne1->position = Vec3(10, 5, 4);
    suzanne1->color = Magenta;
    suzanne1->type = PrimitiveType_Mesh;
    suzanne1->id = 0;

    *suzanne2 = *suzanne1;
    suzanne2->color = Cyan;
    suzanne2->position.x = -10;

    *dragon = *suzanne1;
    dragon->id = 1;
    dragon->position.z = 10;
    dragon->color = Blue;
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

    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed) {
        u8 tab = app->controls.key_map.tab;
        if (key == tab) settings->show_hud = !settings->show_hud;
        if (key == '1') settings->antialias = !settings->antialias;
        if (key == '2') settings->depth_sort = !settings->depth_sort;
    }

    Scene *scene = &app->scene;
    Platform *platform = &app->platform;
    if (app->controls.is_pressed.ctrl &&
        !is_pressed && key == 'S' || key == 'Z') {
        scene->last_io_is_save = key == 'S';
        char *file = scene->settings.file.char_ptr;
        if (scene->last_io_is_save)
            saveSceneToFile(  scene, file, platform);
        else
            loadSceneFromFile(scene, file, platform);
        scene->last_io_ticks = app->time.getTicks();
    }
}
void initApp(Defaults *defaults) {
    static String files[2];
    static char string_buffers[3][100];
    String *scene = &defaults->settings.scene.file;
    String *mesh1 = &files[0];
    String *mesh2 = &files[1];
    mesh1->char_ptr = string_buffers[0];
    mesh2->char_ptr = string_buffers[1];
    scene->char_ptr = string_buffers[2];
    u32 offset = getDirectoryLength(__FILE__);
    mergeString(scene, __FILE__, "this.scene",   offset);
    mergeString(mesh2, __FILE__, "dragon.mesh",  offset);
    mergeString(mesh1, __FILE__, "suzanne.mesh", offset);
    defaults->settings.scene.mesh_files = files;
    defaults->settings.scene.meshes     = 2;
    defaults->settings.scene.boxes      = 1;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.curves     = 2;
    defaults->settings.scene.primitives = 7;
    defaults->settings.viewport.hud_line_count = 2;
    defaults->settings.viewport.hud_default_color = Green;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onButtonDown;
    app->on.mouseButtonDoubleClicked = onDoubleClick;
    app->on.windowRedraw  = updateAndRender;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
}