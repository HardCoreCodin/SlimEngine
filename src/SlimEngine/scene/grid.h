#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../shapes/edge.h"
#include "./primitive.h"

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