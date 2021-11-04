//#include "../SlimEngine/app.h"
//#include "../SlimEngine/core/time.h"
//#include "../SlimEngine/viewport/viewport.h"
//#include "../SlimEngine/viewport/navigation.h"
//#include "../SlimEngine/viewport/manipulation.h"
//#include "../SlimEngine/renderer/rasterizer.h"
// Or using the single-header file:
 #include "../SlimEngine.h"


void onMouseButtonDown(MouseButton *mouse_button) {
    app->controls.mouse.pos_raw_diff = Vec2i(0, 0);
}
void onMouseDoubleClicked(MouseButton *mouse_button) {
    if (mouse_button == &app->controls.mouse.left_button) {
        app->controls.mouse.is_captured = !app->controls.mouse.is_captured;
        app->platform.setCursorVisibility(!app->controls.mouse.is_captured);
        app->platform.setWindowCapture(    app->controls.mouse.is_captured);
        onMouseButtonDown(mouse_button);
    }
}
void onKeyChanged(u8 key, bool is_pressed) {
    if (!is_pressed) {
        Viewport *viewport = &app->viewport;
        ViewportSettings *settings = &viewport->settings;
        u8 tab = app->controls.key_map.tab;
        if (key == tab) settings->show_hud = !settings->show_hud;
        if (key == '1') {
            settings->show_wire_frame = !settings->show_wire_frame;
            setString(&viewport->hud.lines[1].value.string,settings->show_wire_frame ? (char*)"On" : (char*)"Off");
            viewport->hud.lines[1].value_color = settings->show_wire_frame ? White : Grey;
        }
        if (key == '2') {
            settings->antialias = !settings->antialias;
            setString(&viewport->hud.lines[2].value.string,settings->antialias ? (char*)"On" : (char*)"Off");
            viewport->hud.lines[2].value_color = settings->antialias ? White : Grey;
        }
    }

    NavigationMove *move = &app->viewport.navigation.move;
    if (key == 'R') move->up       = is_pressed;
    if (key == 'F') move->down     = is_pressed;
    if (key == 'W') move->forward  = is_pressed;
    if (key == 'A') move->left     = is_pressed;
    if (key == 'S') move->backward = is_pressed;
    if (key == 'D') move->right    = is_pressed;
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
    f32 dt = app->time.timers.update.delta_time;
    static float elapsed = 0;
    elapsed += dt;

    vec2 sincos = Vec2(sinf(elapsed), cosf(elapsed));

    scene->lights[0].position_or_direction.x = scene->primitives[0].position.x - 3.0f + sincos.x * 0.6f;
    scene->lights[0].position_or_direction.z = scene->primitives[0].position.z + 3.0f + sincos.y * 0.6f;
    scene->lights[0].position_or_direction.y = 2 + sinf(elapsed * 2.0f);

    scene->lights[1].position_or_direction.x = scene->primitives[1].position.x + 3.0f + sinf(elapsed * 0.5f) * 0.6f;
    scene->lights[1].position_or_direction.z = scene->primitives[1].position.z + 3.0f + cosf(elapsed * 0.5f) * 0.6f;
    scene->lights[1].position_or_direction.y = 2 + cosf(elapsed * 2.0f);

    beginFrame(timer);
        if (!mouse->is_captured) manipulateSelection(scene, viewport, controls);
        if (!controls->is_pressed.alt) updateViewport(viewport, mouse);
        beginDrawing(viewport);
            rasterize(scene, viewport, &app->rasterizer);
            drawSelection(scene, viewport, controls);
            printNumberIntoString((i16)app->time.timers.update.average_frames_per_second, &viewport->hud.lines->value);
        endDrawing(viewport);
    endFrame(timer, mouse);
}
void setupViewport(Viewport *viewport) {
    viewport->camera->transform.position = Vec3(0, 15, -15);
    rotateXform3(&viewport->camera->transform, 0, -0.25f, 0);
    printNumberIntoString((i16)app->time.timers.update.average_frames_per_second, &viewport->hud.lines->value);
    setString(&viewport->hud.lines[0].title,(char*)"Fps: ");
    setString(&viewport->hud.lines[1].title,(char*)"Wireframe: ");
    setString(&viewport->hud.lines[1].value.string,(char*)"Off");
    setString(&viewport->hud.lines[2].title,(char*)"MSAA: ");
    setString(&viewport->hud.lines[2].value.string,(char*)"Off");
    setString(&viewport->hud.lines[3].title,(char*)"Triangles: ");
    printNumberIntoString((i32)(app->scene.meshes[0].triangle_count*2 + app->scene.meshes[1].triangle_count + 12), &viewport->hud.lines[3].value);
    viewport->hud.lines[1].value_color = Grey;
    viewport->hud.lines[2].value_color = Grey;
}
void setupScene(Scene *scene) {
    Light *light1 = scene->lights + 0;
    Light *light2 = scene->lights + 1;
    Light *light3 = scene->lights + 2;
    Material *mat1 = scene->materials + 0;
    Material *mat2 = scene->materials + 1;
    Primitive *cube = scene->primitives + 0;
    Primitive *dog  = scene->primitives + 1;

    mat1->flags = BLINN | LAMBERT;
    mat2->flags = LAMBERT;
    mat1->diffuse = Vec3(0.4f, 0.4f, 0.4f);
    mat2->diffuse = Vec3(0.7f, 0.7f, 0.7f);

    mat1->pixel_shader = mat2->pixel_shader = shadePixelClassic;

    cube->type = PrimitiveType_Box;
    cube->scale    = Vec3(16, 1, 16);
    cube->position = Vec3(-6, -3,0);
    cube->material_id = 0;

    dog->type = PrimitiveType_Mesh;
    dog->id = 0;
    dog->material_id = 1;
    dog->position = Vec3(2, -2, 9);
    dog->rotation = getRotationAroundAxisBySinCon(Vec3(0, 1, 0), Vec2(sinf(0.5f), cosf(0.5f)));
    dog->scale = Vec3(0.1f, 0.1f, 0.1f);

    scene->ambient_light.color = Vec3(0.008f, 0.008f, 0.014f);

    vec3 mesh1_position = Vec3(0, 0, 5);
    vec3 mesh2_position = Vec3(5, 0, 5);

    light1->intensity = 20;
    light1->color.r = 0.8f;
    light1->color.g = 0.3f;
    light1->color.b = 0.2f;
    light1->position_or_direction.x = mesh1_position.x - 3;
    light1->position_or_direction.z = mesh1_position.z + 3;
    light1->position_or_direction.y = 5;

    light2->intensity = 20;
    light2->color.r = 0.2f;
    light2->color.g = 0.3f;
    light2->color.b = 0.8f;
    light2->position_or_direction.x = mesh2_position.x + 3;
    light2->position_or_direction.z = mesh2_position.z + 3;
    light2->position_or_direction.y = 4;

    light3->intensity = 16;
    light3->color.r = 0.2f;
    light3->color.g = 0.9f;
    light3->color.b = 0.3f;
    light3->position_or_direction.x = (mesh1_position.x + mesh2_position.x) / 2;
    light3->position_or_direction.z = -1;
    light3->position_or_direction.y = 3;

    mat1->texture_count = 2;
    mat1->texture_ids[0] = 0;
    mat1->texture_ids[1] = 1;

    mat2->texture_count = 2;
    mat2->texture_ids[0] = 2;
    mat2->texture_ids[1] = 3;
}

void initApp(Defaults *defaults) {
    static char string_buffers[5][100];
    static String files[5];
    files[0].char_ptr = string_buffers[0];
    files[1].char_ptr = string_buffers[1];
    files[2].char_ptr = string_buffers[2];
    files[3].char_ptr = string_buffers[3];
    files[4].char_ptr = string_buffers[4];

    char* this_file   = __FILE__;
    char* dog_mesh = "dog.mesh";
    char* dog_texture = "dog_diffuse.texture";
    char* dog_normal = "dog_normal.texture";
    char* rock_texture = "rock_01_diffuse.texture";
    char* rock_normal = "rock_01_normal.texture";
    u32 dir_len = getDirectoryLength(this_file);
    mergeString(files, this_file, dog_mesh, dir_len);
    mergeString(files + 1, this_file, rock_texture, dir_len);
    mergeString(files + 2, this_file, rock_normal, dir_len);
    mergeString(files + 3, this_file, dog_texture, dir_len);
    mergeString(files + 4, this_file, dog_normal, dir_len);
    defaults->settings.scene.textures = 4;
    defaults->settings.scene.meshes = 1;
    defaults->settings.scene.mesh_files = files;
    defaults->settings.scene.texture_files = files + 1;
    defaults->settings.scene.lights     = 3;
    defaults->settings.scene.primitives = 2;
    defaults->settings.scene.materials  = 2;
    defaults->settings.viewport.near_clipping_plane_distance = 1;
    defaults->settings.viewport.hud_line_count = 4;
    app->on.sceneReady    = setupScene;
    app->on.viewportReady = setupViewport;
    app->on.windowRedraw  = updateAndRender;
    app->on.keyChanged               = onKeyChanged;
    app->on.mouseButtonDown          = onMouseButtonDown;
    app->on.mouseButtonDoubleClicked = onMouseDoubleClicked;
}