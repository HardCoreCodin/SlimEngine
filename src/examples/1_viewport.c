#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/core/string.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/viewport/hud.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

void updateHudVisibility(u8 key, bool is_pressed) {
    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}

enum HUDLineIndex { Fps, Mfs, Width, Height, MouseX, MouseY };

void setDimensionsInHUD(HUD *hud, i32 width, i32 height) {
    printNumberIntoString(width,  &hud->lines[Width ].value);
    printNumberIntoString(height, &hud->lines[Height].value);
}
void setMouseInHUD(HUD *hud, vec2i mouse_pos) {
    printNumberIntoString(mouse_pos.x, &hud->lines[MouseX].value);
    printNumberIntoString(mouse_pos.y, &hud->lines[MouseY].value);
}
void updateMouseCoordsInHUD(i32 x, i32 y) {
    setMouseInHUD(&app->viewport.hud, app->controls.mouse.pos);
}
void updateSizeInHUD(u16 width, u16 height) {
    setDimensionsInHUD(&app->viewport.hud, width, height);
}
void setCountersInHUD(HUD *hud, Timer *timer) {
    printNumberIntoString(timer->average_frames_per_second,      &hud->lines[Fps].value);
    printNumberIntoString(timer->average_microseconds_per_frame, &hud->lines[Mfs].value);
}
void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    hud->line_height = 1.2f;
    hud->position = Vec2i(10, 10);

    Dimensions *dim = &viewport->frame_buffer->dimensions;
    setDimensionsInHUD(hud, dim->width, dim->height);
    setCountersInHUD(  hud, &app->time.timers.update);
    setMouseInHUD(     hud, app->controls.mouse.pos);

    HUDLine *lines = hud->lines;
    setString(&lines[Fps   ].title, "Fps    : ");
    setString(&lines[Mfs   ].title, "mic-s/f: ");
    setString(&lines[Width ].title, "Width  : ");
    setString(&lines[Height].title, "Height : ");
    setString(&lines[MouseX].title, "Mouse X: ");
    setString(&lines[MouseY].title, "Mouse Y: ");
}

void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    startFrameTimer(timer);

    Viewport *viewport = &app->viewport;

    fillPixelGrid(viewport->frame_buffer, Color(Black), 1);

    Grid      *grid = &app->scene.grids[0];
    Primitive *prim = &app->scene.primitives[0];
    drawGrid(viewport, Color(prim->color), 1, grid, prim, 1);
    preparePixelGridForDisplay(viewport->frame_buffer);

    if (viewport->settings.show_hud) {
        setCountersInHUD(&viewport->hud, timer);
        drawHUD(viewport->frame_buffer, &viewport->hud);
    }

    endFrameTimer(timer);
}

void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale    = Vec3(5, 1, 5);
    grid_primitive->position = Vec3(0, 0, 5);
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(&scene->grids[0],11, 11);

    xform3 *camera_xform = &scene->cameras[0].transform;
    camera_xform->position = Vec3(0, 7, -11);
    rotateXform3(camera_xform, 0, -0.2f, 0);
}
void initApp(Defaults *defaults) {
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
    app->on.windowRedraw = updateAndRender;
    app->on.windowResize = updateSizeInHUD;
    app->on.keyChanged   = updateHudVisibility;
    app->on.mouseMovementSet = updateMouseCoordsInHUD;
    defaults->settings.viewport.hud_default_color = Green;
    defaults->settings.viewport.hud_line_count = 6;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}