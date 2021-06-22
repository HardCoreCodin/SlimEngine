#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "./line.h"

void projectEdge(Edge *edge, Viewport *viewport) {
    Dimensions *dimensions = &viewport->frame_buffer->dimensions;
    f32 focal_length = viewport->camera->focal_length;
    f32 n = viewport->settings.near_clipping_plane_distance;

    bool v1_is_out = edge->from.z < n;
    bool v2_is_out = edge->to.z   < n;

    // Cull:
    if (v1_is_out &&
        v2_is_out) {
        edge->from = getVec3Of(-1);
        edge->to   = getVec3Of(-1);
        return;
    }

    // Clip:
    if (v1_is_out ||
        v2_is_out) {
        vec3 v;
        if (v1_is_out) {
            v = subVec3(edge->from, edge->to);
            v = scaleVec3(v, (edge->to.z - n) / (edge->to.z - edge->from.z));
            edge->from = addVec3(edge->to, v);
        } else {
            v = subVec3(edge->to, edge->from);
            v = scaleVec3(v, (edge->from.z - n) / (edge->from.z - edge->to.z));
            edge->to = addVec3(edge->from, v);
        }
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

void drawEdge(Viewport *viewport, RGBA color, Edge *edge) {
    projectEdge(edge, viewport);
    drawLine2D(viewport->frame_buffer, color,
               (i32)edge->from.x,
               (i32)edge->from.y,
               (i32)edge->to.x,
               (i32)edge->to.y);
}