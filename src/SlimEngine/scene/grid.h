#pragma once

#include "../core/base.h"
#include "../math/math2D.h"
#include "../shapes/line.h"
#include "../shapes/edge.h"
#include "../viewport/viewport.h"

#define GRID__MAX_SEGMENTS 11

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

void setGridEdgesFromVertices(Edge *edges, u8 edge_count, vec3 *from, vec3 *to) {
    for (u8 i = 0; i < edge_count; i++) {
        edges[i].from = from[i];
        edges[i].to   = to[i];
    }
}

bool initGrid(Grid *grid, f32 min_u, f32 min_v, f32 max_u, f32 max_v, u8 u_segments, u8 v_segments) {
    if (!u_segments || u_segments > GRID__MAX_SEGMENTS ||
        !v_segments || v_segments > GRID__MAX_SEGMENTS)
        return false;

    grid->u_segments = u_segments;
    grid->v_segments = v_segments;

    f32 u_step = u_segments > 1 ? ((max_u - min_u) / (u_segments - 1)) : 0;
    f32 v_step = v_segments > 1 ? ((max_v - min_v) / (v_segments - 1)) : 0;

    for (u8 u = 0; u < grid->u_segments; u++) {
        grid->vertices.uv.u.from[u].y = grid->vertices.uv.u.to[u].y = 0;
        grid->vertices.uv.u.from[u].x = grid->vertices.uv.u.to[u].x = min_u + u * u_step;
        grid->vertices.uv.u.from[u].z = min_v;
        grid->vertices.uv.u.to[  u].z = max_v;
    }
    for (u8 v = 0; v < grid->v_segments; v++) {
        grid->vertices.uv.v.from[v].y = grid->vertices.uv.v.to[v].y = 0;
        grid->vertices.uv.v.from[v].z = grid->vertices.uv.v.to[v].z = min_v + v * v_step;
        grid->vertices.uv.v.from[v].x = min_u;
        grid->vertices.uv.v.to[  v].x = max_u;
    }

    setGridEdgesFromVertices(grid->edges.uv.u, grid->u_segments, grid->vertices.uv.u.from, grid->vertices.uv.u.to);
    setGridEdgesFromVertices(grid->edges.uv.v, grid->v_segments, grid->vertices.uv.v.from, grid->vertices.uv.v.to);

    return true;
}

void drawGrid(Viewport *viewport, RGBA color, Grid *grid, Primitive *primitive) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    GridVertices vertices;
    vec3 position;
    for (u8 side = 0; side < 2; side++) {
        for (u8 axis = 0; axis < 2; axis++) {
            u8 segment_count = axis ? grid->v_segments : grid->u_segments;
            for (u8 segment = 0; segment < segment_count; segment++) {
                position = grid->vertices.buffer[axis][side][segment];
                position = convertPositionToWorldSpace(position, primitive);
                position = subVec3(    position, viewport->camera->transform.position);
                position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
                vertices.buffer[axis][side][segment] = position;
            }
        }
    }

    // Distribute transformed vertices positions to edges:
    GridEdges edges;
    setGridEdgesFromVertices(edges.uv.u, grid->u_segments, vertices.uv.u.from, vertices.uv.u.to);
    setGridEdgesFromVertices(edges.uv.v, grid->v_segments, vertices.uv.v.from, vertices.uv.v.to);

    for (u8 u = 0; u < grid->u_segments; u++) drawEdge(viewport, color, edges.uv.u + u);
    for (u8 v = 0; v < grid->v_segments; v++) drawEdge(viewport, color, edges.uv.v + v);
}