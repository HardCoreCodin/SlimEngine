#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/viewport/hud.h"
// Or using the single-header file:
// #include "../SlimEngine.h"

enum HUDLineIndex {
    Fps, Mfs, Width, Height, MouseX, MouseY
};
void setCountersInHUD(HUD *hud, Timer *timer) {
    printNumberIntoString(timer->average_frames_per_second,
                          &hud->lines[Fps].value);
    printNumberIntoString(timer->average_microseconds_per_frame,
                          &hud->lines[Mfs].value);
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
void setupViewport(Viewport *viewport) {
    HUD *hud = &viewport->hud;
    HUDLine *lines = hud->lines;
    Dimensions *dim = &viewport->frame_buffer->dimensions;
    xform3 *camera_xform = &viewport->camera->transform;
    camera_xform->position.y = 7;
    camera_xform->position.z = -11;
    rotateXform3(camera_xform, 0, -0.2f, 0);
    hud->line_height = 1.2f;
    hud->position.x = hud->position.y = 10;
    setDimensionsInHUD(hud, dim->width, dim->height);
    setCountersInHUD(  hud, &app->time.timers.update);
    setMouseInHUD(     hud, app->controls.mouse.pos);
    setString(&lines[Fps   ].title, "Fps    : ");
    setString(&lines[Mfs   ].title, "mic-s/f: ");
    setString(&lines[Width ].title, "Width  : ");
    setString(&lines[Height].title, "Height : ");
    setString(&lines[MouseX].title, "Mouse X: ");
    setString(&lines[MouseY].title, "Mouse Y: ");
    for (u32 i = 0; i < hud->line_count; i++)
        lines[i].title_color = lines[i].value_color = Green;
}
void updateAndRender() {
    Viewport *viewport = &app->viewport;
    PixelGrid *canvas = viewport->frame_buffer;
    HUD *hud = &viewport->hud;
    Grid *grid = &app->scene.grids[0];
    Primitive *prim = &app->scene.primitives[0];
    startFrameTimer(&app->time.timers.update);

    fillPixelGrid(canvas, Color(Black));
    drawGrid(viewport, Color(prim->color), grid, prim);
    if (viewport->settings.show_hud) {
        setCountersInHUD(hud, &app->time.timers.update);
        drawHUD(canvas, hud);
    }

    endFrameTimer(&app->time.timers.update);
}
void updateHudVisibility(u8 key, bool is_pressed) {
    ViewportSettings *settings = &app->viewport.settings;
    if (!is_pressed && key == app->controls.key_map.tab)
        settings->show_hud = !settings->show_hud;
}
void setupScene(Scene *scene) {
    Primitive *grid_primitive = &scene->primitives[0];
    grid_primitive->type = PrimitiveType_Grid;
    grid_primitive->scale.x = 5;
    grid_primitive->scale.z = 5;
    grid_primitive->position.z = 5;
    rotatePrimitive(grid_primitive, 0.5f, 0, 0);
    initGrid(scene->grids,11, 11);
}
void initApp(Defaults *defaults) {
    app->on.windowRedraw     = updateAndRender;
    app->on.windowResize     = updateSizeInHUD;
    app->on.keyChanged       = updateHudVisibility;
    app->on.mouseMovementSet = updateMouseCoordsInHUD;
    app->on.sceneReady       = setupScene;
    app->on.viewportReady    = setupViewport;
    defaults->settings.viewport.hud_line_count = 6;
    defaults->settings.scene.grids      = 1;
    defaults->settings.scene.primitives = 1;
}