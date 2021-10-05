#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
#include "./line.h"

INLINE f32 clipFactor(f32 Aw, f32 Bw, f32 a, f32 b, bool negative_w) {
    if (negative_w)
        b = -b;
    else
        a = -1;

    f32 numerator = Aw + a;
    return numerator / (numerator - Bw + b);
}

INLINE bool cullAndClipEdgeInClipSpace(vec4 *A, vec4 *B) {
    u8 A_flags = (A->x > A->w) | ((A->x < -A->w) * 2) | ((A->y > A->w) * 4) | ((A->y < -A->w) * 8) | ((A->z > A->w) * 16) | ((A->z < -A->w) * 32);
    u8 B_flags = (B->x > B->w) | ((B->x < -B->w) * 2) | ((B->y > B->w) * 4) | ((B->y < -B->w) * 8) | ((B->z > B->w) * 16) | ((B->z < -B->w) * 32);

    // Cull:
    if (A_flags & B_flags)
        return false;

    // Clip:
    if (A_flags | B_flags) {
        if (     A_flags & 3) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->x, A->x, A_flags & 2));
        else if (B_flags & 3) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->x, B->x, B_flags & 2));

        if (     A_flags & 12) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->y, A->y, A_flags & 8));
        else if (B_flags & 12) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->y, B->y, B_flags & 8));

        if (     A_flags & 48) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->z, A->z, A_flags & 32));
        else if (B_flags & 48) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->z, B->z, B_flags & 32));
    }

    return true;
}

INLINE bool cullAndClipEdge(Edge *edge, Viewport *viewport) {
    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);

    if (cullAndClipEdgeInClipSpace(&A, &B)) {
        edge->from = Vec3fromVec4(mulVec4Mat4(A, viewport->pre_projection_matrix_inverted));
        edge->to   = Vec3fromVec4(mulVec4Mat4(B, viewport->pre_projection_matrix_inverted));
        return true;
    } else
        return false;
}

INLINE bool projectEdge(Edge *edge, Viewport *viewport) {
    Dimensions *dimensions = &viewport->frame_buffer->dimensions;

    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);

    if (!cullAndClipEdgeInClipSpace(&A, &B)) {
        edge->to = edge->from = getVec3Of(-1);
        return false;
    }

    // Project:
    edge->from = scaleVec3(Vec3fromVec4(A), 1.0f / A.w);
    edge->to   = scaleVec3(Vec3fromVec4(B), 1.0f / B.w);

    // NDC->screen:
    edge->from.x += 1; edge->from.x *= dimensions->h_width;
    edge->to.x   += 1; edge->to.x   *= dimensions->h_width;
    edge->from.y += 1; edge->from.y *= dimensions->h_height;
    edge->to.y   += 1; edge->to.y   *= dimensions->h_height;

    // Flip Y:
    edge->from.y = dimensions->f_height - edge->from.y;
    edge->to.y   = dimensions->f_height - edge->to.y;

    return true;
}

void drawEdge(Viewport *viewport, RGBA color, Edge *edge) {
    if (projectEdge(edge, viewport))
        drawLine(viewport->frame_buffer, color,
                 (i32)edge->from.x,
                 (i32)edge->from.y,
                 (i32)edge->to.x,
                 (i32)edge->to.y);
}
void drawEdgeF(Viewport *viewport, vec3 color, f32 opacity, Edge *edge, u8 line_width) {
    if (projectEdge(edge, viewport))
        drawLineF(viewport->frame_buffer, color, opacity,
                  edge->from.x, edge->from.y,
                  edge->to.x,   edge->to.y,
                  line_width);
}
void drawEdge3D(Viewport *viewport, vec3 color, f32 opacity, Edge *edge, u8 line_width) {
    if (projectEdge(edge, viewport))
        drawLine3D(viewport->frame_buffer, color, opacity,
                   edge->from.x, edge->from.y, edge->from.z,
                   edge->to.x,   edge->to.y,   edge->to.z,
                   line_width);
}


//
//INLINE bool cullAndClipEdge(Edge *edge, Viewport *viewport) {
////    f32 n = viewport->settings.near_clipping_plane_distance;
////    f32 f = viewport->settings.far_clipping_plane_distance;
////
////    vec3 a = edge->from;
////    vec3 b = edge->to;
////
////    u8 A_flags = (a.z > f) | ((a.z < n) * 2);
////    u8 B_flags = (b.z > f) | ((b.z < n) * 2);
////
////    // Cull edge in Z:
////    if (A_flags &
////        B_flags) return false;
////
////    if (A_flags |
////        B_flags) {
////        if (     A_flags & 1) a = lerpVec3(b, a, (f - b.z) / (a.z - b.z));
////        else if (B_flags & 1) b = lerpVec3(a, b, (f - a.z) / (b.z - a.z));
////
////        if (     A_flags & 2) a = lerpVec3(b, a, (n - b.z) / (a.z - b.z));
////        else if (B_flags & 2) b = lerpVec3(a, b, (n - a.z) / (b.z - a.z));
////    }
//
//    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
//    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);
//
//    if (cullAndClipEdgeInClipSpace(&A, &B)) {
//        edge->from = Vec3fromVec4(mulVec4Mat4(A, viewport->pre_projection_matrix_inverted));
//        edge->to   = Vec3fromVec4(mulVec4Mat4(B, viewport->pre_projection_matrix_inverted));
//        return true;
//    } else
//        return false;
//}
//
//
//bool projectEdge(Edge *edge, Viewport *viewport) {
//    Dimensions *dimensions = &viewport->frame_buffer->dimensions;
//    f32 focal_length = viewport->camera->focal_length;
//
//    if (viewport->settings.show_hud) {
//        f32 n = viewport->settings.near_clipping_plane_distance;
//        f32 f = viewport->settings.far_clipping_plane_distance;
//
//        u8 A_flags = (edge->from.z > f) | ((edge->from.z < n) * 2);
//        u8 B_flags = (edge->to.z > f) | ((edge->to.z < n) * 2);
//
//        // Cull edge in Z:
//        if (A_flags &
//            B_flags) return false;
//
//        if (A_flags |
//            B_flags) {
//            if (     A_flags & 1) edge->from = lerpVec3(edge->to, edge->from, (f - edge->to.z) / (edge->from.z - edge->to.z));
//            else if (B_flags & 1) edge->to = lerpVec3(edge->from, edge->to, (f - edge->from.z) / (edge->to.z - edge->from.z));
//
//            if (     A_flags & 2) edge->from = lerpVec3(edge->to, edge->from, (n - edge->to.z) / (edge->from.z - edge->to.z));
//            else if (B_flags & 2) edge->to = lerpVec3(edge->from, edge->to, (n - edge->from.z) / (edge->to.z - edge->from.z));
//        }
//
//        vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->pre_projection_matrix);
//        vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->pre_projection_matrix);
//
//
//        if (!cullAndClipEdgeInClipSpace(&A, &B)) {
//            edge->to = edge->from = getVec3Of(-1);
//            return false;
//        }
//
//        // Project:
//        f32 one_over_w = 1.0f / A.w;
//        edge->from.x = A.x * one_over_w;
//        edge->from.y = A.y * one_over_w;
////    edge->from.z = A.w;
//
//        one_over_w = 1.0f / B.w;
//        edge->to.x = B.x * one_over_w;
//        edge->to.y = B.y * one_over_w;
////    edge->to.z = B.w;
//
//    } else {
//        if (!cullAndClipEdge(edge, viewport)) {
//            edge->to = edge->from = getVec3Of(-1);
//            return false;
//        }
//
//        // Project:
//        f32 fl_over_z_from = focal_length / edge->from.z;
//        f32 fl_over_z_to   = focal_length / edge->to.z;
//        edge->from.x *= fl_over_z_from * dimensions->height_over_width;
//        edge->from.y *= fl_over_z_from;
//        edge->to.x   *= fl_over_z_to * dimensions->height_over_width;
//        edge->to.y   *= fl_over_z_to;
//    }
//
//    // NDC->screen:
//    edge->from.x += 1; edge->from.x *= dimensions->h_width;
//    edge->to.x   += 1; edge->to.x   *= dimensions->h_width;
//    edge->from.y += 1; edge->from.y *= dimensions->h_height;
//    edge->to.y   += 1; edge->to.y   *= dimensions->h_height;
//
//    // Flip Y:
//    edge->from.y = dimensions->f_height - edge->from.y;
//    edge->to.y   = dimensions->f_height - edge->to.y;
//
//    return true;
//}