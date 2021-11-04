#pragma once

#include "../core/base.h"
#include "../core/types.h"

u32 getTextureMemorySize(Texture *texture, char* file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    platform->readFromFile(&texture->width,  sizeof(u16),  file);
    platform->readFromFile(&texture->height, sizeof(u16),  file);
    platform->readFromFile(&texture->filter, sizeof(bool), file);
    platform->readFromFile(&texture->mipmap, sizeof(bool), file);

    u16 mip_width  = texture->width;
    u16 mip_height = texture->height;

    u32 memory_size = 0;

    do {
        memory_size += sizeof(TextureMip);
        memory_size += mip_width * mip_height * sizeof(vec4);
        if (texture->filter) {
            memory_size += (mip_width + 1) * (mip_height + 1) * sizeof(TexelQuad);
            memory_size += (mip_height + 1) * sizeof(TexelQuad*);
        }

        mip_width /= 2;
        mip_height /= 2;
    } while (texture->mipmap && mip_width > 2 && mip_height > 2);

    platform->closeFile(file);

    return memory_size;
}

void loadTextureFromFile(Texture *texture, char* file_path, Platform *platform, Memory *memory) {
    void *file = platform->openFileForReading(file_path);
    platform->readFromFile(&texture->width,  sizeof(u16),  file);
    platform->readFromFile(&texture->height, sizeof(u16),  file);
    platform->readFromFile(&texture->filter, sizeof(bool), file);
    platform->readFromFile(&texture->mipmap, sizeof(bool), file);
    platform->readFromFile(&texture->wrap,   sizeof(bool), file);
    platform->readFromFile(&texture->mip_count, sizeof(u8), file);

    texture->mips = (TextureMip*)allocateMemory(memory, sizeof(TextureMip) * texture->mip_count);

    u32 size, height, stride;
    TextureMip *texture_mip = texture->mips;
    for (u8 mip_index = 0; mip_index < texture->mip_count; mip_index++, texture_mip++) {
        platform->readFromFile(&texture_mip->width,  sizeof(u16), file);
        platform->readFromFile(&texture_mip->height, sizeof(u16), file);

        size = sizeof(vec4) * texture_mip->width * texture_mip->height;
        texture_mip->texels = (vec4*)allocateMemory(memory, size);
        platform->readFromFile(texture_mip->texels, size, file);

        if (texture->filter) {
            height = texture_mip->height + 1;
            stride = texture_mip->width  + 1;

            size = sizeof(TexelQuad) * height * stride;
            texture_mip->texel_quads = (TexelQuad*)allocateMemory(memory, size);
            platform->readFromFile(texture_mip->texel_quads, size, file);

            size = sizeof(TexelQuad*) * height;
            texture_mip->texel_quad_lines = (TexelQuad**)allocateMemory(memory, size);
            TexelQuad *line_start = texture_mip->texel_quads;
            TexelQuad **line = texture_mip->texel_quad_lines;
            for (u16 y = 0; y < height; y++, line++, line_start += stride) *line = line_start;
        }
    }

    platform->closeFile(file);
}

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

void writeSceneSettingsToFile(SceneSettings *settings, void *file, Platform *platform) {
    platform->writeToFile(&settings->boxes, sizeof(u32), file);
    platform->writeToFile(&settings->cameras, sizeof(u32), file);
    platform->writeToFile(&settings->curves, sizeof(u32), file);
    platform->writeToFile(&settings->grids, sizeof(u32), file);
    platform->writeToFile(&settings->meshes, sizeof(u32), file);
    platform->writeToFile(&settings->primitives, sizeof(u32), file);
}

void readSceneSettingsFromFile(SceneSettings *settings, void *file, Platform *platform) {
    platform->readFromFile(&settings->boxes, sizeof(u32), file);
    platform->readFromFile(&settings->cameras, sizeof(u32), file);
    platform->readFromFile(&settings->curves, sizeof(u32), file);
    platform->readFromFile(&settings->grids, sizeof(u32), file);
    platform->readFromFile(&settings->meshes, sizeof(u32), file);
    platform->readFromFile(&settings->primitives, sizeof(u32), file);
}

void writeTransformToFile(xform3 *xform, void *file, Platform *platform) {
    platform->writeToFile(&xform->matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->yaw_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->pitch_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->roll_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->rotation_matrix, sizeof(mat3), file);
    platform->writeToFile(&xform->rotation_matrix_inverted, sizeof(mat3), file);

    platform->writeToFile(&xform->rotation, sizeof(quat), file);
    platform->writeToFile(&xform->rotation_inverted, sizeof(quat), file);

    platform->writeToFile(&xform->position, sizeof(vec3), file);
    platform->writeToFile(&xform->scale, sizeof(vec3), file);
}

void readTransformFromFile(xform3 *xform, void *file, Platform *platform) {
    platform->readFromFile(&xform->matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->yaw_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->pitch_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->roll_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->rotation_matrix, sizeof(mat3), file);
    platform->readFromFile(&xform->rotation_matrix_inverted, sizeof(mat3), file);

    platform->readFromFile(&xform->rotation, sizeof(quat), file);
    platform->readFromFile(&xform->rotation_inverted, sizeof(quat), file);

    platform->readFromFile(&xform->position, sizeof(vec3), file);
    platform->readFromFile(&xform->scale, sizeof(vec3), file);

    xform->right_direction   = &xform->rotation_matrix.X;
    xform->up_direction      = &xform->rotation_matrix.Y;
    xform->forward_direction = &xform->rotation_matrix.Z;
}

void loadSceneFromFile(Scene *scene, char* file_path, Platform *platform) {
    void *file = platform->openFileForReading(file_path);

    readSceneSettingsFromFile(&scene->settings, file, platform);

    if (scene->cameras) {
        Camera *camera = scene->cameras;
        for (u32 i = 0; i < scene->settings.cameras; i++, camera++) {
            platform->readFromFile(&camera->focal_length, sizeof(f32), file);
            platform->readFromFile(&camera->zoom, sizeof(f32), file);
            platform->readFromFile(&camera->dolly, sizeof(f32), file);
            platform->readFromFile(&camera->target_distance, sizeof(f32), file);
            platform->readFromFile(&camera->current_velocity, sizeof(vec3), file);
            readTransformFromFile(&camera->transform, file, platform);
        }
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

    writeSceneSettingsToFile(&scene->settings, file, platform);

    if (scene->cameras) {
        Camera *camera = scene->cameras;
        for (u32 i = 0; i < scene->settings.cameras; i++, camera++) {
            platform->writeToFile(&camera->focal_length, sizeof(f32), file);
            platform->writeToFile(&camera->zoom, sizeof(f32), file);
            platform->writeToFile(&camera->dolly, sizeof(f32), file);
            platform->writeToFile(&camera->target_distance, sizeof(f32), file);
            platform->writeToFile(&camera->current_velocity, sizeof(vec3), file);
            writeTransformToFile(&camera->transform, file, platform);
        }
    }

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
        }
    }

    platform->closeFile(file);
}