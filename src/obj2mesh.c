#ifdef COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "./SlimEngine/core/types.h"

enum VertexAttributes {
    VertexAttributes_None,
    VertexAttributes_Positions,
    VertexAttributes_PositionsAndUVs,
    VertexAttributes_PositionsUVsAndNormals
};

int obj2mesh(char* obj_file_path, char* mesh_file_path) {
    Mesh mesh;
    mesh.aabb.min.x = mesh.aabb.min.y = mesh.aabb.min.z = 0;
    mesh.aabb.max.x = mesh.aabb.max.y = mesh.aabb.max.z = 0;
    mesh.triangle_count = 0;
    mesh.normals_count = 0;
    mesh.vertex_count = 0;
    mesh.edge_count = 0;
    mesh.uvs_count = 0;
    mesh.vertex_normals          = null;
    mesh.vertex_normal_indices   = null;
    mesh.vertex_uvs              = null;
    mesh.vertex_uvs_indices      = null;

    FILE* file;
    file = fopen(obj_file_path, "r");
    char line[1024];

    enum VertexAttributes vertex_attributes = VertexAttributes_None;
    while (fgets(line, 1024, file)) {
        if (strncmp(line, "vn ", 2) == 0) mesh.normals_count++;
        if (strncmp(line, "vt ", 2) == 0) mesh.uvs_count++;
        if (strncmp(line, "v ", 2) == 0) mesh.vertex_count++;
        if (strncmp(line, "f ", 2) == 0) {
            mesh.triangle_count++;
            if (vertex_attributes == VertexAttributes_None) {
                int forward_slash_count = 0;
                char *character = line;
                while (*character) {
                    if ((*character) == '/') forward_slash_count++;
                    character++;
                }
                switch (forward_slash_count) {
                    case 0: vertex_attributes = VertexAttributes_Positions; break;
                    case 3: vertex_attributes = VertexAttributes_PositionsAndUVs; break;
                    case 6: vertex_attributes = VertexAttributes_PositionsUVsAndNormals; break;
                    default: break;
                }
            }
        }
    }
    fclose(file);

    mesh.vertex_position_indices = (TriangleVertexIndices*)malloc(sizeof(TriangleVertexIndices) * mesh.triangle_count);
    mesh.vertex_positions        = (                 vec3*)malloc(sizeof(vec3                 ) * mesh.vertex_count);
    mesh.edge_vertex_indices     = (    EdgeVertexIndices*)malloc(sizeof(EdgeVertexIndices    ) * mesh.triangle_count * 3);

    if (vertex_attributes == VertexAttributes_PositionsUVsAndNormals) {
        mesh.vertex_normals        = (                 vec3*)malloc(sizeof(vec3                 ) * mesh.normals_count);
        mesh.vertex_normal_indices = (TriangleVertexIndices*)malloc(sizeof(TriangleVertexIndices) * mesh.triangle_count);
        mesh.vertex_uvs            = (                 vec2*)malloc(sizeof(vec2                 ) * mesh.uvs_count);
        mesh.vertex_uvs_indices    = (TriangleVertexIndices*)malloc(sizeof(TriangleVertexIndices) * mesh.triangle_count);
    } else if (vertex_attributes == VertexAttributes_PositionsAndUVs) {
        mesh.vertex_uvs            = (                 vec2*)malloc(sizeof(vec2                 ) * mesh.uvs_count);
        mesh.vertex_uvs_indices    = (TriangleVertexIndices*)malloc(sizeof(TriangleVertexIndices) * mesh.triangle_count);
    }

    vec3 *vertex_position = mesh.vertex_positions;
    vec3 *vertex_normal = mesh.vertex_normals;
    vec2 *vertex_uvs = mesh.vertex_uvs;
    TriangleVertexIndices *vertex_position_indices = mesh.vertex_position_indices;
    TriangleVertexIndices *vertex_normal_indices = mesh.vertex_normal_indices;
    TriangleVertexIndices *vertex_uvs_indices = mesh.vertex_uvs_indices;

    file = fopen(obj_file_path, "r");
    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", &vertex_position->x, &vertex_position->y, &vertex_position->z);
            vertex_position++;
        } else if (strncmp(line, "vn ", 2) == 0) {
            sscanf(line, "vn %f %f %f", &vertex_normal->x, &vertex_normal->y, &vertex_normal->z);
            vertex_normal++;
        } else if (strncmp(line, "vt ", 2) == 0) {
            sscanf(line, "vt %f %f", &vertex_uvs->x, &vertex_uvs->y);
            vertex_uvs++;
        } else if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int uvs_indices[3];
            int normal_indices[3];

            switch (vertex_attributes) {
                case VertexAttributes_Positions:
                    sscanf(
                            line, "f %d %d %d",
                            &vertex_indices[0],
                            &vertex_indices[1],
                            &vertex_indices[2]
                    );
                    break;
                case VertexAttributes_PositionsAndUVs:
                    sscanf(
                            line, "f %d/%d %d/%d %d/%d",
                            &vertex_indices[0], &uvs_indices[0],
                            &vertex_indices[1], &uvs_indices[1],
                            &vertex_indices[2], &uvs_indices[2]
                    );
                    vertex_uvs_indices->ids[0] = uvs_indices[0] - 1;
                    vertex_uvs_indices->ids[1] = uvs_indices[1] - 1;
                    vertex_uvs_indices->ids[2] = uvs_indices[2] - 1;
                    vertex_uvs_indices++;
                    break;
                case VertexAttributes_PositionsUVsAndNormals:
                    sscanf(
                            line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                            &vertex_indices[0], &uvs_indices[0], &normal_indices[0],
                            &vertex_indices[1], &uvs_indices[1], &normal_indices[1],
                            &vertex_indices[2], &uvs_indices[2], &normal_indices[2]
                    );
                    vertex_uvs_indices->ids[0] = uvs_indices[0] - 1;
                    vertex_uvs_indices->ids[1] = uvs_indices[1] - 1;
                    vertex_uvs_indices->ids[2] = uvs_indices[2] - 1;
                    vertex_normal_indices->ids[0] = normal_indices[0] - 1;
                    vertex_normal_indices->ids[1] = normal_indices[1] - 1;
                    vertex_normal_indices->ids[2] = normal_indices[2] - 1;
                    vertex_normal_indices++;
                    vertex_uvs_indices++;
                    break;
                default:
                    return 1;
            }
            vertex_position_indices->ids[0] = vertex_indices[0] - 1;
            vertex_position_indices->ids[1] = vertex_indices[1] - 1;
            vertex_position_indices->ids[2] = vertex_indices[2] - 1;
            vertex_position_indices++;
        }
    }
    fclose(file);

    vertex_position = mesh.vertex_positions;
    for (u32 i = 0; i < mesh.vertex_count; i++, vertex_position++) {
        mesh.aabb.min.x = mesh.aabb.min.x < vertex_position->x ? mesh.aabb.min.x : vertex_position->x;
        mesh.aabb.min.y = mesh.aabb.min.y < vertex_position->y ? mesh.aabb.min.y : vertex_position->y;
        mesh.aabb.min.z = mesh.aabb.min.z < vertex_position->z ? mesh.aabb.min.z : vertex_position->z;
        mesh.aabb.max.x = mesh.aabb.max.x > vertex_position->x ? mesh.aabb.max.x : vertex_position->x;
        mesh.aabb.max.y = mesh.aabb.max.y > vertex_position->y ? mesh.aabb.max.y : vertex_position->y;
        mesh.aabb.max.z = mesh.aabb.max.z > vertex_position->z ? mesh.aabb.max.z : vertex_position->z;
    }

    EdgeVertexIndices current_edge_vertex_indices, *edge_vertex_indices;
    vertex_position_indices = mesh.vertex_position_indices;
    for (u32 i = 0; i < mesh.triangle_count; i++, vertex_position_indices++) {
        for (u8 from = 0, to = 1; from < 3; from++, to = (to + 1) % 3) {
            current_edge_vertex_indices.from = vertex_position_indices->ids[from];
            current_edge_vertex_indices.to   = vertex_position_indices->ids[to];
            if (current_edge_vertex_indices.from > current_edge_vertex_indices.to) {
                u32 temp = current_edge_vertex_indices.from;
                current_edge_vertex_indices.from = current_edge_vertex_indices.to;
                current_edge_vertex_indices.to = temp;
            }

            bool found = false;
            edge_vertex_indices = mesh.edge_vertex_indices;
            for (u32 e = 0; e < mesh.edge_count; e++, edge_vertex_indices++) {
                if (edge_vertex_indices->from == current_edge_vertex_indices.from &&
                    edge_vertex_indices->to   == current_edge_vertex_indices.to) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                mesh.edge_vertex_indices[mesh.edge_count] = current_edge_vertex_indices;
                mesh.edge_count++;
            }
        }
    }

    file = fopen(mesh_file_path, "wb");

    fwrite(&mesh.aabb,           sizeof(AABB), 1, file);
    fwrite(&mesh.vertex_count,   sizeof(u32),  1, file);
    fwrite(&mesh.triangle_count, sizeof(u32),  1, file);
    fwrite(&mesh.edge_count,     sizeof(u32),  1, file);
    fwrite(&mesh.uvs_count,      sizeof(u32),  1, file);
    fwrite(&mesh.normals_count,  sizeof(u32),  1, file);
    fwrite( mesh.vertex_positions,        sizeof(vec3)                 , mesh.vertex_count,   file);
    fwrite( mesh.vertex_position_indices, sizeof(TriangleVertexIndices), mesh.triangle_count, file);
    fwrite( mesh.edge_vertex_indices,     sizeof(EdgeVertexIndices)    , mesh.edge_count,     file);
    if (mesh.uvs_count) {
        fwrite(mesh.vertex_uvs,          sizeof(vec2)                  , mesh.uvs_count,      file);
        fwrite(mesh.vertex_uvs_indices,  sizeof(TriangleVertexIndices) , mesh.triangle_count, file);
    }
    if (mesh.normals_count) {
        fwrite(mesh.vertex_normals,        sizeof(vec3)                  , mesh.normals_count,  file);
        fwrite(mesh.vertex_normal_indices, sizeof(TriangleVertexIndices) , mesh.triangle_count, file);
    }

    fclose(file);

    return 0;
}

int main(int argc, char *argv[]) {
    //return error if user does not provide exactly two arguments
    if (argc == 3)
        return obj2mesh(argv[1], argv[2]);

    printf("Exactly 2 file paths need to be provided: An '.obj' file (input) then a '.mesh' file (output)");
    return 1;
}