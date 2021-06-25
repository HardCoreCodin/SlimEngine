#pragma once

#include "../core/base.h"
#include "../core/types.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void loadMeshFromFile(Mesh *mesh, String file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path.char_ptr);

    platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
    platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);

    mesh->vertex_positions        = allocateMemory(memory, sizeof(vec3)                  * mesh->vertex_count);
    mesh->triangle_vertex_indices = allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
    mesh->edge_vertex_indices     = allocateMemory(memory, sizeof(EdgeVertexIndices)     * mesh->edge_count);

    platform->readFromFile(mesh->vertex_positions,        sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->readFromFile(mesh->triangle_vertex_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->readFromFile(mesh->edge_vertex_indices,     sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);

    platform->closeFile(file);
}

void drawMesh(Viewport *viewport, RGBA color, Mesh *mesh, Primitive *primitive) {
    EdgeVertexIndices *ids = mesh->edge_vertex_indices;
    quat cam_rot = viewport->camera->transform.rotation_inverted;
    vec3 cam_pos = viewport->camera->transform.position;
    vec3 *positions = mesh->vertex_positions;
    vec3 position;
    Edge edge;
    for (u32 i = 0; i < mesh->edge_count; i++, ids++) {
        position = positions[ids->from];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.from = position;

        position = positions[ids->to];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.to  = position;

        drawEdge(viewport, color, &edge);
    }
}