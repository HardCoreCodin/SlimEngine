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
#include "./SlimEngine/math/vec3.h"
#include "./SlimEngine/math/mat3.h"

enum VertexAttributes {
    VertexAttributes_None,
    VertexAttributes_Positions,
    VertexAttributes_PositionsAndUVs,
    VertexAttributes_PositionsUVsAndNormals
};

int obj2mesh(char* obj_file_path, char* mesh_file_path, bool invert_winding_order, float scale, float rotY) {
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
        if (strncmp(line, (char*)"vn ", 2) == 0) mesh.normals_count++;
        if (strncmp(line, (char*)"vt ", 2) == 0) mesh.uvs_count++;
        if (strncmp(line, (char*)"v ", 2) == 0) mesh.vertex_count++;
        if (strncmp(line, (char*)"f ", 2) == 0) {
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

    u8 v1_id = 0;
    u8 v2_id = invert_winding_order ? 2 : 1;
    u8 v3_id = invert_winding_order ? 1 : 2;

    file = fopen(obj_file_path, (char*)"r");
    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, (char*)"v ", 2) == 0) {
            sscanf(line, (char*)"v %f %f %f", &vertex_position->x, &vertex_position->y, &vertex_position->z);
            vertex_position++;
        } else if (strncmp(line, (char*)"vn ", 2) == 0) {
            sscanf(line, (char*)"vn %f %f %f", &vertex_normal->x, &vertex_normal->y, &vertex_normal->z);
            vertex_normal++;
        } else if (strncmp(line, (char*)"vt ", 2) == 0) {
            sscanf(line, (char*)"vt %f %f", &vertex_uvs->x, &vertex_uvs->y);
            vertex_uvs++;
        } else if (strncmp(line, (char*)"f ", 2) == 0) {
            int vertex_indices[3];
            int uvs_indices[3];
            int normal_indices[3];

            switch (vertex_attributes) {
                case VertexAttributes_Positions:
                    sscanf(
                            line, (char*)"f %d %d %d",
                            &vertex_indices[v1_id],
                            &vertex_indices[v2_id],
                            &vertex_indices[v3_id]
                    );
                    break;
                case VertexAttributes_PositionsAndUVs:
                    sscanf(
                            line, (char*)"f %d/%d %d/%d %d/%d",
                            &vertex_indices[v1_id], &uvs_indices[v1_id],
                            &vertex_indices[v2_id], &uvs_indices[v2_id],
                            &vertex_indices[v3_id], &uvs_indices[v3_id]
                    );
                    vertex_uvs_indices->ids[0] = uvs_indices[0] - 1;
                    vertex_uvs_indices->ids[1] = uvs_indices[1] - 1;
                    vertex_uvs_indices->ids[2] = uvs_indices[2] - 1;
                    vertex_uvs_indices++;
                    break;
                case VertexAttributes_PositionsUVsAndNormals:
                    sscanf(
                            line, (char*)"f %d/%d/%d %d/%d/%d %d/%d/%d",
                            &vertex_indices[v1_id], &uvs_indices[v1_id], &normal_indices[v1_id],
                            &vertex_indices[v2_id], &uvs_indices[v2_id], &normal_indices[v2_id],
                            &vertex_indices[v3_id], &uvs_indices[v3_id], &normal_indices[v3_id]
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

    mat3 rot = getMat3Identity();
    if (rotY) {
        rotY *= DEG_TO_RAD;
        f32 c = cosf(rotY);
        f32 s = sinf(rotY);
        rot.X.x = rot.Z.z = c;
        rot.X.z = +s;
        rot.Z.x = -s;
        vertex_normal = mesh.vertex_normals;
        for (u32 i = 0; i < mesh.normals_count; i++, vertex_normal++)
            *vertex_normal = mulVec3Mat3(*vertex_normal, rot);
    }

    vertex_position = mesh.vertex_positions;
    for (u32 i = 0; i < mesh.vertex_count; i++, vertex_position++) {
        if (rotY) *vertex_position = mulVec3Mat3(*vertex_position, rot);
        mesh.aabb.min.x = mesh.aabb.min.x < vertex_position->x ? mesh.aabb.min.x : vertex_position->x;
        mesh.aabb.min.y = mesh.aabb.min.y < vertex_position->y ? mesh.aabb.min.y : vertex_position->y;
        mesh.aabb.min.z = mesh.aabb.min.z < vertex_position->z ? mesh.aabb.min.z : vertex_position->z;
        mesh.aabb.max.x = mesh.aabb.max.x > vertex_position->x ? mesh.aabb.max.x : vertex_position->x;
        mesh.aabb.max.y = mesh.aabb.max.y > vertex_position->y ? mesh.aabb.max.y : vertex_position->y;
        mesh.aabb.max.z = mesh.aabb.max.z > vertex_position->z ? mesh.aabb.max.z : vertex_position->z;
    }

    vec3 centroid = scaleVec3(addVec3(mesh.aabb.min, mesh.aabb.max), 0.5f);
    if (nonZeroVec3(centroid)) {
        mesh.aabb.min = subVec3(mesh.aabb.min, centroid);
        mesh.aabb.max = subVec3(mesh.aabb.max, centroid);
        vertex_position = mesh.vertex_positions;
        for (u32 i = 0; i < mesh.vertex_count; i++, vertex_position++)
            *vertex_position = subVec3(*vertex_position, centroid);
    }

    vertex_position = mesh.vertex_positions;
    for (u32 i = 0; i < mesh.vertex_count; i++, vertex_position++)
        *vertex_position = scaleVec3(*vertex_position, scale);
    mesh.aabb.min = scaleVec3(mesh.aabb.min, scale);
    mesh.aabb.max = scaleVec3(mesh.aabb.max, scale);

    file = fopen(mesh_file_path, (char*)"wb");

    fwrite(&mesh.vertex_count,   sizeof(u32),  1, file);
    fwrite(&mesh.triangle_count, sizeof(u32),  1, file);
    fwrite(&mesh.edge_count,     sizeof(u32),  1, file);
    fwrite(&mesh.uvs_count,      sizeof(u32),  1, file);
    fwrite(&mesh.normals_count,  sizeof(u32),  1, file);
    fwrite(&mesh.aabb.min,       sizeof(vec3), 1, file);
    fwrite(&mesh.aabb.max,       sizeof(vec3), 1, file);
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
    if (argc == 2 && !strcmp(argv[1], (char*)"--help")) {
        printf((char*)("Exactly 2 file paths need to be provided: "
                       "An '.obj' file (input) then a '.mesh' file (output), "
                       "an optional flag '-invert_winding_order' for inverting winding order"
                       "an optional flag 'scale:<float>' for scaling the mesh,"
                       "an optional flag 'rotY:<float> for rotating the mesh around Y"
        ));
        return 0;
    } else if (argc == 3 || // 2 arguments
               argc == 4 || // 3 arguments
               argc == 5 || // 4 arguments
               argc == 6    // 5 arguments
            ) {
        char *obj_file_path = argv[1];
        char *mesh_file_path = argv[2];
        if (argc == 3) return obj2mesh(obj_file_path, mesh_file_path, false, 1, 0);

        bool invert_winding_order = false;
        float scale = 1;
        float rotY = 0;
        for (u32 i = 3; i < (u32)argc; i++) {
            char *arg = argv[i];
            if (strcmp(arg, (char *) "-invert_winding_order") == 0)
                invert_winding_order = true;
            else {
                char *scale_arg_prefix = (char *) "scale:";
                bool is_scale_arg = true;
                for (u32 c = 0; c < 6; c++)
                    if (arg[c] != scale_arg_prefix[c]) {
                        is_scale_arg = false;
                        break;
                    }
                if (is_scale_arg) scale = (f32)atof(arg + 6);
                else {
                    char *rotY_arg_prefix = (char *) "rotY:";
                    bool is_rotY_arg = true;
                    for (u32 c = 0; c < 5; c++)
                        if (arg[c] != rotY_arg_prefix[c]) {
                            is_rotY_arg = false;
                            break;
                        }
                    if (is_rotY_arg) rotY = (f32)atof(arg + 5);
                }
            }
        }
        return obj2mesh(obj_file_path, mesh_file_path, invert_winding_order, scale, rotY);
    }

    printf((char*)("Exactly 2 file paths need to be provided: "
                   "An '.obj' file (input) then a '.mesh' file (output), "
                   "and an optional flag '-invert_winding_order' for inverting winding order"));
    return 1;
}