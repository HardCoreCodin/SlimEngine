#pragma once

#include "../core/init.h"
#include "../math/math3D.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void drawBox(Viewport *viewport, RGBA color, Box *box, Primitive *primitive, u8 sides) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    BoxVertices vertices;
    vec3 position;
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        position = box->vertices.buffer[i];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(    position, viewport->camera->transform.position);
        position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
        vertices.buffer[i] = position;
    }

    // Distribute transformed vertices positions to edges:
    BoxEdges edges;
    setBoxEdgesFromVertices(&edges, &vertices);

    if (sides == BOX__ALL_SIDES) {
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++)
            drawEdge(viewport, color, edges.buffer + i);
    } else {
        if (sides & Front | sides & Top   ) drawEdge(viewport, color, &edges.sides.front_top);
        if (sides & Front | sides & Bottom) drawEdge(viewport, color, &edges.sides.front_bottom);
        if (sides & Front | sides & Left  ) drawEdge(viewport, color, &edges.sides.front_left);
        if (sides & Front | sides & Right ) drawEdge(viewport, color, &edges.sides.front_right);
        if (sides & Back  | sides & Top   ) drawEdge(viewport, color, &edges.sides.back_top);
        if (sides & Back  | sides & Bottom) drawEdge(viewport, color, &edges.sides.back_bottom);
        if (sides & Back  | sides & Left  ) drawEdge(viewport, color, &edges.sides.back_left);
        if (sides & Back  | sides & Right ) drawEdge(viewport, color, &edges.sides.back_right);
        if (sides & Left  | sides & Top   ) drawEdge(viewport, color, &edges.sides.left_top);
        if (sides & Left  | sides & Bottom) drawEdge(viewport, color, &edges.sides.left_bottom);
        if (sides & Right | sides & Top   ) drawEdge(viewport, color, &edges.sides.right_top);
        if (sides & Right | sides & Bottom) drawEdge(viewport, color, &edges.sides.right_bottom);
    }
}