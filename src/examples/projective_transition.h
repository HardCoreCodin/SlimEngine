#pragma once

#include "./projective_base.h"
#include "./projective_matrix.h"

#define TRANSITION_COUNT 19

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
        scale_back,
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
        scale_out;
    };
    Transition states[TRANSITION_COUNT];
} Transitions;

Transitions transitions;

void updateTransitions(u8 key) {
    if (key == 'T') {
        if (app->controls.is_pressed.ctrl) {
            if (matrix_count) matrix_count--;
        } else {
            if (transitions.view_frustom_slice.active) {

            } else {
                matrices[matrix_count].M = main_matrix.M;
                updateMatrixStrings(&matrices[matrix_count]);
                matrix_count++;
            }

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
        transitions.view_frustom_slice.active = true;
        transitions.view_frustom_slice.t = 0;
    } else if (key == '1') {
        transitions.full_projection.active = true;
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
        transitions.lines_through_NDC.active = true;
        transitions.lines_through_NDC.t = 0;
    } else if (key == '8') {
        //            alpha = !alpha;
    } else if (key == 'Z') {
        transitions.focal_length_and_plane.active = !transitions.focal_length_and_plane.active;
        transitions.focal_length_and_plane.t = 0;
    } else if (key == 'C') {
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
    } else if (key == 'B') {
        transitions.scale_out.active = !transitions.scale_out.active;
        transitions.scale_out.t = 0;
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

Edge* transformedEdge(Edge *edge, mat4 M) {
    if (transitions.view_frustom_slice.active) {
        edge->from = vec3wUp(mulVec4Mat4(Vec4fromVec3(edge->from, 1.0f), M));
        edge->to   = vec3wUp(mulVec4Mat4(Vec4fromVec3(edge->to,   1.0f), M));
        if (!transitions.translate_back.active) edge->from.y = edge->to.y = transitions.lift_up.eased_t;
    } else {
        mulVec3Mat4(edge->from, 1.0f, M, &edge->from);
        mulVec3Mat4(edge->to,   1.0f, M, &edge->to);
    }

    return edge;
}