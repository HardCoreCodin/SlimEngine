#pragma once

#include "../core/types.h"
#include "../math/vec3.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void transformGridVerticesFromObjectToViewSpace(Grid *grid, GridVertices *transformed_vertices, Primitive *primitive, Viewport *viewport) {
    vec3 position;
    for (u8 side = 0; side < 2; side++) {
        for (u8 axis = 0; axis < 2; axis++) {
            u8 segment_count = axis ? grid->v_segments : grid->u_segments;
            for (u8 segment = 0; segment < segment_count; segment++) {
                position = grid->vertices.buffer[axis][side][segment];
                position = convertPositionToWorldSpace(position, primitive);
                position = subVec3(    position, viewport->camera->transform.position);
                position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
                transformed_vertices->buffer[axis][side][segment] = position;
            }
        }
    }
}

void drawGrid(Grid *grid, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    static GridVertices vertices;

    transformGridVerticesFromObjectToViewSpace(grid, &vertices, primitive, viewport);

    // Distribute transformed vertices positions to edges:
    static GridEdges edges;
    setGridEdgesFromVertices(edges.uv.u, grid->u_segments, vertices.uv.u.from, vertices.uv.u.to);
    setGridEdgesFromVertices(edges.uv.v, grid->v_segments, vertices.uv.v.from, vertices.uv.v.to);

    for (u8 u = 0; u < grid->u_segments; u++) drawEdge(edges.uv.u + u, color, opacity, line_width, viewport);
    for (u8 v = 0; v < grid->v_segments; v++) drawEdge(edges.uv.v + v, color, opacity, line_width, viewport);
}