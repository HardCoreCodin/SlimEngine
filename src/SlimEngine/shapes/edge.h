#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
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

    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);
    f32 t;

    if (fabsf(B.z) > B.w) {
        vec4 AB = subVec4(B, A);
        if (B.z > 0) {
            t = A.z - A.w;
            t /= AB.w - AB.z;
        } else {
            t = A.z + A.w;
            t /= -(AB.w + AB.z);
        }
        B = scaleAddVec4(AB, t, A);
    }
    if (fabsf(A.z) > A.w) {
        vec4 BA = subVec4(A, B);
        if (A.z > 0) {
            t = B.z - B.w;
            t /= BA.w - BA.z;
        } else {
            t = B.z + B.w;
            t /= -(BA.w + BA.z);
        }
        A = scaleAddVec4(BA, t, B);
    }

    if (fabsf(B.x) > B.w) {
        vec4 AB = subVec4(B, A);
        if (B.x > 0) {
            t = A.x - A.w;
            t /= AB.w - AB.x;
        } else {
            t = A.x + A.w;
            t /= -(AB.w + AB.x);
        }
        B = scaleAddVec4(AB, t, A);
    }
    if (fabsf(A.x) > A.w) {
        vec4 BA = subVec4(A, B);
        if (A.x > 0) {
            t = B.x - B.w;
            t /= BA.w - BA.x;
        } else {
            t = B.x + B.w;
            t /= -(BA.w + BA.x);
        }
        A = scaleAddVec4(BA, t, B);
    }

    if (fabsf(B.y) > B.w) {
        vec4 AB = subVec4(B, A);
        if (B.y > 0) {
            t = A.y - A.w;
            t /= AB.w - AB.y;
        } else {
            t = A.y + A.w;
            t /= -(AB.w + AB.y);
        }
        B = scaleAddVec4(AB, t, A);
    }
    if (fabsf(A.y) > A.w) {
        vec4 BA = subVec4(A, B);
        if (A.y > 0) {
            t = B.y - B.w;
            t /= BA.w - BA.y;
        } else {
            t = B.y + B.w;
            t /= -(BA.w + BA.y);
        }
        A = scaleAddVec4(BA, t, B);
    }

    bool A_is_out_x = fabsf(A.x) >= fabsf(A.w);
    bool A_is_out_y = fabsf(A.y) >= fabsf(A.w);
    bool A_is_out_z = fabsf(A.z) >= fabsf(A.w);
    bool B_is_out_x = fabsf(B.x) >= fabsf(B.w);
    bool B_is_out_y = fabsf(B.y) >= fabsf(B.w);
    bool B_is_out_z = fabsf(B.z) >= fabsf(B.w);
    if ((A_is_out_x && A_is_out_y && A_is_out_z) ||
        (B_is_out_x && B_is_out_y && B_is_out_z))
        return false;

    edge->from = Vec3fromVec4(mulVec4Mat4(A, viewport->pre_projection_matrix_inverted));
    edge->to   = Vec3fromVec4(mulVec4Mat4(B, viewport->pre_projection_matrix_inverted));

    return true;
}

bool cullAndClipEdge2(Edge *edge, Viewport *viewport) {
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

    vec4 from = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
    vec4 to   = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);

    if (fabsf(from.x) > from.w) from = scaleAddVec4(subVec4(from, to), 1.0f - ((fabsf(from.x) - from.w) / fabsf(to.x - from.x)), to);
    if (fabsf(from.y) > from.w) from = scaleAddVec4(subVec4(from, to), 1.0f - ((fabsf(from.y) - from.w) / fabsf(to.y - from.y)), to);
    if (fabsf(to.x) > to.w) to = scaleAddVec4(subVec4(to, from), 1.0f - ((fabsf(to.x) - to.w) / fabsf(to.x - from.x)), from);
    if (fabsf(to.y) > to.w) to = scaleAddVec4(subVec4(to, from), 1.0f - ((fabsf(to.y) - to.w) / fabsf(to.y - from.y)), from);

//    bool from_is_out_x = from.w > 0 ? (from.x > from.w) : (from.x < from.w);
//    bool from_is_out_y = from.w > 0 ? (from.y > from.w) : (from.y < from.w);
//    bool to_is_out_x   = to.w > 0 ? (to.x > to.w) : (to.x < to.w);
//    bool to_is_out_y   = to.w > 0 ? (to.y > to.w) : (to.y < to.w);
//    if ((from_is_out_x || from_is_out_y) &&
//        (to_is_out_x || to_is_out_y))
//        return false;

    mat4 inv = invMat4(viewport->pre_projection_matrix);
    edge->from = Vec3fromVec4(mulVec4Mat4(from, inv));
    edge->to = Vec3fromVec4(mulVec4Mat4(to, inv));

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