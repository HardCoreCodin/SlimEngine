#pragma once

#include "../core/base.h"
#include "../core/types.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void loadMeshFromFile(Mesh *mesh, String file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path.char_ptr);

    mesh->vertex_normals          = null;
    mesh->vertex_normal_indices   = null;
    mesh->vertex_uvs              = null;
    mesh->vertex_uvs_indices      = null;

    platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
    platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

    mesh->vertex_positions        = allocateMemory(memory, sizeof(vec3)                  * mesh->vertex_count);
    mesh->vertex_position_indices = allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
    mesh->edge_vertex_indices     = allocateMemory(memory, sizeof(EdgeVertexIndices)     * mesh->edge_count);

    platform->readFromFile(mesh->vertex_positions,             sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->readFromFile(mesh->vertex_position_indices,      sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->readFromFile(mesh->edge_vertex_indices,          sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
    if (mesh->uvs_count) {
        mesh->vertex_uvs         = allocateMemory(memory, sizeof(vec2)                  * mesh->uvs_count);
        mesh->vertex_uvs_indices = allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_uvs,               sizeof(vec2)                  * mesh->uvs_count,      file);
        platform->readFromFile(mesh->vertex_uvs_indices,       sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }
    if (mesh->normals_count) {
        mesh->vertex_normals          = allocateMemory(memory, sizeof(vec3)                  * mesh->normals_count);
        mesh->vertex_normal_indices   = allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_normals,                sizeof(vec3)                  * mesh->normals_count,  file);
        platform->readFromFile(mesh->vertex_normal_indices,         sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }

    platform->closeFile(file);
}

void saveMeshToFile(Mesh *mesh, String file_path, Platform *platform) {
    void *file = platform->openFileForWriting(file_path.char_ptr);

    platform->writeToFile(&mesh->aabb,           sizeof(AABB), file);
    platform->writeToFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->writeToFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->writeToFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->writeToFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->writeToFile(&mesh->normals_count,  sizeof(u32),  file);
    platform->writeToFile(mesh->vertex_positions,        sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->writeToFile(mesh->vertex_position_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->writeToFile(mesh->edge_vertex_indices,     sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
    if (mesh->uvs_count) {
        platform->writeToFile(mesh->vertex_uvs,          sizeof(vec2)                  * mesh->uvs_count,      file);
        platform->writeToFile(mesh->vertex_uvs_indices,  sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }
    if (mesh->normals_count) {
        platform->writeToFile(mesh->vertex_normals,        sizeof(vec3)                  * mesh->normals_count,  file);
        platform->writeToFile(mesh->vertex_normal_indices, sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }

    platform->closeFile(file);
}

void drawMesh(Viewport *viewport, RGBA color, Mesh *mesh, Primitive *primitive, bool draw_normals) {
    EdgeVertexIndices *edge_vertex_indices = mesh->edge_vertex_indices;
    quat cam_rot = viewport->camera->transform.rotation_inverted;
    vec3 cam_pos = viewport->camera->transform.position;
    vec3 *positions = mesh->vertex_positions;
    vec3 position;
    Edge edge;
    for (u32 i = 0; i < mesh->edge_count; i++, edge_vertex_indices++) {
        position = positions[edge_vertex_indices->from];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.from = position;

        position = positions[edge_vertex_indices->to];
        position = convertPositionToWorldSpace(position, primitive);
        position = subVec3(position,     cam_pos);
        position = mulVec3Quat(position, cam_rot);
        edge.to  = position;

        drawEdge(viewport, color, &edge);
    }

    if (draw_normals && mesh->normals_count && mesh->vertex_normals && mesh->vertex_normal_indices) {
        TriangleVertexIndices *vertex_normal_indices = mesh->vertex_normal_indices;
        TriangleVertexIndices *vertex_position_indices = mesh->vertex_position_indices;
        vec3 *normals = mesh->vertex_normals;
        for (u32 t = 0; t < mesh->triangle_count; t++, vertex_normal_indices++, vertex_position_indices++) {
            for (u8 i = 0; i < 3; i++) {
                position = positions[vertex_position_indices->ids[i]];
                edge.from = convertPositionToWorldSpace(position, primitive);
                edge.from = subVec3(edge.from,     cam_pos);
                edge.from = mulVec3Quat(edge.from, cam_rot);

                edge.to = addVec3(position, scaleVec3(normals[vertex_normal_indices->ids[i]], 0.1f));
                edge.to = convertPositionToWorldSpace(edge.to, primitive);
                edge.to = subVec3(edge.to,     cam_pos);
                edge.to = mulVec3Quat(edge.to, cam_rot);

                drawEdge(viewport, Color(Red), &edge);
            }
        }
    }
}