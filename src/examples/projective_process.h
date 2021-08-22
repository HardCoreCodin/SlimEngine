#pragma once

#include "./projective_base.h"
#include "./projective_arrow.h"
#include "./projective_space.h"
#include "./projective_matrix.h"
#include "./projective_locator.h"
#include "./projective_transition.h"

Quad3 warping_ndc;

void renderViewSpaceFrustumSlice(Viewport *viewport, f32 delta_time) {
    static float elapsed = 0;
    elapsed += delta_time;
    arrow1.head.length = arrowX.head.length = arrowZ.head.length = arrowY.head.length = transitions.view_frustom_slice.active ? 0.5f : 0.25f;

    transitions.scale_arrows.active = true;
    incTransition(&transitions.scale_arrows, delta_time, true);
    if (transitions.scale_arrows.eased_t == 0) transitions.scale_arrows.eased_t = 0.001f;

    f32 L = secondary_viewport.camera->focal_length;
    f32 A = secondary_viewport.frame_buffer->dimensions.width_over_height;
    f32 N = secondary_viewport.settings.near_clipping_plane_distance;
    f32 F = secondary_viewport.settings.far_clipping_plane_distance;

    rotatePrimitive(main_box_prim, delta_time / -3, 0, 0);

    if (transitions.view_frustom_slice.active && matrix_count) {
        Matrix *matrix = matrices;

        initMatrix(matrix);
        matrix->M.X.x = L / A;
        matrix->M.Y.y = L;
        copyToString(&matrix->components[0][0].string, "L/A", 0);
        copyToString(&matrix->components[1][1].string, "L", 0);
        matrix->component_colors[0][0] = Color(White);;
        matrix->component_colors[1][1] = Color(White);;

        if (matrix_count > 1) {
            matrix++;

            initMatrix(matrix);
            matrix->M.Z.w = 1;
            matrix->M.W.w = 0;
            matrix->M.W.z = -N;
            copyToString(&matrix->components[2][3].string, "1", 0);
            copyToString(&matrix->components[3][3].string, "0", 0);
            copyToString(&matrix->components[3][2].string, "-N", 0);
            matrix->component_colors[3][2] = Color(White);;

            if (matrix_count == 3) {
                matrix++;

                initMatrix(matrix);
                matrix->M.Z.z = F / (F - N);
                copyToString(&matrix->components[2][2].string, "F/(F-N)", 0);
                matrix->component_colors[2][2] = Color(White);;
            }
        }
    }
    mat4 M = getMat4Identity();
    for (u8 i = 0; i < matrix_count; i++) M = mulMat4(M, matrices[i].M);
    M = mulMat4(M, main_matrix.M);

    Quad3 NDC_quad, view_frustum_quad, transforming_quad;
    setQuad3FromBox(&view_frustum_quad, &view_frustum_box);
    setQuad3FromBox(&NDC_quad, &NDC_box);

    if (transitions.lift_up.active) {
        incTransition(&transitions.lift_up, delta_time, false);
        for (u8 i = 0; i < 4; i++) NDC_quad.corners[i].y = transitions.lift_up.eased_t;
    }

    transforming_quad = view_frustum_quad;

    Edge projective_point_edge, edge;
    RGBA projective_point_color;
    focal_length_color.A = 0;
    aspect_ratio_color.A = 0;
    projective_point_color.G = MAX_COLOR_VALUE / 2;
    projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G);
    projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G);

    vec3 corner = Vec3(secondary_viewport.frame_buffer->dimensions.width_over_height, 1, L);

    RGBA up_color = Y_color;
    if (transitions.view_frustom_slice.active) {
        if (incTransition(&transitions.view_frustom_slice, delta_time, false))
            up_color = getColorInBetween(Y_color, W_color, transitions.view_frustom_slice.eased_t);

        for (u8 i = 0; i < 4; i++) transformedPosition(transforming_quad.corners + i, M);
    }
    if (transitions.view_frustom_slice.eased_t < 1) {
        sides_color.A = near_color.A = far_color.A = NDC_color.A = (u8)((f32)MAX_COLOR_VALUE * (1.0f - transitions.view_frustom_slice.eased_t));
        if (!transitions.focal_length_and_plane.active)
            drawBox(viewport, NDC_color, &NDC_box, secondary_camera_prim, BOX__ALL_SIDES, 1);

        transformBoxVertices(&view_frustum_box, M, &transforming_view_frustum_box.vertices);
        drawFrustum(viewport, &transforming_view_frustum_box, near_color, far_color, sides_color, 1);
    }

    if (transitions.projective_lines.active) {
        if (incTransition(&transitions.projective_lines, delta_time, false))
            drawProjectiveSpace(viewport, &transitions.projective_lines, !transitions.reveal_projective_point.active);

        if (transitions.reveal_projective_point.active) {
            if (incTransition(&transitions.reveal_projective_point, delta_time, false))
                projective_point_color.A = X_color.A = W_color.A = Z_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.reveal_projective_point.eased_t);

            Locator projective_point;
            vec3 location;
            if (move_projective_point) {
                location.x = 2 * sinf(elapsed)      - 1;
                location.z = 2 * sinf(elapsed+1) - 1;
                location.y = 1;
                projective_point_edge.to = normVec3(location);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);
                projective_point_edge.to = scaleVec3(location, 5);
            } else {
                location = getVec3Of(5 * sinf(elapsed));
                location.z = -location.z;
                projective_point_edge.to = getVec3Of(10);
                projective_point_edge.to.z = -projective_point_edge.to.z;
            }
            if (transitions.reveal_normalizing_projective_point.active) {
                if (incTransition(&transitions.reveal_normalizing_projective_point, delta_time, false))
                    location = lerpVec3(Vec3(-4, 2, -8), Vec3(-2, 1, -4), transitions.reveal_normalizing_projective_point.eased_t);
                if (transitions.reveal_normalizing_projective_point.t >= 1)
                    transitions.reveal_normalizing_projective_point.t = 0;

                projective_point_edge.to = scaleVec3(normVec3(location), 5);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);
                transitions.reveal_normalizing_projective_point.active = true;
                arrow1.body.from = getVec3Of(0);
                arrow1.body.to   = location;
                viewport->settings.depth_sort = false;
                if (updateArrow(&arrow1)) drawArrow(viewport, projective_point_color, &arrow1, 2);
                viewport->settings.depth_sort = true;
            }

            setLocator(&projective_point, location);
            Edge *edge_ptr = projective_point.edges;
            for (u8 i = 0; i < 3; i++, edge_ptr++) {
                edge_ptr->to   = convertPositionToObjectSpace(edge_ptr->to,   main_camera_prim);
                edge_ptr->from = convertPositionToObjectSpace(edge_ptr->from, main_camera_prim);
                drawEdge(viewport, projective_point_color, edge_ptr, 3);
            }

            projective_point_color.A /= 2;
            projective_point_edge.from = invertedVec3(projective_point_edge.to);
            projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
            projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);
            drawEdge(viewport, projective_point_color, &projective_point_edge, 2);

            viewport->settings.depth_sort = false;
            drawCoordinateArrowsToPoint(viewport, X_color, up_color, Z_color, location, 2);
            viewport->settings.depth_sort = true;

            X_color.A = up_color.A = Z_color.A = MAX_COLOR_VALUE;
        }
    }

    if (transitions.reveal_ref_plane.active) {
        RGBA grid_color = Color(projective_ref_plane_prim->color);
        if (incTransition(&transitions.reveal_ref_plane, delta_time, false))
            grid_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.reveal_ref_plane.eased_t);

        grid_color.A /= 4;
        drawGrid(viewport, grid_color, projective_ref_plane_grid, projective_ref_plane_prim,0);
    }

    if (transitions.full_projection.active) {
        if (incTransition(&transitions.full_projection, delta_time, false)) {
            sides_color = getColorInBetween(sides_color, Color(Yellow), transitions.full_projection.eased_t);
            near_color  = getColorInBetween(near_color,  Color(Yellow), transitions.full_projection.eased_t);
            far_color   = getColorInBetween(far_color,   Color(Yellow), transitions.full_projection.eased_t);
        }
    }

    if (transitions.lines_through_NDC.active) {
        viewport->settings.depth_sort = true;
        if (transitions.corner_trajectory.active) incTransition(&transitions.corner_trajectory, delta_time, false);

        if (incTransition(&transitions.lines_through_NDC, delta_time, false)) {
            projective_point_edge.from = getVec3Of(0);
            projective_point_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.lines_through_NDC.eased_t);
            projective_point_color.G = MAX_COLOR_VALUE / 2;

            for (u8 i = 0; i < 4; i++) {
                projective_point_edge.to = normVec3(NDC_quad.corners[i]);
                projective_point_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.x);
                projective_point_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)projective_point_color.G * projective_point_edge.to.z);

                if (transitions.corner_trajectory.active) {
                    f32 f = 4 + 2 * sinf(elapsed + (f32)i);
                    arrow1.body.from = transforming_quad.corners[i];
                    arrow1.body.to   = warping_ndc.corners[i] = lerpVec3(arrow1.body.from, scaleVec3(projective_point_edge.to, f), transitions.corner_trajectory.eased_t);

                    if (updateArrow(&arrow1)) drawArrow(viewport, projective_point_color, &arrow1, 2);
                }

                projective_point_edge.to = scaleVec3(projective_point_edge.to, 40);
                projective_point_edge.from = invertedVec3(projective_point_edge.to);

                projective_point_edge.from = convertPositionToObjectSpace(projective_point_edge.from, main_camera_prim);
                projective_point_edge.to   = convertPositionToObjectSpace(projective_point_edge.to, main_camera_prim);

                edge_color = projective_point_color;
                edge_color.A /= 2;

                drawEdge(viewport, edge_color, &projective_point_edge, 2);
            }
            edge_color = NDC_color;
            if (transitions.corners_warping.active) {
                incTransition(&transitions.corners_warping, delta_time, false);
                edge_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.corners_warping.eased_t);
            }
            if ((transitions.corners_warping.active ||
                 transitions.stop_warping_ndc.active) &&
                !transitions.drop_warping_ndc.active) {
                for (u8 i = 0; i < 4; i++) {
                    edge.from = warping_ndc.corners[i];
                    edge.to   = warping_ndc.corners[(i + 1) % 4];
                    edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
                    edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
                    drawEdge(viewport, edge_color, &edge, 2);
                }
            }

            if (transitions.drop_warping_ndc.active) {
                incTransition(&transitions.drop_warping_ndc, delta_time, false);

                for (u8 i = 0; i < 4; i++) {
                    edge.from = warping_ndc.corners[i];
                    edge.to   = warping_ndc.corners[(i + 1) % 4];
                    edge.from = lerpVec3(edge.from,     NDC_quad.corners[i],          transitions.drop_warping_ndc.eased_t);
                    edge.to   = lerpVec3(edge.to,   NDC_quad.corners[(i + 1) % 4], transitions.drop_warping_ndc.eased_t);
                    edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
                    edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
                    drawEdge(viewport, NDC_color, &edge, 2);
                }
            }

            if (transitions.show_diagonality.active) {
                if (incTransition(&transitions.show_diagonality, delta_time, false)) {
                    f32 f = 4 + 2 * sinf(elapsed);
                    edge.from.z = edge.to.z = edge.from.y = 0;
                    edge.from.x = edge.to.x = f;
                    edge.to.y = f;

                    arrow1.body.from = edge.from;
                    arrow1.body.to = edge.to;
                    if (updateArrow(&arrow1)) drawArrow(viewport, near_color, &arrow1, 2);

                    arrow1.body.from = edge.from;
                    arrow1.body.to = getVec3Of(0);
                    if (updateArrow(&arrow1)) drawArrow(viewport, near_color, &arrow1, 2);

                    f = 4 + 2 * sinf(elapsed + 1);
                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.x = 0;
                    if (updateArrow(&arrow1)) drawArrow(viewport, far_color, &arrow1, 2);

                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.y = 0;
                    if (updateArrow(&arrow1)) drawArrow(viewport, far_color, &arrow1, 2);

                    arrow1.body.from = getVec3Of(f);
                    arrow1.body.to = arrow1.body.from;
                    arrow1.body.to.z = 0;
                    if (updateArrow(&arrow1)) drawArrow(viewport, far_color, &arrow1, 2);
                }
            }

        }
    }

    if (transitions.show_chosen_trajectory.active && incTransition(&transitions.show_chosen_trajectory, delta_time, false)) {
        near_color = default_near_color;
        far_color  = default_far_color;
        if (transitions.show_chosen_trajectory_labels.active) {
            if (incTransition(&transitions.show_chosen_trajectory_labels, delta_time, false))
                near_color.A = far_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.show_chosen_trajectory_labels.eased_t);
        }

        edge.to = normVec3(NDC_quad.top_right);
        edge_color.G = MAX_COLOR_VALUE / 2;
        edge_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)edge_color.G * edge.to.x);
        edge_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)edge_color.G * edge.to.z);
        edge_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.show_chosen_trajectory.eased_t);

        arrow1.body.from = Vec3(N, 0, N);;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.x;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, "N", (i32)edge.to.x, (i32)edge.to.y);
        }
        if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 1);


        arrow1.body.from = Vec3(N, 0, N);;
        arrow1.body.from.y = arrow1.body.from.x;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 1);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, "N", (i32)edge.to.x, (i32)edge.to.y);

            arrow1.body.from = Vec3(N, 0, N);
            arrow1.body.from.y = 0;
            arrow1.body.to = arrow1.body.from;
            arrow1.body.to.z = 0;
            edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
            arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);

            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, "N", (i32)edge.to.x, (i32)edge.to.y);

            drawLocalEdge(arrow1.body, near_color, 1);
        }

        arrow1.body.from = Vec3(N, 0, N);;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.z;
        arrow1.body.to.z = 0;
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.scale_arrows.eased_t);
        if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 1);

        edge.to = normVec3(NDC_quad.bottom_right);
        edge_color.R = MAX_COLOR_VALUE / 4 + (u8)((f32)edge_color.G * edge.to.x);
        edge_color.B = MAX_COLOR_VALUE / 4 + (u8)((f32)edge_color.G * edge.to.z);
        arrow1.body.from = Vec3(F, 0, F);;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.y = arrow1.body.to.x;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.scale_arrows.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(far_color, "F", (i32)edge.to.x, (i32)edge.to.y);
        }
        if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 1);


        arrow1.body.from = Vec3(F, 0, F);;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(far_color, "F", (i32)edge.to.x, (i32)edge.to.y);
            drawLocalEdge(arrow1.body, far_color, 1);
        }

        arrow1.body.from = Vec3(N, 0, N);;
        arrow1.body.to = arrow1.body.from;
        arrow1.body.to.z = 0;
        edge.from = lerpVec3(arrow1.body.from, arrow1.body.to, 0.5f);
        arrow1.body.to = lerpVec3(arrow1.body.from, arrow1.body.to, transitions.show_chosen_trajectory.eased_t);
        if (transitions.show_chosen_trajectory_labels.active) {
            edge.from.x += 0.05f;
            edge.to = edge.from;
            edge.to.x += 0.25f;
            edge.to.y += 0.15f;

            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(near_color, "N", (i32)edge.to.x, (i32)edge.to.y);
        }
    }

    if (transitions.focal_length_and_plane.active) {
        focal_length_color = default_focal_length_color;
        if (incTransition(&transitions.focal_length_and_plane, delta_time, false))
            focal_length_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.focal_length_and_plane.eased_t);

        // Touching diagonals:
        edge.from = getVec3Of(0);
        edge.to = scaleVec3(Vec3(0, 1, L), 30);
        edge.to = lerpVec3(edge.from, edge.to, transitions.focal_length_and_plane.eased_t);
        drawLocalEdge(edge, focal_length_color, 1);

        edge.to = scaleVec3(Vec3(A, 0, L), 30);
        edge.to = lerpVec3(edge.from, edge.to, transitions.focal_length_and_plane.eased_t);
        drawLocalEdge(edge, focal_length_color, 1);

        // Labels
        edge.from = Vec3(0.05f, 0.5f, L);
        edge.to   = Vec3(0.35f, 0.6f, L);
        drawLocalEdge(edge, Color(BrightGrey), 1);

        edge.to = lerpVec3(edge.from, edge.to, 1.15f);

        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);

        addLabel(Y_color, "1", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(0.05f, 0, L*0.75f);
        edge.to   = Vec3(0.35f, 0, L*0.75f + 0.25f);
        drawLocalEdge(edge, Color(BrightGrey), 1);

        edge.to = lerpVec3(edge.from, edge.to, 1.15f);

        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(focal_length_color, "L", (i32)edge.to.x, (i32)edge.to.y);

        if (transitions.show_focal_ratio.active) {
            edge.from = Vec3(0, 0, L/2);
            edge.to   = Vec3(0, 0.5f, L);

            arrow1.body = edge;
            if (updateArrow(&arrow1)) drawArrow(viewport, Color(BrightGrey), &arrow1, 1);

            arrow1.body.from = edge.to;
            arrow1.body.to = edge.from;
            if (updateArrow(&arrow1)) drawArrow(viewport, Color(BrightGrey), &arrow1, 1);

            vec3 offset_dir = Vec3(0, edge.to.z - edge.from.z, edge.from.y - edge.to.y);

            edge.from = scaleAddVec3(offset_dir, 0.05f, lerpVec3(edge.from, edge.to, 0.5f));
            edge.to   = scaleAddVec3(offset_dir, 0.5f, edge.from);
            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to   = scaleAddVec3(offset_dir, 0.15f, edge.to);

            edge.to = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(Color(White), "  /   =", (i32)edge.to.x - 30, (i32)edge.to.y);
            addLabel(focal_length_color, "L", (i32)edge.to.x - 30, (i32)edge.to.y);
            addLabel(Y_color, "1", (i32)edge.to.x - 30 + 4 * FONT_WIDTH, (i32)edge.to.y);
            addLabel(focal_length_color, "L", (i32)edge.to.x - 30 + 8 * FONT_WIDTH, (i32)edge.to.y);

            offset_dir.z = -offset_dir.z;

            edge.from = Vec3(0, 0, L/4.0f);
            edge.to   = Vec3(0, 1.0f/6.0f, L/6.0f);
            drawLocalEdge(edge, Color(BrightBlue), 1);

            edge.from.y = 0.1f;
            edge.from.z = 0.3f;
            edge.to = scaleAddVec3(offset_dir, 0.5f, edge.from);
            drawLocalEdge(edge, Color(BrightGrey), 1);

            edge.to   = scaleAddVec3(offset_dir, 0.05f, edge.to);

            edge.to = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(Color(White), "cot(     )", (i32)edge.to.x - 60, (i32)edge.to.y);
            addLabel(Color(BrightBlue), "angle", (i32)edge.to.x - 60 + 4 * FONT_WIDTH, (i32)edge.to.y);
        }

//        viewport->settings.depth_sort = false;
        Edge focal_length_edge;
        focal_length_edge.from = focal_length_edge.to = getVec3Of(0);
        focal_length_edge.to.z = L;
        focal_length_edge.from = convertPositionToObjectSpace(focal_length_edge.from, main_camera_prim);
        focal_length_edge.to   = convertPositionToObjectSpace(focal_length_edge.to,   main_camera_prim);
        drawEdge(viewport, focal_length_color, &focal_length_edge, 3);

        Quad3 ref_quad;
        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = corner;

        ref_quad.top_left.x    = ref_quad.bottom_left.x  = -ref_quad.top_left.x;
        ref_quad.bottom_left.y = ref_quad.bottom_right.y = -ref_quad.top_left.y;

        for (u8 i = 0; i < 4; i++)
            ref_quad.corners[i] = convertPositionToObjectSpace(ref_quad.corners[i], main_camera_prim);

        edge_color = Color(Grey);
        edge_color.A = focal_length_color.A;
        for (u8 i = 0; i < 4; i++) {
            edge.from = ref_quad.corners[i];
            edge.to   = ref_quad.corners[(i + 1) % 4];
            drawEdge(viewport, edge_color, &edge, 2);
        }

        edge.from = edge.to = corner;
        edge.to.y = -1;
        edge.from.x = edge.to.x = 1;
        drawLocalEdge(edge, edge_color, 0);
        edge.from.x = edge.to.x = -1;
        drawLocalEdge(edge, edge_color, 0);

        edge_color = default_aspect_ratio_color;
        edge_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = 0;
        edge.from.y = edge.to.y = 0;
        drawLocalEdge(edge, edge_color, 2);

        edge_color = Y_color;
        edge_color.A = focal_length_color.A;
        edge.from = edge.to = corner;
        edge.from.x = edge.to.x = 0;
        edge.from.y = 0;
        drawLocalEdge(edge, edge_color, 2);

        if (transitions.aspect_ratio.active) {
            aspect_ratio_color = default_aspect_ratio_color;

            if (incTransition(&transitions.aspect_ratio, delta_time, false))
                aspect_ratio_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.aspect_ratio.eased_t);

            // Diagonal:
            edge.from = edge.to = corner;
            edge.from.x = -edge.to.x;
            edge.from.y = -edge.to.y;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, Color(Grey), 1);

            // Bottom:
            edge.from = edge.to = corner;
            edge.from.y = -edge.from.y;
            edge.to = edge.from;
            edge.to.x = -edge.to.x;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, default_aspect_ratio_color, 1);

            // Side
            edge.from = edge.to = corner;
            edge.from.y = -edge.from.y;
            edge.to = lerpVec3(edge.from, edge.to, transitions.aspect_ratio.eased_t);
            drawLocalEdge(edge, Y_color, 1);

            // Labels
            edge.from = Vec3(A * 0.85f, 0.05f, L);
            edge.to   = Vec3(A * 0.85f + 0.4f, 0.25f, L);
            drawLocalEdge(edge, Color(BrightGrey), 1);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(default_aspect_ratio_color, "A", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), " / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(Y_color, "1", (i32)edge.to.x + FONT_WIDTH * 4, (i32)edge.to.y);
            addLabel(Color(White), " = ", (i32)edge.to.x + FONT_WIDTH * 5, (i32)edge.to.y);
            addLabel(default_aspect_ratio_color, "W", (i32)edge.to.x + FONT_WIDTH * 8, (i32)edge.to.y);
            addLabel(Color(White), " / ", (i32)edge.to.x + FONT_WIDTH * 9, (i32)edge.to.y);
            addLabel(Y_color, "H", (i32)edge.to.x + FONT_WIDTH * 12, (i32)edge.to.y);

            edge.from = Vec3(0.05f + A, 0.5f, L);
            edge.to   = Vec3(0.35f + A, 0.6f, L);
            drawLocalEdge(edge, Color(BrightGrey), 1);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);

            addLabel(Y_color, "1", (i32)edge.to.x, (i32)edge.to.y);
        } else {
            edge.from = Vec3(A * 0.85f, 0.05f, L);
            edge.to   = Vec3(A * 0.85f + 0.4f, 0.25f, L);
            drawLocalEdge(edge, Color(BrightGrey), 1);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(default_aspect_ratio_color, "A", (i32)edge.to.x, (i32)edge.to.y);
        }

        if (transitions.scale_out.active && incTransition(&transitions.scale_out, delta_time, true)) {
            edge.from = Vec3(0, 1, L);
            edge.to   = Vec3(0, L, L);
            edge.to   = lerpVec3(edge.from, edge.to, transitions.scale_out.eased_t);
            arrow1.body = edge;
            edge_color = Y_color;
            edge_color.A = (u8)((f32)MAX_COLOR_VALUE * 0.75f);
            if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 2);

            edge.from = edge.to;
            edge.from.x -= 0.05f;
            edge.from.y -= 0.05f;
            edge.to.y += 0.15f;
            edge.to.x -= 0.25f;
            drawLocalEdge(edge, Color(BrightGrey), 1);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(focal_length_color, "L", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), " / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(Y_color, "1", (i32)edge.to.x + FONT_WIDTH * 3, (i32)edge.to.y);


            edge.from = Vec3(A, 0, L);
            edge.to   = Vec3(L, 0, L);
            edge.to   = lerpVec3(edge.from, edge.to, transitions.scale_arrows.eased_t);
            arrow1.body = edge;
            edge_color = X_color;
            edge_color.A = (u8)((f32)MAX_COLOR_VALUE * 0.75f);
            if (updateArrow(&arrow1)) drawArrow(viewport, edge_color, &arrow1, 2);

            edge.from = edge.to;
            edge.from.x += 0.05f;
            edge.from.y += 0.05f;
            edge.to.y += 0.25f;
            edge.to.x += 0.15f;
            drawLocalEdge(edge, Color(BrightGrey), 0);

            edge.to = lerpVec3(edge.from, edge.to, 1.15f);

            edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
            edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
            projectEdge(&edge, viewport);
            addLabel(focal_length_color, "L", (i32)edge.to.x, (i32)edge.to.y);
            addLabel(Color(White), " / ", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
            addLabel(default_aspect_ratio_color, "A", (i32)edge.to.x + FONT_WIDTH * 3, (i32)edge.to.y);
        }
    }

    u8 opacity = MAX_COLOR_VALUE;
    if (transitions.view_scene.active) {
        if (incTransition(&transitions.view_scene, delta_time, false))
            opacity = (u8)((f32)MAX_COLOR_VALUE * transitions.view_scene.eased_t);

        if (!transitions.view_frustom_slice.active) {
            edge_color = Color(DarkYellow);
            edge_color.A = opacity;
            drawBox(viewport, edge_color, main_box, main_box_prim, BOX__ALL_SIDES, 0);
        }
        Edge *clipped_edge;
        transformGridVerticesFromObjectToViewSpace(&secondary_viewport, main_grid_prim, main_grid, &clipped_grid.vertices);
        setGridEdgesFromVertices(clipped_grid.edges.uv.u, main_grid->u_segments, clipped_grid.vertices.uv.u.from, clipped_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(clipped_grid.edges.uv.v, main_grid->v_segments, clipped_grid.vertices.uv.v.from, clipped_grid.vertices.uv.v.to);

        edge_color = Color(main_grid_prim->color);
        edge_color.A = opacity;
        clipped_edge = clipped_grid.edges.uv.u;
        for (u8 u = 0; u < main_grid->u_segments; u++, clipped_edge++)
            if (cullAndClipEdge(clipped_edge, &secondary_viewport))
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M), edge_color);

        clipped_edge = clipped_grid.edges.uv.v;
        for (u8 v = 0; v < main_grid->v_segments; v++, clipped_edge++)
            if (cullAndClipEdge(clipped_edge, &secondary_viewport))
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M), edge_color);

        transformBoxVerticesFromObjectToViewSpace(&secondary_viewport, main_box_prim, &main_box->vertices, &clipped_box.vertices);
        setBoxEdgesFromVertices(&clipped_box.edges, &clipped_box.vertices);
        clipped_edge = clipped_box.edges.buffer;
        edge_color = Color(main_box_prim->color);
        edge_color.A = opacity;
        for (u8 i = 0; i < BOX__EDGE_COUNT; i++, clipped_edge++) {
            if (transitions.view_frustom_slice.active && !(
                    clipped_edge == &clipped_box.edges.sides.left_bottom ||
                    clipped_edge == &clipped_box.edges.sides.right_bottom ||
                    clipped_edge == &clipped_box.edges.sides.front_bottom ||
                    clipped_edge == &clipped_box.edges.sides.back_bottom))
                continue;

            if (cullAndClipEdge(clipped_edge, &secondary_viewport))
                drawClippedEdge(viewport, transformedEdge(clipped_edge, M), edge_color);
        }
    }

    if (transitions.grid_XW.active) {
        edge_color = Color(DarkGrey);
        if (incTransition(&transitions.grid_XW, delta_time, false))
            edge_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.grid_XW.eased_t) / 2;

        vec3 P3;
        vec4 P4;
        for (u8 side = 0; side < 2; side++) {
            for (u8 axis = 0; axis < 2; axis++) {
                u8 segment_count = axis ? main_grid->v_segments : main_grid->u_segments;
                for (u8 segment = 0; segment < segment_count; segment++) {
                    P3 = main_grid->vertices.buffer[axis][side][segment];
                    if (transitions.view_frustom_slice.active) {
                        P4.x = 0;
                        P4.y = 0;
                        P4.z = P3.z;
                        P4.w = P3.x;
                        P4 = mulVec4Mat4(P4, M);
                        P3.x = 0;
                        P3.y = P4.w;
                        P3.z = P4.z;
                    } else {
                        P3.y = P3.x;
                        P3.x = 0;
                        mulVec3Mat4(P3, 1.0f, M, &P3);
                    }
                    transformed_grid.vertices.buffer[axis][side][segment] = P3;
                }
            }
        }
        setGridEdgesFromVertices(transformed_grid.edges.uv.u, main_grid->u_segments, transformed_grid.vertices.uv.u.from, transformed_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(transformed_grid.edges.uv.v, main_grid->v_segments, transformed_grid.vertices.uv.v.from, transformed_grid.vertices.uv.v.to);

        drawGrid(viewport, edge_color, &transformed_grid, main_grid_prim, 0);

        for (u8 side = 0; side < 2; side++) {
            for (u8 axis = 0; axis < 2; axis++) {
                u8 segment_count = axis ? main_grid->v_segments : main_grid->u_segments;
                for (u8 segment = 0; segment < segment_count; segment++) {
                    P3 = main_grid->vertices.buffer[axis][side][segment];
                    if (transitions.view_frustom_slice.active) {
                        P4.z = 0;
                        P4.y = 0;
                        P4.x = P3.x;
                        P4.w = P3.z;
                        P4 = mulVec4Mat4(P4, M);
                        P3.x = P4.x;
                        P3.y = P4.w;
                        P3.z = 0;
                    } else {
                        P3.y = P3.z;
                        P3.z = 0;
                        mulVec3Mat4(P3, 1.0f, M, &P3);
                    }
                    transformed_grid.vertices.buffer[axis][side][segment] = P3;
                }
            }
        }
        setGridEdgesFromVertices(transformed_grid.edges.uv.u, main_grid->u_segments, transformed_grid.vertices.uv.u.from, transformed_grid.vertices.uv.u.to);
        setGridEdgesFromVertices(transformed_grid.edges.uv.v, main_grid->v_segments, transformed_grid.vertices.uv.v.from, transformed_grid.vertices.uv.v.to);

        drawGrid(viewport, edge_color, &transformed_grid, main_grid_prim, 0);
    }

    viewport->settings.depth_sort = false;

    if (transitions.ground_diagonal.active) {
        incTransition(&transitions.ground_diagonal, delta_time, false);

        edge_color = Color(Grey);
        edge_color.A = MAX_COLOR_VALUE / 4 * 3;
        edge.from = getVec3Of(0);

        edge.to = Vec3(50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, 0);

        edge.to = Vec3(-50, 0, 50);
        edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
        drawLocalEdge(edge, edge_color, 0);

        if (!transitions.view_frustom_slice.active) {
            edge.to = Vec3(0, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            edge.to = Vec3(0, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            edge.to = Vec3(50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            edge.to = Vec3(-50, 50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            edge.to = Vec3(50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            edge.to = Vec3(-50, -50, 50);
            edge.to = lerpVec3(edge.from, edge.to, transitions.ground_diagonal.eased_t);
            drawLocalEdge(edge, edge_color, 0);

            if (transitions.focal_length_and_plane.active) {
                edge.from = Vec3(0, 0, L);
                edge.to = Vec3(0, L, L);
                drawLocalEdge(edge, default_focal_length_color, 0);

                edge.from = Vec3(0, 0, L);
                edge.to = Vec3(L, 0, L);
                drawLocalEdge(edge, default_focal_length_color, 0);
            }
        }
    }

    if (transitions.view_frustom_slice.active) {
        NDC_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.view_frustom_slice.eased_t);
        vec3 *NDC = NDC_quad.corners, *frs = transforming_quad.corners;
        for (u8 i = 0; i < 4; i++, frs++, NDC++) {
            *frs = convertPositionToObjectSpace(*frs, main_camera_prim);
            *NDC = convertPositionToObjectSpace(*NDC, main_camera_prim);
        }
        for (u8 i = 0; i < 4; i++) {
            edge.from = NDC_quad.corners[i];
            edge.to   = NDC_quad.corners[(i + 1) % 4];

            drawEdge(viewport, NDC_color, &edge, 2);

            edge.from = transforming_quad.corners[i];
            edge.to   = transforming_quad.corners[(i + 1) % 4];
            edge_color = i % 2 ? sides_color : (i ? far_color : near_color);
            edge_color.A = NDC_color.A;
            drawEdge(viewport, edge_color, &edge, 2);
        }
    }

    if (transitions.show_final_scale.active) {
        incTransition(&transitions.show_final_scale, delta_time, false);

        near_color = default_near_color;
        far_color  = default_far_color;
        near_color.A = far_color.A = (u8)((f32)MAX_COLOR_VALUE * transitions.show_final_scale.eased_t);

        edge.from = Vec3(0.8f, F, -N);
        edge.to = Vec3(0.8f, F, 0);
        drawLocalEdge(edge, near_color, 1);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x += 0.1f;
        edge.to = edge.from;
        edge.to.x += 0.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), 0);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(near_color, "N", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(0, F, -N);
        edge.to = Vec3(0, F, -N + F);
        drawLocalEdge(edge, far_color, 1);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x += 0.1f;
        edge.to = edge.from;
        edge.to.x += 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), 0);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(far_color, "F", (i32)edge.to.x, (i32)edge.to.y);

        edge.from = Vec3(-0.8f, F, 0);
        edge.to = Vec3(-0.8f, F, F);
        drawLocalEdge(edge, far_color, 1);
        edge.from = lerpVec3(edge.from, edge.to, 0.5f);
        edge.from.x -= 0.2f;
        edge.to = edge.from;
        edge.to.x -= 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), 0);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(far_color, "F", (i32)edge.to.x, (i32)edge.to.y);

        arrow1.body.from = Vec3(0, F, F-N);
        arrow1.body.to = lerpVec3(arrow1.body.from, Vec3(0, F, F), transitions.scale_arrows.eased_t);
        if (updateArrow(&arrow1)) drawArrow(viewport, Z_color, &arrow1, 1);
        edge.from = lerpVec3(arrow1.body.from, Vec3(0, F, F), 0.5f);
        edge.from.x += 0.2f;
        edge.to = edge.from;
        edge.to.x += 1.5f;
        edge.to.y += 1.25f;
        drawLocalEdge(edge, Color(BrightGrey), 0);
        edge.to = lerpVec3(edge.from, edge.to, 1.25f);
        edge.to   = convertPositionToObjectSpace(edge.to, main_camera_prim);
        edge.from = convertPositionToObjectSpace(edge.from, main_camera_prim);
        projectEdge(&edge, viewport);
        addLabel(far_color, "F", (i32)edge.to.x, (i32)edge.to.y);
        addLabel(Color(White), " / (", (i32)edge.to.x + FONT_WIDTH, (i32)edge.to.y);
        addLabel(far_color, "F", (i32)edge.to.x + FONT_WIDTH * 5, (i32)edge.to.y);
        addLabel(Color(White), " - ", (i32)edge.to.x + FONT_WIDTH * 6, (i32)edge.to.y);
        addLabel(near_color, "N", (i32)edge.to.x + FONT_WIDTH * 9, (i32)edge.to.y);
        addLabel(Color(White), ")", (i32)edge.to.x + FONT_WIDTH * 10, (i32)edge.to.y);
    }

    arrowX.body.from = arrowY.body.from = arrowZ.body.from = getVec3Of(0);
    arrowX.body.to = arrowX_box_prim->position;
    arrowY.body.to = arrowY_box_prim->position;
    arrowZ.body.to = arrowZ_box_prim->position;

    if (transitions.view_frustom_slice.active) {
        arrowX.label = "X";
        arrowY.label = "W";
        arrowZ.label = "Z";
        main_matrix.M.X = Vec4(arrowX_box_prim->position.x, main_matrix.M.X.y, arrowX_box_prim->position.z, arrowX_box_prim->position.y);
        main_matrix.M.Z = Vec4(arrowZ_box_prim->position.x, main_matrix.M.Z.y, arrowZ_box_prim->position.z, arrowZ_box_prim->position.y);
        main_matrix.M.W = Vec4(arrowY_box_prim->position.x, main_matrix.M.W.y, arrowY_box_prim->position.z, arrowY_box_prim->position.y);
    } else {
        arrowX.label = "X";
        arrowY.label = "Y";
        arrowZ.label = "Z";
        main_matrix.M.X = Vec4fromVec3(arrowX_box_prim->position, main_matrix.M.X.w);
        main_matrix.M.Y = Vec4fromVec3(arrowY_box_prim->position, main_matrix.M.Y.w);
        main_matrix.M.Z = Vec4fromVec3(arrowZ_box_prim->position, main_matrix.M.Z.w);
    }
    if (updateArrow(&arrowX)) drawArrow(viewport, X_color, &arrowX, 2);
    if (updateArrow(&arrowY)) drawArrow(viewport, transitions.view_frustom_slice.active ? W_color : Y_color, &arrowY, 2);
    if (updateArrow(&arrowZ)) drawArrow(viewport, Z_color, &arrowZ, 2);
    viewport->settings.depth_sort = true;
}