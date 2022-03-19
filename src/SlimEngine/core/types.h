#pragma once

#include "./base.h"

typedef struct KeyMap      { u8 ctrl, alt, shift, space, tab; } KeyMap;
typedef struct IsPressed { bool ctrl, alt, shift, space, tab; } IsPressed;
typedef struct Controls {
    IsPressed is_pressed;
    KeyMap key_map;
    Mouse mouse;
} Controls;

typedef u64 (*GetTicks)();

typedef struct PerTick {
    f64 seconds, milliseconds, microseconds, nanoseconds;
} PerTick;

typedef struct Ticks {
    PerTick per_tick;
    u64 per_second;
} Ticks;

typedef struct Timer {
    GetTicks getTicks;
    Ticks *ticks;
    f32 delta_time;
    u64 ticks_before,
        ticks_after,
        ticks_diff,
        accumulated_ticks,
        accumulated_frame_count,
        ticks_of_last_report,
        seconds,
        milliseconds,
        microseconds,
        nanoseconds;
    f64 average_frames_per_tick,
        average_ticks_per_frame;
    u16 average_frames_per_second,
        average_milliseconds_per_frame,
        average_microseconds_per_frame,
        average_nanoseconds_per_frame;
} Timer;

typedef struct Timers {
    Timer update, render, aux;
} Timers;

typedef struct Time {
    Timers timers;
    Ticks ticks;
    GetTicks getTicks;
} Time;

typedef struct Curve {
    f32 thickness;
    u32 revolution_count;
} Curve;

typedef enum BoxSide {
    NoSide = 0,
    Top    = 1,
    Bottom = 2,
    Left   = 4,
    Right  = 8,
    Front  = 16,
    Back   = 32
} BoxSide;

typedef struct BoxCorners {
    vec3 front_top_left,
         front_top_right,
         front_bottom_left,
         front_bottom_right,
         back_top_left,
         back_top_right,
         back_bottom_left,
         back_bottom_right;
} BoxCorners;

typedef union BoxVertices {
    BoxCorners corners;
    vec3 buffer[BOX__VERTEX_COUNT];
} BoxVertices;

typedef struct BoxEdgeSides {
    Edge front_top,
         front_bottom,
         front_left,
         front_right,
         back_top,
         back_bottom,
         back_left,
         back_right,
         left_bottom,
         left_top,
         right_bottom,
         right_top;
} BoxEdgeSides;

typedef union BoxEdges {
    BoxEdgeSides sides;
    Edge buffer[BOX__EDGE_COUNT];
} BoxEdges;

typedef struct Box {
    BoxVertices vertices;
    BoxEdges edges;
} Box;

typedef struct GridUVEdges {
    Edge u[GRID__MAX_SEGMENTS];
    Edge v[GRID__MAX_SEGMENTS];
} GridUVEdges;

typedef union GridEdges {
    GridUVEdges uv;
    Edge buffer[2][GRID__MAX_SEGMENTS];
} GridEdges;

typedef struct GridSideVertices {
    vec3 from[GRID__MAX_SEGMENTS];
    vec3 to[  GRID__MAX_SEGMENTS];
} GridSideVertices;

typedef struct GridUVVertices {
    GridSideVertices u, v;
} GridUVVertices;

typedef union GridVertices {
    GridUVVertices uv;
    vec3 buffer[2][2][GRID__MAX_SEGMENTS];
} GridVertices;

typedef struct Grid {
    GridEdges edges;
    GridVertices vertices;
    u8 u_segments,
       v_segments;
} Grid;

enum PrimitiveType {
    PrimitiveType_None = 0,
    PrimitiveType_Mesh,
    PrimitiveType_Grid,
    PrimitiveType_Box,
    PrimitiveType_Helix,
    PrimitiveType_Coil,
    PrimitiveType_Tetrahedron
};

typedef struct Primitive {
    quat rotation;
    vec3 position, scale;
    u32 id;
    enum PrimitiveType type;
    enum ColorID color;
    u8 flags, material_id;
} Primitive;

typedef struct xform3 {
    mat3 matrix,
         yaw_matrix,
         pitch_matrix,
         roll_matrix,
         rotation_matrix,
         rotation_matrix_inverted;
    quat rotation,
         rotation_inverted;
    vec3 position, scale,
         *up_direction,
         *right_direction,
         *forward_direction;
} xform3;

typedef struct Camera {
    xform3 transform;
    vec3 current_velocity;
    f32 focal_length, zoom, dolly, target_distance;
} Camera;

typedef struct NavigationSpeedSettings {
    f32 turn, zoom, dolly, pan, orbit, orient;
} NavigationSpeedSettings;

typedef struct NavigationSettings {
    NavigationSpeedSettings speeds;
    f32 max_velocity, acceleration;
} NavigationSettings;

typedef struct NavigationTurn {
    bool right, left;
} NavigationTurn;

typedef struct NavigationMove {
    bool right, left, up, down, forward, backward;
} NavigationMove;

typedef struct Navigation {
    NavigationSettings settings;
    NavigationMove move;
    NavigationTurn turn;
    bool zoomed, moved, turned;
} Navigation;

typedef struct ProjectionPlane {
    vec3 start, right, down;
} ProjectionPlane;

typedef struct ViewportSettings {
    Pixel background;
    f32 near_clipping_plane_distance,
        far_clipping_plane_distance;
    u32 hud_line_count;
    HUDLine *hud_lines;
    enum ColorID hud_default_color;
    bool show_hud, use_cube_NDC, flip_z, antialias;
} ViewportSettings;

typedef struct Viewport {
    ViewportSettings settings;
    Dimensions dimensions;
    Navigation navigation;
    ProjectionPlane projection_plane;
    HUD hud;
    Box default_box;
    vec2i position;
    mat4 projection_matrix;
    Camera *camera;
    PixelQuad *pixels;
} Viewport;

typedef struct Ray {
    vec3 origin, direction;
} Ray;

typedef struct RayHit {
    vec3 position, normal;
    f32 distance, distance_squared;
    u32 object_id, object_type;
    bool from_behind;
} RayHit;

// Mesh:
// =====
typedef struct EdgeVertexIndices { u32 from, to; } EdgeVertexIndices;
typedef union TriangleVertexIndices { u32 ids[3]; struct { u32 v1, v2, v3; }; } TriangleVertexIndices;
typedef struct Mesh {
    AABB aabb;
    u32 triangle_count, vertex_count, edge_count, normals_count, uvs_count;
    vec3 *vertex_positions, *vertex_normals;
    vec2 *vertex_uvs;
    TriangleVertexIndices *vertex_position_indices;
    TriangleVertexIndices *vertex_normal_indices;
    TriangleVertexIndices *vertex_uvs_indices;
    EdgeVertexIndices     *edge_vertex_indices;
} Mesh;

typedef struct Selection {
    quat object_rotation;
    vec3 transformation_plane_origin,
         transformation_plane_normal,
         transformation_plane_center,
         object_scale,
         world_offset,
         *world_position;
    Box box;
    Ray ray, local_ray;
    RayHit hit, local_hit;
    Primitive *primitive;
    enum BoxSide box_side;
    f32 object_distance;
    u32 object_type, object_id;
    bool changed;
} Selection;

typedef struct SceneSettings {
    u32 cameras, primitives, meshes, curves, boxes, grids;
    String file, *mesh_files;
} SceneSettings;

typedef struct Scene {
    SceneSettings settings;
    Selection *selection;
    Camera *cameras;
    Mesh *meshes;
    Primitive *primitives;
    Curve *curves;
    Grid *grids;
    Box *boxes;
    u64 last_io_ticks;
    bool last_io_is_save;
} Scene;

typedef struct AppCallbacks {
    void (*sceneReady)(Scene *scene);
    void (*viewportReady)(Viewport *viewport);
    void (*windowRedraw)();
    void (*windowResize)(u16 width, u16 height);
    void (*keyChanged)(  u8 key, bool pressed);
    void (*mouseButtonUp)(  MouseButton *mouse_button);
    void (*mouseButtonDown)(MouseButton *mouse_button);
    void (*mouseButtonDoubleClicked)(MouseButton *mouse_button);
    void (*mouseWheelScrolled)(f32 amount);
    void (*mousePositionSet)(i32 x, i32 y);
    void (*mouseMovementSet)(i32 x, i32 y);
    void (*mouseRawMovementSet)(i32 x, i32 y);
} AppCallbacks;

typedef void* (*CallbackForFileOpen)(const char* file_path);
typedef bool  (*CallbackForFileRW)(void *out, unsigned long, void *handle);
typedef void  (*CallbackForFileClose)(void *handle);

typedef struct Platform {
    GetTicks             getTicks;
    CallbackWithInt      getMemory;
    CallbackWithCharPtr  setWindowTitle;
    CallbackWithBool     setWindowCapture;
    CallbackWithBool     setCursorVisibility;
    CallbackForFileClose closeFile;
    CallbackForFileOpen  openFileForReading;
    CallbackForFileOpen  openFileForWriting;
    CallbackForFileRW    readFromFile;
    CallbackForFileRW    writeToFile;
    u64 ticks_per_second;
} Platform;

typedef struct Settings {
    SceneSettings scene;
    ViewportSettings viewport;
    NavigationSettings navigation;
} Settings;

typedef struct Defaults {
    char* title;
    u16 width, height;
    u64 additional_memory_size;
    Settings settings;
} Defaults;

typedef struct App {
    Memory memory;
    Platform platform;
    Controls controls;
    u32 *window_content;
    AppCallbacks on;
    Time time;
    Scene scene;
    Viewport viewport;
    bool is_running;
    void *user_data;
} App;

void setBoxEdgesFromVertices(BoxEdges *edges, BoxVertices *vertices) {
    edges->sides.front_top.from    = vertices->corners.front_top_left;
    edges->sides.front_top.to      = vertices->corners.front_top_right;
    edges->sides.front_bottom.from = vertices->corners.front_bottom_left;
    edges->sides.front_bottom.to   = vertices->corners.front_bottom_right;
    edges->sides.front_left.from   = vertices->corners.front_bottom_left;
    edges->sides.front_left.to     = vertices->corners.front_top_left;
    edges->sides.front_right.from  = vertices->corners.front_bottom_right;
    edges->sides.front_right.to    = vertices->corners.front_top_right;

    edges->sides.back_top.from     = vertices->corners.back_top_left;
    edges->sides.back_top.to       = vertices->corners.back_top_right;
    edges->sides.back_bottom.from  = vertices->corners.back_bottom_left;
    edges->sides.back_bottom.to    = vertices->corners.back_bottom_right;
    edges->sides.back_left.from    = vertices->corners.back_bottom_left;
    edges->sides.back_left.to      = vertices->corners.back_top_left;
    edges->sides.back_right.from   = vertices->corners.back_bottom_right;
    edges->sides.back_right.to     = vertices->corners.back_top_right;

    edges->sides.left_bottom.from  = vertices->corners.front_bottom_left;
    edges->sides.left_bottom.to    = vertices->corners.back_bottom_left;
    edges->sides.left_top.from     = vertices->corners.front_top_left;
    edges->sides.left_top.to       = vertices->corners.back_top_left;
    edges->sides.right_bottom.from = vertices->corners.front_bottom_right;
    edges->sides.right_bottom.to   = vertices->corners.back_bottom_right;
    edges->sides.right_top.from    = vertices->corners.front_top_right;
    edges->sides.right_top.to      = vertices->corners.back_top_right;
}

void setGridEdgesFromVertices(Edge *edges, u8 edge_count, vec3 *from, vec3 *to) {
    for (u8 i = 0; i < edge_count; i++) {
        edges[i].from = from[i];
        edges[i].to   = to[i];
    }
}

INLINE void setPixel(i32 x, i32 y, f64 depth, vec3 color, f32 opacity, Viewport *viewport) {
    Pixel *pixel;
    PixelQuad *pixel_quad;
    if (viewport->settings.antialias) {
        pixel_quad = viewport->pixels + (viewport->dimensions.stride * (y >> 1)) + (x >> 1);
        pixel = &pixel_quad->quad[y & 1][x & 1];
    } else {
        pixel_quad = viewport->pixels + (viewport->dimensions.stride * y) + x;
        pixel = &pixel_quad->TL;
    }

    Pixel new_pixel;
    new_pixel.opacity = opacity;
    new_pixel.color = color;
    new_pixel.depth = depth;

    if (!(opacity == 1 && depth == 0)) {
        Pixel background, foreground, old_pixel = *pixel;

        if (old_pixel.depth < new_pixel.depth) {
            background = new_pixel;
            foreground = old_pixel;
        } else {
            background = old_pixel;
            foreground = new_pixel;
        }
        if (foreground.opacity != 1) {
            f32 one_minus_foreground_opacity = 1.0f - foreground.opacity;
            opacity = foreground.opacity + background.opacity * one_minus_foreground_opacity;
            f32 one_over_opacity = opacity ? 1.0f / opacity : 1;
            f32 background_factor = background.opacity * one_over_opacity * one_minus_foreground_opacity;
            f32 foreground_factor = foreground.opacity * one_over_opacity;

            pixel->color.r = fast_mul_add(foreground.color.r, foreground_factor, background.color.r * background_factor);
            pixel->color.g = fast_mul_add(foreground.color.g, foreground_factor, background.color.g * background_factor);
            pixel->color.b = fast_mul_add(foreground.color.b, foreground_factor, background.color.b * background_factor);
            pixel->opacity = opacity;
            pixel->depth   = foreground.depth;
        } else *pixel = foreground;
    } else *pixel = new_pixel;

    if (!viewport->settings.antialias) pixel_quad->BR = pixel_quad->BL = pixel_quad->TR = pixel_quad->TL;
}