#pragma once

#include "./core/init.h"
#include "./scene/io.h"

App *app;

void initApp(Defaults *defaults);

void _windowRedraw() {
    if (!app->is_running) return;
    if (app->on.windowRedraw) app->on.windowRedraw();
}

void _windowResize(u16 width, u16 height) {
    if (!app->is_running) return;
    updateDimensions(&app->window_content.dimensions, width, height);

    if (app->on.windowResize) app->on.windowResize(width, height);
    if (app->on.windowRedraw) app->on.windowRedraw();
}

void _keyChanged(u8 key, bool pressed) {
         if (key == app->controls.key_map.ctrl)  app->controls.is_pressed.ctrl  = pressed;
    else if (key == app->controls.key_map.alt)   app->controls.is_pressed.alt   = pressed;
    else if (key == app->controls.key_map.shift) app->controls.is_pressed.shift = pressed;
    else if (key == app->controls.key_map.space) app->controls.is_pressed.space = pressed;
    else if (key == app->controls.key_map.tab)   app->controls.is_pressed.tab   = pressed;

    if (app->on.keyChanged) app->on.keyChanged(key, pressed);
}

void _mouseButtonDown(MouseButton *mouse_button, i32 x, i32 y) {
    mouse_button->is_pressed = true;
    mouse_button->is_handled = false;

    mouse_button->down_pos.x = x;
    mouse_button->down_pos.y = y;

    if (app->on.mouseButtonDown) app->on.mouseButtonDown(mouse_button);
}

void _mouseButtonUp(MouseButton *mouse_button, i32 x, i32 y) {
    mouse_button->is_pressed = false;
    mouse_button->is_handled = false;

    mouse_button->up_pos.x = x;
    mouse_button->up_pos.y = y;

    if (app->on.mouseButtonUp) app->on.mouseButtonUp(mouse_button);
}

void _mouseButtonDoubleClicked(MouseButton *mouse_button, i32 x, i32 y) {
    app->controls.mouse.double_clicked = true;
    mouse_button->double_click_pos.x = x;
    mouse_button->double_click_pos.y = y;
    if (app->on.mouseButtonDoubleClicked) app->on.mouseButtonDoubleClicked(mouse_button);
}

void _mouseWheelScrolled(f32 amount) {
    app->controls.mouse.wheel_scroll_amount += amount * 100;
    app->controls.mouse.wheel_scrolled = true;

    if (app->on.mouseWheelScrolled) app->on.mouseWheelScrolled(amount);
}

void _mousePositionSet(i32 x, i32 y) {
    app->controls.mouse.pos.x = x;
    app->controls.mouse.pos.y = y;

    if (app->on.mousePositionSet) app->on.mousePositionSet(x, y);
}

void _mouseMovementSet(i32 x, i32 y) {
    app->controls.mouse.movement.x = x - app->controls.mouse.pos.x;
    app->controls.mouse.movement.y = y - app->controls.mouse.pos.y;
    app->controls.mouse.moved = true;

    if (app->on.mouseMovementSet) app->on.mouseMovementSet(x, y);
}

void _mouseRawMovementSet(i32 x, i32 y) {
    app->controls.mouse.pos_raw_diff.x += x;
    app->controls.mouse.pos_raw_diff.y += y;
    app->controls.mouse.moved = true;

    if (app->on.mouseRawMovementSet) app->on.mouseRawMovementSet(x, y);
}

bool initAppMemory(u64 size) {
    if (app->memory.address) return false;

    void* memory_address = app->platform.getMemory(size);
    if (!memory_address) {
        app->is_running = false;
        return false;
    }

    initMemory(&app->memory, (u8*)memory_address, size);
    return true;
}

void* allocateAppMemory(u64 size) {
    void *new_memory = allocateMemory(&app->memory, size);
    if (new_memory) return new_memory;

    app->is_running = false;
    return null;
}

void initScene(Scene *scene, SceneSettings settings, Memory *memory, Platform *platform) {
    scene->settings = settings;
    scene->primitives = null;
    scene->cameras    = null;
    scene->curves     = null;
    scene->boxes      = null;
    scene->grids      = null;
    scene->meshes     = null;

    scene->selection.object_type = scene->selection.object_id = 0;
    scene->selection.changed = false;

    if (settings.meshes && settings.mesh_files) {
        scene->meshes = allocateMemory(memory, sizeof(Mesh) * scene->settings.meshes);
        for (u32 i = 0; i < settings.meshes; i++)
            loadMeshFromFile(&scene->meshes[i], settings.mesh_files[i].char_ptr, platform, memory);
    }

    if (settings.cameras) {
        scene->cameras = allocateMemory(memory, sizeof(Camera) * settings.cameras);
        if (scene->cameras)
            for (u32 i = 0; i < settings.cameras; i++)
                initCamera(scene->cameras + i);
    }

    if (settings.primitives) {
        scene->primitives = allocateMemory(memory, sizeof(Primitive) * settings.primitives);
        if (scene->primitives)
            for (u32 i = 0; i < settings.primitives; i++)
                initPrimitive(scene->primitives + i);
    }

    if (settings.curves) {
        scene->curves = allocateMemory(memory, sizeof(Curve) * settings.curves);
        if (scene->curves)
            for (u32 i = 0; i < settings.curves; i++)
                initCurve(scene->curves + i);
    }

    if (settings.boxes) {
        scene->boxes = allocateMemory(memory, sizeof(Box) * settings.boxes);
        if (scene->boxes)
            for (u32 i = 0; i < settings.boxes; i++)
                initBox(scene->boxes + i);
    }

    if (settings.grids) {
        scene->grids = allocateMemory(memory, sizeof(Grid) * settings.grids);
        if (scene->grids)
            for (u32 i = 0; i < settings.grids; i++)
                initGrid(scene->grids + i, 3, 3);
    }
}

void _initApp(Defaults *defaults, void* window_content_memory) {
    app->is_running = true;
    app->user_data = null;
    app->memory.address = null;

    app->on.sceneReady = null;
    app->on.viewportReady = null;
    app->on.windowRedraw = null;
    app->on.keyChanged = null;
    app->on.mouseButtonUp = null;
    app->on.mouseButtonDown = null;
    app->on.mouseButtonDoubleClicked = null;
    app->on.mouseWheelScrolled = null;
    app->on.mousePositionSet = null;
    app->on.mouseMovementSet = null;
    app->on.mouseRawMovementSet = null;

    initTime(&app->time, app->platform.getTicks, app->platform.ticks_per_second);
    initMouse(&app->controls.mouse);
    initPixelGrid(&app->window_content, (Pixel*)window_content_memory);

    defaults->title = "";
    defaults->width = 480;
    defaults->height = 360;
    defaults->additional_memory_size = 0;
    defaults->settings.scene      = getDefaultSceneSettings();
    defaults->settings.viewport   = getDefaultViewportSettings();
    defaults->settings.navigation = getDefaultNavigationSettings();

    initApp(defaults);
    initAppMemory(defaults->additional_memory_size +
            defaults->settings.scene.primitives * sizeof(Primitive) +
            defaults->settings.scene.curves * sizeof(Curve) +
            defaults->settings.scene.boxes * sizeof(Box) +
            defaults->settings.scene.grids * sizeof(Grid) +
            defaults->settings.scene.cameras * sizeof(Camera) +
            defaults->settings.viewport.hud_line_count * sizeof(HUDLine));
    initScene(&app->scene, defaults->settings.scene, &app->memory, &app->platform);
    if (app->on.sceneReady) app->on.sceneReady(&app->scene);
    HUDLine *hud_lines = defaults->settings.viewport.hud_line_count ?
                         allocateAppMemory(defaults->settings.viewport.hud_line_count * sizeof(HUDLine)) : null;
    initViewport(&app->viewport,
                 defaults->settings.viewport,
                 defaults->settings.navigation,
                 app->scene.cameras,
                 &app->window_content,
                 hud_lines,
                 defaults->settings.viewport.hud_line_count);
    if (app->on.viewportReady) app->on.viewportReady(&app->viewport);
}

#include "./platforms/win32.h"