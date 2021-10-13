#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
#include "./line.h"

bool cullAndClipEdge(Edge *edge, Viewport *viewport) {
    f32 distance = viewport->settings.near_clipping_plane_distance;
    vec3 A = edge->from;
    vec3 B = edge->to;

    u8 out = (A.z < distance) | ((B.z < distance) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, (distance - A.z) / (B.z - A.z));
        else         B = lerpVec3(B, A, (distance - B.z) / (A.z - B.z));
    }

    distance = viewport->settings.far_clipping_plane_distance;
    out = (A.z > distance) | ((B.z > distance) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, (A.z - distance) / (A.z - B.z));
        else         B = lerpVec3(B, A, (B.z - distance) / (B.z - A.z));
    }

    // Left plane (facing to the right):
    vec3 N = Vec3(viewport->camera->focal_length, 0, viewport->dimensions.width_over_height);
    f32 NdotA = dotVec3(N, A);
    f32 NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Right plane (facing to the left):
    N.x = -N.x;
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Bottom plane (facing up):
    N = Vec3(0, viewport->camera->focal_length, 1);
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    // Top plane (facing down):
    N.y = -N.y;
    NdotA = dotVec3(N, A);
    NdotB = dotVec3(N, B);

    out = (NdotA < 0) | ((NdotB < 0) << 1);
    if (out) {
        if (out == 3) return false;
        if (out & 1) A = lerpVec3(A, B, NdotA / (NdotA - NdotB));
        else         B = lerpVec3(B, A, NdotB / (NdotB - NdotA));
    }

    edge->from = A;
    edge->to   = B;

    return true;
}

INLINE bool projectEdge(Edge *edge, Viewport *viewport) {
    if (!cullAndClipEdge(edge, viewport))
        return false;

    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->projection_matrix);
    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->projection_matrix);

    // Project:
    edge->from = scaleVec3(Vec3fromVec4(A), 1.0f / A.w);
    edge->to   = scaleVec3(Vec3fromVec4(B), 1.0f / B.w);

    // NDC->screen:
    edge->from.x += 1; edge->from.x *= viewport->dimensions.h_width;
    edge->to.x   += 1; edge->to.x   *= viewport->dimensions.h_width;
    edge->from.y += 1; edge->from.y *= viewport->dimensions.h_height;
    edge->to.y   += 1; edge->to.y   *= viewport->dimensions.h_height;

    // Flip Y:
    edge->from.y = viewport->dimensions.f_height - edge->from.y;
    edge->to.y   = viewport->dimensions.f_height - edge->to.y;

    return true;
}

INLINE void drawEdge(Edge *edge, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    if (projectEdge(edge, viewport))
        drawLine(edge->from.x, edge->from.y, edge->from.z,
                 edge->to.x,   edge->to.y,   edge->to.z,
                 color, opacity, line_width, viewport);
}

//
//INLINE f32 clipFactor(f32 Aw, f32 Bw, f32 a, f32 b, bool negative_w) {
//    if (negative_w)
//        b = -b;
//    else
//        a = -a;
//
//    f32 numerator = Aw + a;
//    return numerator / (numerator - Bw + b);
//}
//
//INLINE bool cullAndClipEdgeInClipSpace(vec4 *A, vec4 *B) {
//    u8 A_flags = (A->x > A->w) | ((A->x < -A->w) * 2) | ((A->y > A->w) * 4) | ((A->y < -A->w) * 8) | ((A->z > A->w) * 16) | ((A->z < -A->w) * 32);
//    u8 B_flags = (B->x > B->w) | ((B->x < -B->w) * 2) | ((B->y > B->w) * 4) | ((B->y < -B->w) * 8) | ((B->z > B->w) * 16) | ((B->z < -B->w) * 32);
//
//    // Cull:
//    if (A_flags & B_flags)
//        return false;
//
//    // Clip:
//    if (A_flags | B_flags) {
//        if (     A_flags & 3) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->x, A->x, A_flags & 2));
//        else if (B_flags & 3) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->x, B->x, B_flags & 2));
//
//        if (     A_flags & 12) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->y, A->y, A_flags & 8));
//        else if (B_flags & 12) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->y, B->y, B_flags & 8));
//
//        if (     A_flags & 48) *A = lerpVec4(*B, *A, clipFactor(B->w, A->w, B->z, A->z, A_flags & 32));
//        else if (B_flags & 48) *B = lerpVec4(*A, *B, clipFactor(A->w, B->w, A->z, B->z, B_flags & 32));
//    }
//
//    return true;
//}
//
//INLINE bool cullAndClipEdge2(Edge *edge, Viewport *viewport) {
//    vec4 A = mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), viewport->projection_matrix);
//    vec4 B = mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), viewport->projection_matrix);
//
//    if (cullAndClipEdgeInClipSpace(&A, &B)) {
//        edge->from = Vec3fromVec4(mulVec4Mat4(A, viewport->pre_projection_matrix_inverted));
//        edge->to   = Vec3fromVec4(mulVec4Mat4(B, viewport->pre_projection_matrix_inverted));
//        return true;
//    } else
//        return false;
//}