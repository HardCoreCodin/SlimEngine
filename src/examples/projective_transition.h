#pragma once

#include "./projective_base.h"
#include "./projective_matrix.h"

#define TRANSITION_COUNT 29

typedef struct Transition {
    bool active;
    f32 t, speed, eased_t;
} Transition;

typedef union Transitions {
    struct {
        Transition pre_projection,
        projection,
        full_projection,
        projective_lines,
        view_frustom_slice,
        lift_up,
        translate_back,
        scale_arrows,
        shear_up,
        reveal_ref_plane,
        reveal_projective_point,
        reveal_normalizing_projective_point,
        lines_through_NDC,
        focal_length_and_plane,
        aspect_ratio,
        ground_diagonal,
        view_scene,
        grid_XW,
        scale_out,
        corner_trajectory,
        corners_warping,
        stop_warping_ndc,
        drop_warping_ndc,
        show_diagonality,
        show_chosen_trajectory,
        show_focal_ratio,
        show_chosen_trajectory_labels,
        show_final_scale,
        show_NDC_corner_labels;
    };
    Transition states[TRANSITION_COUNT];
} Transitions;

Transitions transitions;

void updateTransitions(u8 key) {
    if (key == 'T') {
        if (app->controls.is_pressed.ctrl) {
            if (matrix_count) matrix_count--;
        } else {
            matrices[matrix_count].is_custom = app->controls.is_pressed.shift;
            matrices[matrix_count].M = main_matrix.M;
            updateMatrixStrings(&matrices[matrix_count]);
            matrix_count++;

            main_matrix.M = getMat4Identity();
            arrowX.body.to = app->scene.primitives[0].position = Vec3(1, 0, 0);
            arrowY.body.to = app->scene.primitives[1].position = Vec3(0, 1, 0);
            arrowZ.body.to = app->scene.primitives[2].position = Vec3(0, 0, 1);
            updateArrow(&arrowX);
            updateArrow(&arrowY);
            updateArrow(&arrowZ);
            updateMatrixStrings(&main_matrix);
        }
    } else if (key == '2') {
        transitions.pre_projection.active = true;
        transitions.pre_projection.t = 0;
        transitions.view_frustom_slice.active = !transitions.view_frustom_slice.active;
        transitions.view_frustom_slice.t = 0;
    } else if (key == '1') {
        transitions.full_projection.active = !transitions.full_projection.active;
        transitions.full_projection.t = 0;
    } else if (key == '4') {
        //            transitions.translate_back.active = true;
        //            transitions.translate_back.t = 0;
        transitions.reveal_projective_point.active = true;
        transitions.reveal_projective_point.t = 0;
        move_projective_point = !move_projective_point;
        draw_locator_grids = !draw_locator_grids;
    } else if (key == '3') {
        transitions.projective_lines.active = !transitions.projective_lines.active;
        transitions.projective_lines.t = 0;
        show_pre_projection = !show_pre_projection;
    } else if (key == '5') {
        //            transitions.scale_back.active = true;
        //            transitions.scale_back.t = 0;
        transitions.reveal_ref_plane.active = true;
        if (transitions.reveal_projective_point.active && move_projective_point) {
            transitions.reveal_normalizing_projective_point.active = true;
            transitions.reveal_normalizing_projective_point.t = 0;
        } else
            transitions.reveal_ref_plane.t = 0;
    } else if (key == '6') {
        transitions.reveal_projective_point.active = false;
        transitions.projective_lines.active = false;
        transitions.lift_up.active = true;
        transitions.lift_up.t = 0;
        //            transitions.shear_up.active = true;
        //            transitions.shear_up.t = 0;
    } else if (key == '7') {
        transitions.lines_through_NDC.active = !transitions.lines_through_NDC.active;
        transitions.lines_through_NDC.t = 0;
    } else if (key == '8') {
        transitions.corner_trajectory.active = !transitions.corner_trajectory.active;
        transitions.corner_trajectory.t = 0;
    } else if (key == '9') {
        transitions.corners_warping.active = !transitions.corners_warping.active;
        transitions.corners_warping.t = 0;
    } else if (key == '0') {
        transitions.corner_trajectory.active = false;
        transitions.stop_warping_ndc.active = true;
        transitions.stop_warping_ndc.t = 0;
    } else if (key == 'P') {
        transitions.stop_warping_ndc.active = false;
        transitions.drop_warping_ndc.active = true;
        transitions.drop_warping_ndc.t = 0;
    } else if (key == 'Z') {
        if (app->controls.is_pressed.ctrl) {
            arrowX.body.to = arrowX_box_prim->position = Vec3(1, 0, 0);
            arrowY.body.to = arrowY_box_prim->position = Vec3(0, 1, 0);
            arrowZ.body.to = arrowZ_box_prim->position = Vec3(0, 0, 1);
        } else {
            transitions.focal_length_and_plane.active = !transitions.focal_length_and_plane.active;
            transitions.focal_length_and_plane.t = 0;
        }
    } else if (key == 'A') {
        transitions.aspect_ratio.active = !transitions.aspect_ratio.active;
        transitions.aspect_ratio.t = 0;
    } else if (key == 'X') {
        transitions.ground_diagonal.active = !transitions.ground_diagonal.active;
        transitions.ground_diagonal.t = 0;
    } else if (key == 'V') {
        transitions.view_scene.active = !transitions.view_scene.active;
        transitions.view_scene.t = 0;
    } else if (key == 'G') {
        transitions.grid_XW.active = !transitions.grid_XW.active;
        transitions.grid_XW.t = 0;
    } else if (key == 'S') {
        transitions.scale_out.active = !transitions.scale_out.active;
        transitions.scale_out.t = 0;
    } else if (key == 'D') {
        transitions.show_diagonality.active = !transitions.show_diagonality.active;
        transitions.show_diagonality.t = 0;
    } else if (key == 'C') {
        transitions.show_chosen_trajectory.active = !transitions.show_chosen_trajectory.active;
        transitions.show_chosen_trajectory.t = 0;
    } else if (key == 'F') {
        transitions.show_focal_ratio.active = !transitions.show_focal_ratio.active;
        transitions.show_focal_ratio.t = 0;
    } else if (key == 'B') {
        transitions.show_chosen_trajectory_labels.active = !transitions.show_chosen_trajectory_labels.active;
        transitions.show_chosen_trajectory_labels.t = 0;
    } else if (key == 'H') {
        transitions.show_final_scale.active = !transitions.show_final_scale.active;
        transitions.show_final_scale.t = 0;
    } else if (key == 'M') {
        show_final_matrix = !show_final_matrix;
    } else if (key == 'L') {
        transitions.show_NDC_corner_labels.active = !transitions.show_NDC_corner_labels.active;
        transitions.show_NDC_corner_labels.t = 0;
    } else if (key == 'O') {
        secondary_viewport.settings.use_cube_NDC = !secondary_viewport.settings.use_cube_NDC;
        setPreProjectionMatrix(&secondary_viewport);
        initBox(&NDC_box);
        if (!secondary_viewport.settings.use_cube_NDC) for (u8 i = 4; i < 8; i++) NDC_box.vertices.buffer[i].z = 0;
        setBoxEdgesFromVertices(&NDC_box.edges, &NDC_box.vertices);
    } else if (key == 'K') {
        collapse_final_matrix = !collapse_final_matrix;
    } else if (key == 'J') {
        secondary_viewport.settings.flip_z = !secondary_viewport.settings.flip_z;
    }
}

bool incTransition(Transition *transition, f32 inc, bool reset_when_done) {
    transition->t += inc * transition->speed;
    if (transition->t > 1) {
        if (reset_when_done) {
            transition->t = 0;
            transition->active = false;
        } else transition->t = transition->eased_t = 1;
    } else
        transition->eased_t = smoothstep(0, 1, transition->t);

    return transition->active;
}

void transitionBox(Transition *transition, Box *from_box, Box *to_box) {
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++)
        transforming_view_frustum_box.vertices.buffer[i] = lerpVec3(from_box->vertices.buffer[i], to_box->vertices.buffer[i], transition->eased_t);

    setBoxEdgesFromVertices(&transforming_view_frustum_box.edges, &transforming_view_frustum_box.vertices);
}

void transformedPosition(vec3 *pos, mat4 M) {
    vec4 final_pos, pos4 = Vec4fromVec3(*pos, 1.0f);
    if (transitions.full_projection.active) {
        final_pos = mulVec4Mat4(pos4, secondary_viewport.pre_projection_matrix);
        final_pos = scaleVec4(final_pos, 1.0f / final_pos.w);
        final_pos.z = -final_pos.z;
    }

    if (transitions.view_frustom_slice.active) {
        vec3 target_pos = vec3wUp(mulVec4Mat4(pos4, M));
        *pos = transitions.full_projection.active ? lerpVec3(target_pos, vec3wUp(final_pos), transitions.full_projection.eased_t) : target_pos;
        if (!transitions.lift_up.active || transitions.lift_up.t < 1)
            pos->y = transitions.lift_up.eased_t;
    } else {
        mulVec3Mat4(*pos, 1.0f, M, pos);
        if (transitions.full_projection.active)
            *pos = lerpVec3(*pos, Vec3fromVec4(final_pos), transitions.full_projection.eased_t);
    }
}

Edge* transformedEdge(Edge *edge, mat4 M) {
    transformedPosition(&edge->from, M);
    transformedPosition(&edge->to, M);
    return edge;
}

void transformBoxVertices(Box *box, mat4 matrix, BoxVertices *transformed_vertices) {
    for (u8 i = 0; i < BOX__VERTEX_COUNT; i++) {
        transformed_vertices->buffer[i] = box->vertices.buffer[i];
        transformedPosition(transformed_vertices->buffer + i, matrix);
    }
}