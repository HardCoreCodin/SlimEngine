#pragma once

#include "../SlimEngine/app.h"
#include "../SlimEngine/core/time.h"
#include "../SlimEngine/shapes/rect.h"
#include "../SlimEngine/core/text.h"
#include "../SlimEngine/scene/grid.h"
#include "../SlimEngine/scene/xform.h"
#include "../SlimEngine/viewport/navigation.h"
#include "../SlimEngine/viewport/manipulation.h"

RGBA sides_color,
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

PixelGrid secondary_viewport_frame_buffer, text_overlay_frame_buffer;
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
//    scaled_view_frustum_box,
    clipped_box,
    *main_box;

Edge view_space_edge;
RGBA edge_color;
RGBA camera_color;

bool orbit = false;
bool show_pre_projection = false;
bool draw_locator_grids = false;
bool move_projective_point = true;

typedef enum VIZ {
    INTRO,
    VIEW_FRUSTUM,
    PROJECTION,
    PROJECTIVE_SPACE,
    VIEW_FRUSTUM_SLICE,
    VIS_COUNT
} VIZ;
VIZ current_viz = INTRO;

typedef struct Label {
    vec2i position;
    RGBA color;
    char *text;
} Label;

#define MAX_LABEL_COUNT 16
typedef struct Labels {
    u8 count;
    Label array[MAX_LABEL_COUNT];
} Labels;
Labels labels;

void addLabel(RGBA color, char* text, i32 x, i32 y) {
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

void drawSecondaryViewportToFrameBuffer(PixelGrid *frame_buffer) {
    copyPixels(&secondary_viewport_frame_buffer, frame_buffer,
               secondary_viewport_frame_buffer.dimensions.width,
               secondary_viewport_frame_buffer.dimensions.height,
               secondary_viewport.settings.position.x,
               secondary_viewport.settings.position.y);
    i32 x1 = secondary_viewport.settings.position.x;
    i32 x2 = secondary_viewport.frame_buffer->dimensions.width + x1;
    i32 y1 = secondary_viewport.settings.position.y;
    i32 y2 = secondary_viewport.frame_buffer->dimensions.height + y1;

    edge_color = Color(White);

    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1-1);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1+0);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y1+1);

    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2-1);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2+0);
    drawHLine2D(frame_buffer, edge_color, x1-1, x2+1, y2+1);


    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1-1);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1+0);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x1+1);

    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2-1);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2+0);
    drawVLine2D(frame_buffer, edge_color, y1-1, y2+1, x2+1);
}

INLINE vec3 vec3wUp(vec4 v) {
    return Vec3(v.x, v.w, v.z);
}

BoxCorners getViewFrustumCorners(Viewport *viewport) {
    BoxCorners corners;
    corners.back_top_right.z = viewport->settings.near_clipping_plane_distance;
    corners.back_top_right.y = viewport->settings.near_clipping_plane_distance / viewport->camera->focal_length;
    corners.back_top_right.x = viewport->frame_buffer->dimensions.width_over_height * corners.back_top_right.y;
    corners.back_top_left = corners.back_top_right;
    corners.back_top_left.x *= -1;
    corners.back_bottom_left = corners.back_top_left;
    corners.back_bottom_left.y *= -1;
    corners.back_bottom_right = corners.back_top_right;
    corners.back_bottom_right.y *= -1;

    corners.front_top_right.z = viewport->settings.far_clipping_plane_distance;
    corners.front_top_right.y = viewport->settings.far_clipping_plane_distance / viewport->camera->focal_length;
    corners.front_top_right.x = viewport->frame_buffer->dimensions.width_over_height * corners.front_top_right.y;
    corners.front_top_left = corners.front_top_right;
    corners.front_top_left.x *= -1;
    corners.front_bottom_left = corners.front_top_left;
    corners.front_bottom_left.y *= -1;
    corners.front_bottom_right = corners.front_top_right;
    corners.front_bottom_right.y *= -1;

    return corners;
}

void drawFrustum(Viewport *viewport, Box *view_frustum, RGBA near_col, RGBA far_col, RGBA side_col, u8 line_width) {
    transformBoxVerticesFromObjectToViewSpace(viewport, secondary_camera_prim, &view_frustum->vertices, &view_frustum->vertices);
    setBoxEdgesFromVertices(&view_frustum->edges, &view_frustum->vertices);

    drawEdge(viewport, far_col,  &view_frustum->edges.sides.front_top,    line_width);
    drawEdge(viewport, far_col,  &view_frustum->edges.sides.front_bottom, line_width);
    drawEdge(viewport, far_col,  &view_frustum->edges.sides.front_left,   line_width);
    drawEdge(viewport, far_col,  &view_frustum->edges.sides.front_right,  line_width);
    drawEdge(viewport, near_col, &view_frustum->edges.sides.back_top,     line_width);
    drawEdge(viewport, near_col, &view_frustum->edges.sides.back_bottom,  line_width);
    drawEdge(viewport, near_col, &view_frustum->edges.sides.back_left,    line_width);
    drawEdge(viewport, near_col, &view_frustum->edges.sides.back_right,   line_width);
    drawEdge(viewport, side_col, &view_frustum->edges.sides.left_top,     line_width);
    drawEdge(viewport, side_col, &view_frustum->edges.sides.left_bottom,  line_width);
    drawEdge(viewport, side_col, &view_frustum->edges.sides.right_top,    line_width);
    drawEdge(viewport, side_col, &view_frustum->edges.sides.right_bottom, line_width);
}

void convertEdgeFromSecondaryToMain(Edge *edge) {
    edge->from = convertPositionToWorldSpace(edge->from, secondary_camera_prim);
    edge->to   = convertPositionToWorldSpace(edge->to,   secondary_camera_prim);
    edge->from = convertPositionToObjectSpace(edge->from, main_camera_prim);
    edge->to   = convertPositionToObjectSpace(edge->to,   main_camera_prim);
}

void drawLocalEdge(Edge edge, RGBA color, u8 line_width) {
    convertEdgeFromSecondaryToMain(&edge);
    drawEdge(&app->viewport, color, &edge, line_width);
}

void drawClippedEdge(Viewport *viewport, Edge *clipped_edge, RGBA color) {
    convertEdgeFromSecondaryToMain(clipped_edge);
    projectEdge(clipped_edge, viewport);
    clipped_edge->from.z -= 0.1f;
    clipped_edge->to.z -= 0.1f;
    drawLine3D(viewport->frame_buffer, color, clipped_edge->from, clipped_edge->to, 1);
}

void updateProjectionBoxes(Viewport *viewport) {
    setPreProjectionMatrix(viewport);
    view_frustum_box.vertices.corners = getViewFrustumCorners(viewport);
    setBoxEdgesFromVertices(&view_frustum_box.edges, &view_frustum_box.vertices);

    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        mulVec3Mat4(view_frustum_box.vertices.buffer[i], 1.0f, viewport->pre_projection_matrix, pre_projected_view_frustum_box.vertices.buffer + i);

    setBoxEdgesFromVertices(&pre_projected_view_frustum_box.edges, &pre_projected_view_frustum_box.vertices);
}