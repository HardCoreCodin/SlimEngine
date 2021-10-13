#pragma once

#include "../../SlimEngine/app.h"
#include "../../SlimEngine/core/time.h"
#include "../../SlimEngine/shapes/rect.h"
#include "../../SlimEngine/core/text.h"
#include "../../SlimEngine/scene/grid.h"
#include "../../SlimEngine/scene/xform.h"
#include "../../SlimEngine/viewport/navigation.h"
#include "../../SlimEngine/viewport/manipulation.h"

#define SECONDARY_VIEWPORT_WIDTH 600
#define SECONDARY_VIEWPORT_HEIGHT 400
#define SECONDARY_VIEWPORT_POSITION_X 20
#define SECONDARY_VIEWPORT_POSITION_Y 20

#define TRAJECTORY_LINE_WIDTH 4

#define LABEL_OPACITY 1
#define LABEL_LINE_WIDTH 0
#define FAT_LABEL_LINE_WIDTH 2

#define PROJECTION_LINE_WIDTH 0
#define PROJECTION_LINES_OPACITY 0.025f
#define PROJECTION_OPACITY 0.5f

#define LOCATOR_GRID_LINE_WIDTH 0
#define LOCATOR_GRID_OPACITY 0.0125f
#define LOCATOR_LINE_WIDTH 3

#define VIEW_FRUSTUM_LINE_WIDTH 1
#define VIEW_FRUSTUM_OPACITY 1

#define NDC_BOX_LINE_WIDTH 1
#define NDC_BOX_OPACITY 1

#define CAMERA_LINE_WIDTH 1

#define ARROW_LINE_WIDTH 1
#define ARROW_OPACITY 1

#define COORDINATE_ARROW_OPACITY 1
#define COORDINATE_ARROW_LINE_WIDTH 2

#define BG_SHAPE_LINE_WIDTH 0
#define FG_SHAPE_LINE_WIDTH 1
#define BG_SHAPE_OPACITY 0.75f
#define FG_SHAPE_OPACITY 0.65f

#define BG_GRID_LINE_WIDTH 0
#define FG_GRID_LINE_WIDTH 1
#define BG_GRID_OPACITY 0.5f
#define FG_GRID_OPACITY 0.6f

vec3 sides_color,
     near_color,
     far_color,
     focal_length_color,
     default_near_color,
     default_far_color,
     aspect_ratio_color,
     default_aspect_ratio_color,
     default_focal_length_color,
     NDC_color,
     X_color,
     Y_color,
     Z_color,
     W_color;

Viewport secondary_viewport, *active_viewport;

Primitive *secondary_camera_prim,
    *arrowX_box_prim,
    *arrowY_box_prim,
    *arrowZ_box_prim,
    *main_box_prim,
    *main_grid_prim,
    *main_camera_prim,
    *projection_plane_prim,
    *projective_ref_plane_prim;

Grid *main_grid,
     *projection_plane_grid,
     *projective_ref_plane_grid,
     clipped_grid,
     transformed_grid;

Box NDC_box,
    projected_box,
    pre_projected_view_frustum_box,
    transforming_view_frustum_box,
    view_frustum_box,
    clipped_box,
    *main_box;

Edge view_space_edge;
vec3 edge_color;
f32 camera_opacity = 0.5f;
vec3 camera_color;

bool draw_in_2D = false;
bool orbit = false;
bool collapse_final_matrix = false;
bool show_final_matrix = false;
bool show_pre_projection = false;
bool draw_locator_grids = false;
bool move_projective_point = true;

typedef enum VIZ {
    INTRO,
    PROJECTIVE_SPACE,
    PROJECTION,
    VIEW_FRUSTUM,
    VIEW_FRUSTUM_SLICE
} VIZ;
VIZ current_viz = INTRO;

typedef union Quad3 {
    struct {
        vec3 top_left, top_right, bottom_right, bottom_left;
    };
    vec3 corners[4];
} Quad3;

void setQuad3FromBox(Quad3 *quad, Box *box) {
    quad->top_left     = box->vertices.corners.back_bottom_left;
    quad->top_right    = box->vertices.corners.back_bottom_right;
    quad->bottom_left  = box->vertices.corners.front_bottom_left;
    quad->bottom_right = box->vertices.corners.front_bottom_right;
    for (u8 i = 0; i < 4; i++) quad->corners[i].y = 0;
}

typedef struct Label {
    vec2i position;
    vec3 color;
    char *text;
} Label;

#define MAX_LABEL_COUNT 32
typedef struct Labels {
    u8 count;
    Label array[MAX_LABEL_COUNT];
} Labels;
Labels labels;

void addLabel(vec3 color, char* text, i32 x, i32 y) {
    Label label;
    label.text = text;
    label.color = color;
    label.position = Vec2i(x, y);
    labels.array[labels.count++] = label;
}

void transformEdge(Edge *in_edge, Edge *out_edge, xform3 *xform) {
    out_edge->from = subVec3(in_edge->from, xform->position);
    out_edge->from = mulVec3Quat(out_edge->from, xform->rotation_inverted);
    out_edge->to = subVec3(in_edge->to, xform->position);
    out_edge->to = mulVec3Quat(out_edge->to, xform->rotation_inverted);
}

INLINE vec3 vec3wUp(vec4 v) {
    return Vec3(v.x, v.w, v.z);
}

BoxCorners getViewFrustumCorners(Viewport *viewport) {
    BoxCorners corners;
    corners.back_top_right.z = viewport->settings.near_clipping_plane_distance;
    corners.back_top_right.y = viewport->settings.near_clipping_plane_distance / viewport->camera->focal_length;
    corners.back_top_right.x = viewport->dimensions.width_over_height * corners.back_top_right.y;
    corners.back_top_left = corners.back_top_right;
    corners.back_top_left.x *= -1;
    corners.back_bottom_left = corners.back_top_left;
    corners.back_bottom_left.y *= -1;
    corners.back_bottom_right = corners.back_top_right;
    corners.back_bottom_right.y *= -1;

    corners.front_top_right.z = viewport->settings.far_clipping_plane_distance;
    corners.front_top_right.y = viewport->settings.far_clipping_plane_distance / viewport->camera->focal_length;
    corners.front_top_right.x = viewport->dimensions.width_over_height * corners.front_top_right.y;
    corners.front_top_left = corners.front_top_right;
    corners.front_top_left.x *= -1;
    corners.front_bottom_left = corners.front_top_left;
    corners.front_bottom_left.y *= -1;
    corners.front_bottom_right = corners.front_top_right;
    corners.front_bottom_right.y *= -1;

    return corners;
}

void drawFrustum(Viewport *viewport, Box *view_frustum, vec3 near_col, vec3 far_col, vec3 side_col, f32 opacity, u8 line_width) {
    transformBoxVerticesFromObjectToViewSpace(&view_frustum->vertices, &view_frustum->vertices, secondary_camera_prim, viewport);
    setBoxEdgesFromVertices(&view_frustum->edges, &view_frustum->vertices);

    drawEdge(&view_frustum->edges.sides.front_top,    far_col,  1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.front_bottom, far_col,  1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.front_left,   far_col,  1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.front_right,  far_col,  1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.back_top,     near_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.back_bottom,  near_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.back_left,    near_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.back_right,   near_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.left_top,     side_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.left_bottom,  side_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.right_top,    side_col, 1, line_width, viewport);
    drawEdge(&view_frustum->edges.sides.right_bottom, side_col, 1, line_width, viewport);
}

void convertEdgeFromSecondaryToMain(Edge *edge) {
    edge->from = convertPositionToWorldSpace(edge->from, secondary_camera_prim);
    edge->to   = convertPositionToWorldSpace(edge->to,   secondary_camera_prim);
    edge->from = convertPositionToObjectSpace(edge->from, main_camera_prim);
    edge->to   = convertPositionToObjectSpace(edge->to,   main_camera_prim);
}

void drawLocalEdge(Edge edge, vec3 color, f32 opacity, u8 line_width) {
    convertEdgeFromSecondaryToMain(&edge);
    if (draw_in_2D) edge.from.z = edge.to.z = 0;
    drawEdge(&edge, color, opacity, line_width, &app->viewport);
}

void drawClippedEdge(Viewport *viewport, Edge *clipped_edge, vec3 color, f32 opacity, u8 line_width) {
    convertEdgeFromSecondaryToMain(clipped_edge);
    projectEdge(clipped_edge, viewport);
    drawLine(clipped_edge->from.x,
             clipped_edge->from.y,
             clipped_edge->from.z - 0.1f,
             clipped_edge->to.x,
             clipped_edge->to.y,
             clipped_edge->to.z - 0.1f,
             color, opacity, line_width, viewport);
}

void updateProjectionBoxes(Viewport *viewport) {
    setProjectionMatrix(viewport);
    view_frustum_box.vertices.corners = getViewFrustumCorners(viewport);
    setBoxEdgesFromVertices(&view_frustum_box.edges, &view_frustum_box.vertices);

    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        mulVec3Mat4(view_frustum_box.vertices.buffer[i], 1.0f, viewport->projection_matrix, pre_projected_view_frustum_box.vertices.buffer + i);

    setBoxEdgesFromVertices(&pre_projected_view_frustum_box.edges, &pre_projected_view_frustum_box.vertices);
}