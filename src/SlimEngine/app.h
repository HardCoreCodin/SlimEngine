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
    updateDimensions(&app->viewport.dimensions, width, height, width);

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

void initScene(Scene *scene, SceneSettings *settings, Memory *memory, Platform *platform) {
    scene->settings   = *settings;
    scene->primitives = null;
    scene->cameras    = null;
    scene->curves     = null;
    scene->boxes      = null;
    scene->grids      = null;
    scene->meshes     = null;
    scene->textures   = null;

    scene->selection = (Selection*)allocateMemory(memory, sizeof(Selection));
    scene->selection->object_type = scene->selection->object_id = 0;
    scene->selection->changed = false;

    if (settings->meshes && settings->mesh_files) {
        scene->meshes = (Mesh*)allocateMemory(memory, sizeof(Mesh) * settings->meshes);
        for (u32 i = 0; i < settings->meshes; i++)
            loadMeshFromFile(&scene->meshes[i], settings->mesh_files[i].char_ptr, platform, memory);
    }

    if (settings->cameras) {
        scene->cameras = (Camera*)allocateMemory(memory, sizeof(Camera) * settings->cameras);
        if (scene->cameras)
            for (u32 i = 0; i < settings->cameras; i++)
                initCamera(scene->cameras + i);
    }

    if (settings->primitives) {
        scene->primitives = (Primitive*)allocateMemory(memory, sizeof(Primitive) * settings->primitives);
        if (scene->primitives)
            for (u32 i = 0; i < settings->primitives; i++) {
                initPrimitive(scene->primitives + i);
                scene->primitives[i].id = i;
            }
    }

    if (settings->curves) {
        scene->curves = (Curve*)allocateMemory(memory, sizeof(Curve) * settings->curves);
        if (scene->curves)
            for (u32 i = 0; i < settings->curves; i++)
                initCurve(scene->curves + i);
    }

    if (settings->boxes) {
        scene->boxes = (Box*)allocateMemory(memory, sizeof(Box) * settings->boxes);
        if (scene->boxes)
            for (u32 i = 0; i < settings->boxes; i++)
                initBox(scene->boxes + i);
    }

    if (settings->grids) {
        scene->grids = (Grid*)allocateMemory(memory, sizeof(Grid) * settings->grids);
        if (scene->grids)
            for (u32 i = 0; i < settings->grids; i++)
                initGrid(scene->grids + i, 3, 3);
    }

    if (settings->lights) {
        Light *light = scene->lights = (Light*)allocateMemory(memory, sizeof(Light) * settings->lights);
        for (u32 i = 0; i < settings->lights; i++, light++) {
            for (u8 c = 0; c < 3; c++) {
                light->position_or_direction.components[c] = 0;
                light->color.components[c]                 = 1;
                light->attenuation.components[c]           = 1;
            }
            light->intensity = 1;
            light->is_directional = false;
        }
    }

    if (settings->materials)   {
        Material *material = scene->materials = (Material*)allocateMemory(memory, sizeof(Material) * settings->materials);
        for (u32 i = 0; i < settings->materials; i++, material++)
            initMaterial(material);
    }

    if (settings->textures && settings->texture_files) {
        scene->textures = (Texture*)allocateMemory(memory, sizeof(Texture) * settings->textures);
        for (u32 i = 0; i < settings->textures; i++)
            loadTextureFromFile(&scene->textures[i], settings->texture_files[i].char_ptr, platform, memory);
    }

    scene->last_io_ticks = 0;
    scene->last_io_is_save = false;
}


void _initApp(Defaults *defaults, u32* window_content) {
    app->window_content = window_content;

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

    defaults->title = (char*)"";
    defaults->width = 480;
    defaults->height = 360;
    defaults->additional_memory_size = 0;

    SceneSettings *scene_settings = &defaults->settings.scene;
    ViewportSettings *viewport_settings = &defaults->settings.viewport;
    NavigationSettings *navigation_settings = &defaults->settings.navigation;

    setDefaultSceneSettings(scene_settings);
    setDefaultViewportSettings(viewport_settings);
    setDefaultNavigationSettings(navigation_settings);

    initTime(&app->time, app->platform.getTicks, app->platform.ticks_per_second);
    initMouse(&app->controls.mouse);
    initApp(defaults);

    u64 memory_size = sizeof(Selection) + defaults->additional_memory_size;
    memory_size += scene_settings->primitives * sizeof(Primitive);
    memory_size += scene_settings->textures   * sizeof(Texture);
    memory_size += scene_settings->meshes     * sizeof(Mesh);
    memory_size += scene_settings->curves     * sizeof(Curve);
    memory_size += scene_settings->boxes      * sizeof(Box);
    memory_size += scene_settings->grids      * sizeof(Grid);
    memory_size += scene_settings->cameras    * sizeof(Camera);
    memory_size += scene_settings->materials  * sizeof(Material);
    memory_size += scene_settings->lights     * sizeof(Light);
    memory_size += viewport_settings->hud_line_count * sizeof(HUDLine);

    if (scene_settings->textures && scene_settings->texture_files) {
        Texture texture;
        for (u32 i = 0; i < scene_settings->textures; i++)
            memory_size += getTextureMemorySize(&texture, scene_settings->texture_files[i].char_ptr, &app->platform);
    }

    u32 max_triangle_count = CUBE__TRIANGLE_COUNT;
    u32 max_vertex_count = CUBE__VERTEX_COUNT;
    u32 max_normal_count = CUBE__NORMAL_COUNT;
    if (scene_settings->meshes && scene_settings->mesh_files) {
        Mesh mesh;
        for (u32 i = 0; i < scene_settings->meshes; i++) {
            memory_size += getMeshMemorySize(&mesh, scene_settings->mesh_files[i].char_ptr, &app->platform);
            if (mesh.triangle_count > max_triangle_count) max_triangle_count = mesh.triangle_count;
            if (mesh.vertex_count > max_vertex_count) max_vertex_count = mesh.vertex_count;
            if (mesh.normals_count > max_normal_count) max_normal_count = mesh.normals_count;
        }
    }

    memory_size += max_vertex_count * (sizeof(vec3) + sizeof(vec4) + 1);
    memory_size += max_normal_count * sizeof(vec3);
    memory_size += FRAME_BUFFER_MEMORY_SIZE;

    initAppMemory(memory_size);

    PixelQuad *pixels = (PixelQuad*)allocateAppMemory(FRAME_BUFFER_MEMORY_SIZE);
    initRasterizer(&app->rasterizer, max_vertex_count, max_normal_count, &app->memory);
    initScene(&app->scene, scene_settings, &app->memory, &app->platform);
    if (app->on.sceneReady) app->on.sceneReady(&app->scene);

    if (viewport_settings->hud_line_count)
        viewport_settings->hud_lines = (HUDLine*)allocateAppMemory(viewport_settings->hud_line_count * sizeof(HUDLine));

    initViewport(&app->viewport, viewport_settings, navigation_settings, app->scene.cameras, pixels);
    if (app->on.viewportReady) app->on.viewportReady(&app->viewport);
}

#ifdef __linux__
//linux code goes here
#elif _WIN32
#include "./platforms/win32.h"
#endif