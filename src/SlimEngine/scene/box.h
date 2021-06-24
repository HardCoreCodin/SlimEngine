#pragma once

#include "../core/init.h"
#include "../math/quat.h"
#include "../math/vec3.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void drawBox(Viewport *viewport, RGBA color, Box *box, Primitive *primitive, u8 sides) {
    // Transform vertices positions from local-space to world-space and then to view-space:
    static BoxVertices vertices;
    vec3 position;
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        position = box->vertices.buffer[i];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(    position, viewport->camera->transform.position);
        position = mulVec3Quat(position, viewport->camera->transform.rotation_inverted);
        vertices.buffer[i] = position;
    }

    // Distribute transformed vertices positions to edges:
    static BoxEdges edges;
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

void drawCamera(Viewport *viewport, RGBA color, Camera *camera) {
    static Box box;
    static Primitive primitive;
    initBox(&box);
    primitive.flags = ALL_FLAGS;
    primitive.rotation = camera->transform.rotation;
    primitive.position = camera->transform.position;
    primitive.scale.x  = primitive.scale.y = primitive.scale.z = 1;
    drawBox(viewport, color, &box, &primitive, BOX__ALL_SIDES);
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
    drawBox(viewport, color, &box, &primitive, BOX__ALL_SIDES);
}