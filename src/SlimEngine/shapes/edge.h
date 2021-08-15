#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "./line.h"

bool cullAndClipEdge(Edge *edge, Viewport *viewport) {
    f32 n = viewport->settings.near_clipping_plane_distance;
    f32 f = viewport->settings.far_clipping_plane_distance;

    bool from_is_out_n = edge->from.z < n;
    bool to_is_out_n   = edge->to.z < n;

    bool from_is_out_f = edge->from.z > f;
    bool to_is_out_f   = edge->to.z > f;

    // Cull:
    if ((from_is_out_n && to_is_out_n) ||
        (from_is_out_f && to_is_out_f))
        return false;

    f32 z_Range = fabsf(edge->to.z - edge->from.z);

    vec3 forward = subVec3(edge->to, edge->from);
    vec3 backward = invertedVec3(forward);

    // Clip:
    if (   from_is_out_n) edge->from = scaleAddVec3(backward, (edge->to.z - n)   / z_Range, edge->to);
    else if (to_is_out_n) edge->to   = scaleAddVec3(forward,  (edge->from.z - n) / z_Range, edge->from);

    if (   from_is_out_f) edge->from = scaleAddVec3(backward, (f - edge->to.z)   / z_Range, edge->to);
    else if (to_is_out_f) edge->to   = scaleAddVec3(forward,  (f - edge->from.z) / z_Range, edge->from);

    return true;
}

void projectEdge(Edge *edge, Viewport *viewport) {
    Dimensions *dimensions = &viewport->frame_buffer->dimensions;
    f32 focal_length = viewport->camera->focal_length;

    if (!cullAndClipEdge(edge, viewport)) {
        edge->to = edge->from = getVec3Of(-1);
        return;
    }

    // Project:
    f32 fl_over_z_from = focal_length / edge->from.z;
    f32 fl_over_z_to   = focal_length / edge->to.z;
    edge->from.x *= fl_over_z_from;
    edge->from.y *= fl_over_z_from * dimensions->width_over_height;
    edge->to.x   *= fl_over_z_to;
    edge->to.y   *= fl_over_z_to   * dimensions->width_over_height;

    // NDC->screen:
    edge->from.x += 1; edge->from.x *= dimensions->h_width;
    edge->to.x   += 1; edge->to.x   *= dimensions->h_width;
    edge->from.y += 1; edge->from.y *= dimensions->h_height;
    edge->to.y   += 1; edge->to.y   *= dimensions->h_height;

    // Flip Y:
    edge->from.y = dimensions->f_height - edge->from.y;
    edge->to.y   = dimensions->f_height - edge->to.y;
}

void drawEdge(Viewport *viewport, RGBA color, Edge *edge, u8 line_width) {
    projectEdge(edge, viewport);
    if (viewport->settings.depth_sort)
        drawLine3D(viewport->frame_buffer, color, edge->from, edge->to, line_width);
    else if (viewport->settings.antialias)
        drawLineAA2D(viewport->frame_buffer, color,
                     edge->from.x,
                     edge->from.y,
                     edge->to.x,
                     edge->to.y, line_width);
    else
        drawLine2D(viewport->frame_buffer, color,
                   (i32)edge->from.x,
                   (i32)edge->from.y,
                   (i32)edge->to.x,
                   (i32)edge->to.y);
}