#pragma once

#include "../core/base.h"
#include "../core/types.h"
#include "../shapes/edge.h"
#include "./primitive.h"

void drawMesh(Mesh *mesh, bool draw_normals, Primitive *primitive, vec3 color, f32 opacity, u8 line_width, Viewport *viewport) {
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

        drawEdge(&edge, color, opacity, line_width, viewport);
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

                drawEdge(&edge, Color(Red), opacity * 0.5f, line_width, viewport);
            }
        }
    }
}