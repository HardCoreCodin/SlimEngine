#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/core/string.h"
#include "../SlimEngine/viewport/viewport.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

enum HUDLineIndex { Fps, Mfs, Width, Height, MouseX, MouseY };
void updateHudVisibility(u8 key, bool is_pressed) {
    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}
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
    printNumberIntoString(timer->average_frames_per_second,
                          &hud->lines[Fps].value);
    printNumberIntoString(timer->average_microseconds_per_frame,
                          &hud->lines[Mfs].value);
}
void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    hud->line_height = 1.2f;
    hud->position = Vec2i(10, 10);

    Dimensions *dim = &viewport->dimensions;
    setDimensionsInHUD(hud, dim->width, dim->height);
    setCountersInHUD(  hud, &app->time.timers.update);
    setMouseInHUD(     hud, app->controls.mouse.pos);

    HUDLine *lines = hud->lines;
    setString(&lines[Fps   ].title, (char*)"Fps    : ");
    setString(&lines[Mfs   ].title, (char*)"mic-s/f: ");
    setString(&lines[Width ].title, (char*)"Width  : ");
    setString(&lines[Height].title, (char*)"Height : ");
    setString(&lines[MouseX].title, (char*)"Mouse X: ");
    setString(&lines[MouseY].title, (char*)"Mouse Y: ");
}

void updateAndRender() {
    Timer *timer = &app->time.timers.update;
    Viewport *viewport = &app->viewport;
    Grid      *grid = app->scene.grids;
    Primitive *prim = app->scene.primitives;

    beginFrame(timer);
        beginDrawing(viewport);
            drawGrid(grid, prim, Color(prim->color),
                     0.5f, 0, viewport);
            setCountersInHUD(&viewport->hud, timer);
        endDrawing(viewport);
    endFrame(timer, &app->controls.mouse);
}

void setupScene(Scene *scene) {
    Primitive *grid_prim = scene->primitives;
    grid_prim->type = PrimitiveType_Grid;
    grid_prim->scale    = Vec3(5, 1, 5);
    grid_prim->position = Vec3(0, 0, 5);
    rotatePrimitive(grid_prim, 0.5f, 0, 0);
    initGrid(scene->grids,11, 11);

    xform3 *camera_xform = &scene->cameras->transform;
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