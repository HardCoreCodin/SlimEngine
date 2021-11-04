#pragma once

#include "../math/vec4.h"


u8 shadeMesh(Mesh *mesh, Rasterizer *rasterizer) {

    // Transform the mesh's vertex positions into clip space and world space:
    vec4 world_space;
    for (u32 i = 0; i < mesh->vertex_count; i++) {
        world_space = Vec4fromVec3(mesh->vertex_positions[i], 1.0f);
        world_space = mulVec4Mat4(world_space, rasterizer->model_to_world);
        rasterizer->clip_space_vertex_positions[i] = mulVec4Mat4(world_space, rasterizer->world_to_clip);
        rasterizer->world_space_vertex_positions[i] = world_space.v3;
    }

    // Transform the mesh's vertex normals into world space:
    for (u32 i = 0; i < mesh->normals_count; i++) {
        world_space = Vec4fromVec3(mesh->vertex_normals[i], 0.0f);
        world_space = mulVec4Mat4(world_space, rasterizer->model_to_world_inverted_transposed);
        rasterizer->world_space_vertex_normals[i] = world_space.v3;
    }

    return INSIDE;
}

