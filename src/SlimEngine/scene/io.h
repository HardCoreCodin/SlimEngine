#pragma once

#include "../core/base.h"
#include "../core/types.h"

u32 getMeshMemorySize(Mesh *mesh, char *file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
    platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
    platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
    platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
    platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
    platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

    u32 memory_size = sizeof(vec3) * mesh->vertex_count;
    memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    memory_size += sizeof(EdgeVertexIndices) * mesh->edge_count;

    if (mesh->uvs_count) {
        memory_size += sizeof(vec2) * mesh->uvs_count;
        memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    }
    if (mesh->normals_count) {
        memory_size += sizeof(vec3) * mesh->normals_count;
        memory_size += sizeof(TriangleVertexIndices) * mesh->triangle_count;
    }

    platform->closeFile(file);

    return memory_size;
}

void loadMeshFromFile(Mesh *mesh, char *file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path);

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

    mesh->vertex_positions        = (vec3*                 )allocateMemory(memory, sizeof(vec3)                  * mesh->vertex_count);
    mesh->vertex_position_indices = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
    mesh->edge_vertex_indices     = (EdgeVertexIndices*    )allocateMemory(memory, sizeof(EdgeVertexIndices)     * mesh->edge_count);

    platform->readFromFile(mesh->vertex_positions,             sizeof(vec3)                  * mesh->vertex_count,   file);
    platform->readFromFile(mesh->vertex_position_indices,      sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    platform->readFromFile(mesh->edge_vertex_indices,          sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
    if (mesh->uvs_count) {
        mesh->vertex_uvs         = (vec2*                 )allocateMemory(memory, sizeof(vec2)                  * mesh->uvs_count);
        mesh->vertex_uvs_indices = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_uvs,               sizeof(vec2)                  * mesh->uvs_count,      file);
        platform->readFromFile(mesh->vertex_uvs_indices,       sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }
    if (mesh->normals_count) {
        mesh->vertex_normals          = (vec3*                 )allocateMemory(memory, sizeof(vec3)                  * mesh->normals_count);
        mesh->vertex_normal_indices   = (TriangleVertexIndices*)allocateMemory(memory, sizeof(TriangleVertexIndices) * mesh->triangle_count);
        platform->readFromFile(mesh->vertex_normals,                sizeof(vec3)                  * mesh->normals_count,  file);
        platform->readFromFile(mesh->vertex_normal_indices,         sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
    }

    platform->closeFile(file);
}

void saveMeshToFile(Mesh *mesh, char* file_path, Platform *platform) {
    void *file = platform->openFileForWriting(file_path);

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

void loadSceneFromFile(Scene *scene, char* file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    platform->readFromFile(&scene->settings, sizeof(SceneSettings), file);

    if (scene->cameras)
        for (u32 i = 0; i < scene->settings.cameras; i++) {
            platform->readFromFile(scene->cameras + i, sizeof(Camera), file);
            scene->cameras[i].transform.right_direction   = &scene->cameras[i].transform.rotation_matrix.X;
            scene->cameras[i].transform.up_direction      = &scene->cameras[i].transform.rotation_matrix.Y;
            scene->cameras[i].transform.forward_direction = &scene->cameras[i].transform.rotation_matrix.Z;
        }

    if (scene->primitives)
        for (u32 i = 0; i < scene->settings.primitives; i++)
            platform->readFromFile(scene->primitives + i, sizeof(Primitive), file);

    if (scene->grids)
        for (u32 i = 0; i < scene->settings.grids; i++)
            platform->readFromFile(scene->grids + i, sizeof(Grid), file);

    if (scene->boxes)
        for (u32 i = 0; i < scene->settings.boxes; i++)
            platform->readFromFile(scene->boxes + i, sizeof(Box), file);

    if (scene->curves)
        for (u32 i = 0; i < scene->settings.curves; i++)
            platform->readFromFile(scene->curves + i, sizeof(Curve), file);

    if (scene->meshes) {
        Mesh *mesh = scene->meshes;
        for (u32 i = 0; i < scene->settings.meshes; i++, mesh++) {
            platform->readFromFile(&mesh->aabb,           sizeof(AABB), file);
            platform->readFromFile(&mesh->vertex_count,   sizeof(u32),  file);
            platform->readFromFile(&mesh->triangle_count, sizeof(u32),  file);
            platform->readFromFile(&mesh->edge_count,     sizeof(u32),  file);
            platform->readFromFile(&mesh->uvs_count,      sizeof(u32),  file);
            platform->readFromFile(&mesh->normals_count,  sizeof(u32),  file);

            platform->readFromFile(mesh->vertex_positions,             sizeof(vec3)                  * mesh->vertex_count,   file);
            platform->readFromFile(mesh->vertex_position_indices,      sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            platform->readFromFile(mesh->edge_vertex_indices,          sizeof(EdgeVertexIndices)     * mesh->edge_count,     file);
            if (mesh->uvs_count) {
                platform->readFromFile(mesh->vertex_uvs,               sizeof(vec2)                  * mesh->uvs_count,      file);
                platform->readFromFile(mesh->vertex_uvs_indices,       sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
            if (mesh->normals_count) {
                platform->readFromFile(mesh->vertex_normals,                sizeof(vec3)                  * mesh->normals_count,  file);
                platform->readFromFile(mesh->vertex_normal_indices,         sizeof(TriangleVertexIndices) * mesh->triangle_count, file);
            }
        };
    }

    platform->closeFile(file);
}

void saveSceneToFile(Scene *scene, char* file_path, Platform *platform) {
    void *file = platform->openFileForWriting(file_path);

    platform->writeToFile(&scene->settings, sizeof(SceneSettings), file);

    if (scene->cameras)
        for (u32 i = 0; i < scene->settings.cameras; i++)
            platform->writeToFile(scene->cameras + i, sizeof(Camera), file);

    if (scene->primitives)
        for (u32 i = 0; i < scene->settings.primitives; i++)
            platform->writeToFile(scene->primitives + i, sizeof(Primitive), file);

    if (scene->grids)
        for (u32 i = 0; i < scene->settings.grids; i++)
            platform->writeToFile(scene->grids + i, sizeof(Grid), file);

    if (scene->boxes)
        for (u32 i = 0; i < scene->settings.boxes; i++)
            platform->writeToFile(scene->boxes + i, sizeof(Box), file);

    if (scene->curves)
        for (u32 i = 0; i < scene->settings.curves; i++)
            platform->writeToFile(scene->curves + i, sizeof(Curve), file);

    if (scene->meshes) {
        Mesh *mesh = scene->meshes;
        for (u32 i = 0; i < scene->settings.meshes; i++, mesh++) {
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
        };
    }

    platform->closeFile(file);
}