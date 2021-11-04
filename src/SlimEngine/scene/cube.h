#pragma once

#include "../core/base.h"


#define CUBE__UV_COUNT 4
#define CUBE__NORMAL_COUNT 6
#define CUBE__VERTEX_COUNT 8
#define CUBE__TRIANGLE_COUNT 12

static const vec3 CUBE__VERTEX_POSITIONS[CUBE__VERTEX_COUNT] = {
    {-1, -1, -1},
    {1, -1, -1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, 1},
    {-1, 1, 1}
};

static const TriangleVertexIndices CUBE__VERTEX_POSITION_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 1, 2},
        {1, 5, 6},
        {5, 4, 7},
        {4, 0, 3},
        {3, 2, 6},
        {1, 0, 4},
        {0, 2, 3},
        {1, 6, 2},
        {5, 7, 6},
        {4, 3, 7},
        {3, 6, 7},
        {1, 4, 5}
};

static const vec3 CUBE__VERTEX_NORMALS[CUBE__NORMAL_COUNT] = {
        {0, 0, -1},
        {1, 0, 0},
        {0, 0, 1},
        {-1, 0, 0},
        {0, 1, 0},
        {0, -1, 0}
};
static const TriangleVertexIndices CUBE__VERTEX_NORMAL_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 0, 0},
        {1, 1, 1},
        {2, 2, 2},
        {3, 3, 3},
        {4, 4, 4},
        {5, 5, 5},
        {0, 0, 0},
        {1, 1, 1},
        {2, 2, 2},
        {3, 3, 3},
        {4, 4, 4},
        {5, 5, 5}
};

static const vec2 CUBE__VERTEX_UVS[CUBE__UV_COUNT] = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0},
};
static const TriangleVertexIndices CUBE__VERTEX_UV_INDICES[CUBE__TRIANGLE_COUNT] = {
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1, 2},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3},
        {0, 2, 3}
};

void setMeshToCube(Mesh *mesh) {
    mesh->triangle_count = CUBE__TRIANGLE_COUNT;
    mesh->vertex_count   = CUBE__VERTEX_COUNT;
    mesh->normals_count  = CUBE__NORMAL_COUNT;
    mesh->uvs_count      = CUBE__UV_COUNT;

    mesh->vertex_uvs       = (vec2*)CUBE__VERTEX_UVS;
    mesh->vertex_normals   = (vec3*)CUBE__VERTEX_NORMALS;
    mesh->vertex_positions = (vec3*)CUBE__VERTEX_POSITIONS;

    mesh->vertex_uvs_indices      = (TriangleVertexIndices*)CUBE__VERTEX_UV_INDICES;
    mesh->vertex_normal_indices   = (TriangleVertexIndices*)CUBE__VERTEX_NORMAL_INDICES;
    mesh->vertex_position_indices = (TriangleVertexIndices*)CUBE__VERTEX_POSITION_INDICES;
}