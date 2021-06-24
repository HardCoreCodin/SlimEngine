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
    f32 focal_length;
    xform3 transform;
} Camera;

typedef struct NavigationSpeedSettings {
    f32 turn, zoom, dolly, pan, orbit, orient;
} NavigationSpeedSettings;

typedef struct NavigationSettings {
    NavigationSpeedSettings speeds;
    f32 max_velocity, acceleration, target_distance;
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
    vec3 current_velocity;
    f32 zoom, dolly, target_distance;
    bool zoomed, moved, turned;
} Navigation;

typedef struct ProjectionPlane {
    vec3 start, right, down;
} ProjectionPlane;

typedef struct ViewportSettings {
    f32 near_clipping_plane_distance,
        far_clipping_plane_distance;
    u32 hud_line_count;
    bool show_hud;
} ViewportSettings;

typedef struct Viewport {
    ViewportSettings settings;
    Navigation navigation;
    ProjectionPlane projection_plane;
    HUD hud;
    Camera *camera;
    PixelGrid *frame_buffer;
} Viewport;

typedef struct Ray {
    vec3 origin, scaled_origin, direction, direction_reciprocal;
    u8_3 octant;
} Ray;

typedef struct RayHit {
    vec3 position, normal;
    vec2 uv;
    f32 distance, distance_squared;
    u32 material_id, object_id, object_type;
    bool from_behind;
} RayHit;

typedef struct Triangle {
    mat3 world_to_tangent;
    vec3 position, normal;
} Triangle;

typedef struct TriangleVertexIndices {
    u32 ids[3];
} TriangleVertexIndices;

typedef struct Mesh {
    AABB aabb;
    u32 triangle_count, vertex_count;
    vec3 *vertex_positions;
    TriangleVertexIndices *triangle_vertex_indices;
    Triangle *triangles;
    Edge *edges;
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

typedef struct SceneCounts {
    u32 cameras, primitives, meshes, curves, boxes, grids;
} SceneCounts;

typedef struct Scene {
    SceneCounts counts;
    Selection selection;
    Camera *cameras;
    Mesh *meshes;
    Primitive *primitives;
    Curve *curves;
    Grid *grids;
    Box *boxes;
} Scene;

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