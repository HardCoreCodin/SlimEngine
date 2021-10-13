#pragma once

#include "../core/init.h"
#include "../math/quat.h"
#include "../math/vec3.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void transformBoxVerticesFromObjectToViewSpace(BoxVertices *vertices, BoxVertices *transformed_vertices, Primitive *primitive, Viewport *viewport) {
    vec3 position;
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        position = vertices->buffer[i];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(    position, viewport->camera->transform.position);
        position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
        transformed_vertices->buffer[i] = position;
    }
}

void drawBox(Box *box, u8 sides, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    static BoxVertices vertices;
    transformBoxVerticesFromObjectToViewSpace(&box->vertices, &vertices, primitive, viewport);

    // Distribute transformed vertices positions to edges:
    static BoxEdges edges;
    setBoxEdgesFromVertices(&edges, &vertices);

    if (sides == BOX__ALL_SIDES) {
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++)
            drawEdge(edges.buffer + i, color, opacity, line_width, viewport);
    } else {
        if (sides & Front | sides & Top   ) drawEdge(&edges.sides.front_top,    color, opacity, line_width, viewport);
        if (sides & Front | sides & Bottom) drawEdge(&edges.sides.front_bottom, color, opacity, line_width, viewport);
        if (sides & Front | sides & Left  ) drawEdge(&edges.sides.front_left,   color, opacity, line_width, viewport);
        if (sides & Front | sides & Right ) drawEdge(&edges.sides.front_right,  color, opacity, line_width, viewport);
        if (sides & Back  | sides & Top   ) drawEdge(&edges.sides.back_top,     color, opacity, line_width, viewport);
        if (sides & Back  | sides & Bottom) drawEdge(&edges.sides.back_bottom,  color, opacity, line_width, viewport);
        if (sides & Back  | sides & Left  ) drawEdge(&edges.sides.back_left,    color, opacity, line_width, viewport);
        if (sides & Back  | sides & Right ) drawEdge(&edges.sides.back_right,   color, opacity, line_width, viewport);
        if (sides & Left  | sides & Top   ) drawEdge(&edges.sides.left_top,     color, opacity, line_width, viewport);
        if (sides & Left  | sides & Bottom) drawEdge(&edges.sides.left_bottom,  color, opacity, line_width, viewport);
        if (sides & Right | sides & Top   ) drawEdge(&edges.sides.right_top,    color, opacity, line_width, viewport);
        if (sides & Right | sides & Bottom) drawEdge(&edges.sides.right_bottom, color, opacity, line_width, viewport);
    }
}

void drawCamera(Camera *camera, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
    static Box box;
    static Primitive primitive;
    initBox(&box);
    primitive.flags = ALL_FLAGS;
    primitive.rotation = camera->transform.rotation;
    primitive.position = camera->transform.position;
    primitive.scale.x  = primitive.scale.y = primitive.scale.z = 1;
    drawBox(&box, BOX__ALL_SIDES, &primitive, color, opacity, line_width, viewport);
    box.vertices.corners.back_bottom_left   = scaleVec3(box.vertices.corners.back_bottom_left,  0.5f);
    box.vertices.corners.back_bottom_right  = scaleVec3(box.vertices.corners.back_bottom_right, 0.5f);
    box.vertices.corners.back_top_left      = scaleVec3(box.vertices.corners.back_top_left,     0.5f);
    box.vertices.corners.back_top_right     = scaleVec3(box.vertices.corners.back_top_right,    0.5f);
    box.vertices.corners.front_bottom_left  = scaleVec3(box.vertices.corners.front_bottom_left,  2);
    box.vertices.corners.front_bottom_right = scaleVec3(box.vertices.corners.front_bottom_right, 2);
    box.vertices.corners.front_top_left     = scaleVec3(box.vertices.corners.front_top_left,     2);
    box.vertices.corners.front_top_right    = scaleVec3(box.vertices.corners.front_top_right,    2);
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        box.vertices.buffer[i].z += 1.5f;
    drawBox(&box, BOX__ALL_SIDES, &primitive, color, opacity, line_width, viewport);
}