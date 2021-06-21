#pragma once

#include "../core/base.h"
#include "../math/math3D.h"
#include "../shapes/edge.h"
#include "../viewport/viewport.h"
#include "./primitive.h"

typedef enum BoxSide {
    NoSide = 0,
    Top    = 1,
    Bottom = 2,
    Left   = 4,
    Right  = 8,
    Front  = 16,
    Back   = 32
} BoxSide;

#define BOX__ALL_SIDES (Top | Bottom | Left | Right | Front | Back)
#define BOX__VERTEX_COUNT 8
#define BOX__EDGE_COUNT 12

typedef struct BoxCorners {
    vec3 front_top_left,
         front_top_right,
         front_bottom_left,
         front_bottom_right,
         back_top_left,
         back_top_right,
         back_bottom_left,
         back_bottom_right;
} BoxCorners;

typedef union BoxVertices {
    BoxCorners corners;
    vec3 buffer[BOX__VERTEX_COUNT];
} BoxVertices;

typedef struct BoxEdgeSides {
    Edge front_top,
         front_bottom,
         front_left,
         front_right,
         back_top,
         back_bottom,
         back_left,
         back_right,
         left_bottom,
         left_top,
         right_bottom,
         right_top;
} BoxEdgeSides;

typedef union BoxEdges {
    BoxEdgeSides sides;
    Edge buffer[BOX__EDGE_COUNT];
} BoxEdges;

typedef struct Box {
    BoxVertices vertices;
    BoxEdges edges;
} Box;

void setBoxEdgesFromVertices(BoxEdges *edges, BoxVertices *vertices) {
    edges->sides.front_top.from    = vertices->corners.front_top_left;
    edges->sides.front_top.to      = vertices->corners.front_top_right;
    edges->sides.front_bottom.from = vertices->corners.front_bottom_left;
    edges->sides.front_bottom.to   = vertices->corners.front_bottom_right;
    edges->sides.front_left.from   = vertices->corners.front_bottom_left;
    edges->sides.front_left.to     = vertices->corners.front_top_left;
    edges->sides.front_right.from  = vertices->corners.front_bottom_right;
    edges->sides.front_right.to    = vertices->corners.front_top_right;

    edges->sides.back_top.from     = vertices->corners.back_top_left;
    edges->sides.back_top.to       = vertices->corners.back_top_right;
    edges->sides.back_bottom.from  = vertices->corners.back_bottom_left;
    edges->sides.back_bottom.to    = vertices->corners.back_bottom_right;
    edges->sides.back_left.from    = vertices->corners.back_bottom_left;
    edges->sides.back_left.to      = vertices->corners.back_top_left;
    edges->sides.back_right.from   = vertices->corners.back_bottom_right;
    edges->sides.back_right.to     = vertices->corners.back_top_right;

    edges->sides.left_bottom.from  = vertices->corners.front_bottom_left;
    edges->sides.left_bottom.to    = vertices->corners.back_bottom_left;
    edges->sides.left_top.from     = vertices->corners.front_top_left;
    edges->sides.left_top.to       = vertices->corners.back_top_left;
    edges->sides.right_bottom.from = vertices->corners.front_bottom_right;
    edges->sides.right_bottom.to   = vertices->corners.back_bottom_right;
    edges->sides.right_top.from    = vertices->corners.front_top_right;
    edges->sides.right_top.to      = vertices->corners.back_top_right;
}

void initBox(Box *box, vec3 min, vec3 max) {
    box->vertices.corners.front_top_left.x = min.x;
    box->vertices.corners.back_top_left.x = min.x;
    box->vertices.corners.front_bottom_left.x = min.x;
    box->vertices.corners.back_bottom_left.x = min.x;

    box->vertices.corners.front_top_right.x = max.x;
    box->vertices.corners.back_top_right.x = max.x;
    box->vertices.corners.front_bottom_right.x = max.x;
    box->vertices.corners.back_bottom_right.x = max.x;


    box->vertices.corners.front_bottom_left.y = min.y;
    box->vertices.corners.front_bottom_right.y = min.y;
    box->vertices.corners.back_bottom_left.y = min.y;
    box->vertices.corners.back_bottom_right.y = min.y;

    box->vertices.corners.front_top_left.y = max.y;
    box->vertices.corners.front_top_right.y = max.y;
    box->vertices.corners.back_top_left.y = max.y;
    box->vertices.corners.back_top_right.y = max.y;


    box->vertices.corners.front_top_left.z = max.z;
    box->vertices.corners.front_top_right.z = max.z;
    box->vertices.corners.front_bottom_left.z = max.z;
    box->vertices.corners.front_bottom_right.z = max.z;

    box->vertices.corners.back_top_left.z = min.z;
    box->vertices.corners.back_top_right.z = min.z;
    box->vertices.corners.back_bottom_left.z = min.z;
    box->vertices.corners.back_bottom_right.z = min.z;

    setBoxEdgesFromVertices(&box->edges, &box->vertices);
}

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